/* * This file is part of meego-im-framework *
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * Contact: Nokia Corporation (directui@nokia.com)
 *
 * If you have questions regarding the use of this file, please contact
 * Nokia at directui@nokia.com.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 */


#include "mattributeextensionmanager.h"
#include "mattributeextension.h"
#include "mkeyoverridedata.h"
#include "mkeyoverride.h"

#include <QVariant>
#include <QFileInfo>
#include <QFile>
#include <QDebug>

namespace {
    const QString DefaultConfigurationPath = QString::fromLatin1(MALIIT_EXTENSIONS_DIR);
    const char * const PreferredDomainSettingName(MALIIT_CONFIG_ROOT"preferred_domain");
    const char * const DomainItemName("_domain");
    const char * const KeysExtensionString("/keys");
    const char * const ToolbarExtensionString("/toolbar");
    const char * const GlobalExtensionString("/");

    const char * const ToolbarIdAttribute = "toolbarId";
    const char * const ToolbarAttribute = "toolbar";
    const char * const FocusStateAttribute = "focusState";
}

MAttributeExtensionManager::MAttributeExtensionManager()
    : copyPasteStatus(Maliit::InputMethodNoCopyPaste)
{
}

MAttributeExtensionManager::~MAttributeExtensionManager()
{
}

QList<MAttributeExtensionId> MAttributeExtensionManager::attributeExtensionIdList() const
{
    return attributeExtensions.keys();
}

QSharedPointer<MAttributeExtension> MAttributeExtensionManager::attributeExtension(const MAttributeExtensionId &id) const
{
    AttributeExtensionContainer::const_iterator iterator(attributeExtensions.find(id));

    if (iterator != attributeExtensions.end())
        return iterator.value();
    return QSharedPointer<MAttributeExtension>();
}

bool  MAttributeExtensionManager::contains(const MAttributeExtensionId &id) const
{
    return attributeExtensions.contains(id);
}

void MAttributeExtensionManager::setCopyPasteState(bool copyAvailable, bool pasteAvailable)
{
    Maliit::CopyPasteState newStatus = Maliit::InputMethodNoCopyPaste;

    if (copyAvailable) {
        newStatus = Maliit::InputMethodCopy;
    } else if (pasteAvailable) {
        newStatus = Maliit::InputMethodPaste;
    }

    if (copyPasteStatus == newStatus)
        return;

    copyPasteStatus = newStatus;
    switch (newStatus) {
    case Maliit::InputMethodNoCopyPaste:
        break;
    case Maliit::InputMethodCopy:
        break;
    case Maliit::InputMethodPaste:
        break;
    }
}

void MAttributeExtensionManager::registerAttributeExtension(const MAttributeExtensionId &id, const QString &fileName)
{
    if (!id.isValid() || attributeExtensions.contains(id))
        return;

    // Only register default extension in the case of empty string.
    // Don't register extension if user makes a typo in the file name.
    if (!fileName.isEmpty()) {
        QString absoluteFileName = fileName;
        QFileInfo info(absoluteFileName);
        if (info.isRelative())
            absoluteFileName = DefaultConfigurationPath + info.fileName();
        if (!QFile::exists(absoluteFileName))
            return;
    }

    QSharedPointer<MAttributeExtension> attributeExtension(new MAttributeExtension(id, fileName));
    if (attributeExtension) {
        attributeExtensions.insert(id, attributeExtension);
    }
}

void MAttributeExtensionManager::unregisterAttributeExtension(const MAttributeExtensionId &id)
{
    AttributeExtensionContainer::iterator iterator(attributeExtensions.find(id));

    if (iterator == attributeExtensions.end()) {
        return;
    }

    attributeExtensions.remove(id);
}

void MAttributeExtensionManager::setToolbarItemAttribute(const MAttributeExtensionId &id,
                                              const QString &itemName,
                                              const QString &attribute,
                                              const QVariant &value)
{
    setExtendedAttribute(id, ToolbarExtensionString, itemName,
                         attribute, value);
}

QMap<QString, QSharedPointer<MKeyOverride> > MAttributeExtensionManager::keyOverrides(
        const MAttributeExtensionId &id) const
{
    QList<QSharedPointer<MKeyOverride> > overrides;
    QSharedPointer<MAttributeExtension> extension = attributeExtension(id);
    if (extension) {
        overrides = extension->keyOverrideData()->keyOverrides();
    }
    QMap<QString, QSharedPointer<MKeyOverride> > overridesMap;
    Q_FOREACH (const QSharedPointer<MKeyOverride> &override, overrides) {
        overridesMap.insert(override->keyId(), override);
    }
    return overridesMap;
}

void MAttributeExtensionManager::setExtendedAttribute(const MAttributeExtensionId &id,
                                                      const QString &target,
                                                      const QString &targetItem,
                                                      const QString &attribute,
                                                      const QVariant &value)
{
    if (target == GlobalExtensionString) {
        Q_EMIT globalAttributeChanged(id, targetItem, attribute, value);
        return;
    }

    if (!id.isValid() || attribute.isEmpty() || targetItem.isEmpty() || !value.isValid())
        return;

    QSharedPointer<MAttributeExtension> extension = attributeExtension(id);

    if (!extension) {
        return;
    }

    if (target == KeysExtensionString) {
        // create key override if not exist.
        bool newKeyOverrideCreated = extension->keyOverrideData()->createKeyOverride(targetItem);
        QSharedPointer<MKeyOverride> keyOverride = extension->keyOverrideData()->keyOverride(targetItem);

        Q_ASSERT(keyOverride);
        const QByteArray byteArray = attribute.toLatin1();
        const char * const c_str = byteArray.data();

        // Ignore l10n lengthvariants in QStrings for labels, always pick longest variant (first)
        if (attribute == "label") {
            QString label = value.toString();
            label = label.split(QChar(0x9c)).first();
            const QVariant newValue(label);
            keyOverride->setProperty(c_str, newValue);
        } else {
            keyOverride->setProperty(c_str, value);
        }

        // Q_EMIT signal to notify the new key override is created.
        if (newKeyOverrideCreated) {
            Q_EMIT keyOverrideCreated();
        }
    } else {
        qWarning() << "Invalid or incompatible attribute extension target:" << target;
    }
}


void MAttributeExtensionManager::handleClientDisconnect(unsigned int clientId)
{
    // unregister toolbars registered by the lost connection
    const QString service(QString::number(clientId));
    QSet<MAttributeExtensionId>::iterator i(attributeExtensionIds.begin());
    while (i != attributeExtensionIds.end()) {
        if ((*i).service() == service) {
            unregisterAttributeExtension(*i);
            i = attributeExtensionIds.erase(i);
        } else {
            ++i;
        }
    }
}

void MAttributeExtensionManager::handleExtendedAttributeUpdate(unsigned int clientId, int id,
                                   const QString &target, const QString &targetName,
                                   const QString &attribute, const QVariant &value)
{
    MAttributeExtensionId globalId(id, QString::number(clientId));
    if (globalId.isValid() && attributeExtensionIds.contains(globalId)) {
        setExtendedAttribute(globalId, target, targetName, attribute, value);
    }
}

void MAttributeExtensionManager::handleAttributeExtensionRegistered(unsigned int clientId,
                                                                  int id, const QString &attributeExtension)
{
    MAttributeExtensionId globalId(id, QString::number(clientId));
    if (globalId.isValid() && !attributeExtensionIds.contains(globalId)) {
        registerAttributeExtension(globalId, attributeExtension);
        attributeExtensionIds.insert(globalId);
    }
}

void MAttributeExtensionManager::handleAttributeExtensionUnregistered(unsigned int clientId, int id)
{
    MAttributeExtensionId globalId(id, QString::number(clientId));
    if (globalId.isValid() && attributeExtensionIds.contains(globalId)) {
        unregisterAttributeExtension(globalId);
        attributeExtensionIds.remove(globalId);
    }
}

void MAttributeExtensionManager::handleWidgetStateChanged(unsigned int clientId,
                                                          const QMap<QString, QVariant> &newState,
                                                          const QMap<QString, QVariant> &oldState,
                                                          bool focusChanged)

{
    Q_UNUSED(oldState);
    // toolbar change
    MAttributeExtensionId oldAttributeExtensionId;
    MAttributeExtensionId newAttributeExtensionId;
    oldAttributeExtensionId = attributeExtensionId;

    QVariant variant = newState[ToolbarIdAttribute];
    if (variant.isValid()) {
        // map toolbar id from local to global
        newAttributeExtensionId = MAttributeExtensionId(variant.toInt(), QString::number(clientId));
    }
    if (!newAttributeExtensionId.isValid()) {
        newAttributeExtensionId = MAttributeExtensionId::standardAttributeExtensionId();
    }

    variant = newState[FocusStateAttribute];
    if (not variant.isValid()) {
        qCritical() << __PRETTY_FUNCTION__ << "Invalid focus state";
    }
    bool widgetFocusState = variant.toBool();

    // compare the toolbar id (global)
    if (oldAttributeExtensionId != newAttributeExtensionId) {
        QString toolbarFile = newState[ToolbarAttribute].toString();
        if (!contains(newAttributeExtensionId) && !toolbarFile.isEmpty()) {
            // register toolbar if toolbar manager does not contain it but
            // toolbar file is not empty. This can reload the toolbar data
            // if im-uiserver crashes.
            // XXX: should not happen, the input context is reponsible for registering
            // and resending the toolbar information on server reconnect
            qWarning() << "Unregistered toolbar found in widget information";

            variant = newState[ToolbarIdAttribute];
            if (variant.isValid()) {
                const int toolbarLocalId = variant.toInt();
                // FIXME: brittle to call the signal handler directly like this
                handleAttributeExtensionRegistered(clientId, toolbarLocalId, toolbarFile);
            }
        }
        Q_EMIT attributeExtensionIdChanged(newAttributeExtensionId);
        // store the new used toolbar id(global).
        attributeExtensionId = newAttributeExtensionId;
    }
    // this happens when we focus on a text widget with no attribute extensions.
    else if (focusChanged && widgetFocusState)
    {
        Q_EMIT attributeExtensionIdChanged(newAttributeExtensionId);
    }
}
