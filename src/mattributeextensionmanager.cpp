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
#include "mattributeextensionid.h"
#include "mtoolbardata.h"
#include "mtoolbarlayout.h"
#include "mkeyoverridedata.h"
#include "mkeyoverride.h"

#include <QVariant>
#include <QFileInfo>
#include <QFile>
#include <QDebug>

namespace {
    const QString DefaultConfigurationPath   = QString::fromLatin1("/usr/share/meegoimframework/imtoolbars/");
    const char * const PreferredDomainSettingName("/meegotouch/inputmethods/preferred_domain");
    const char * const DomainItemName("_domain");
    const char * const KeysExtensionString("/keys");
    const char * const ToolbarExtensionString("/toolbar");
}

MAttributeExtensionManager *MAttributeExtensionManager::attributeExtensionMgrInstance = 0;

MAttributeExtensionManager::MAttributeExtensionManager()
    : copyPasteStatus(MInputMethod::InputMethodNoCopyPaste),
      preferredDomainSetting(PreferredDomainSettingName)
{
    createStandardObjects();
    connect(&preferredDomainSetting, SIGNAL(valueChanged()), this, SLOT(handlePreferredDomainUpdate()));
}

MAttributeExtensionManager::~MAttributeExtensionManager()
{
}

void MAttributeExtensionManager::destroyInstance()
{
    Q_ASSERT(attributeExtensionMgrInstance);
    delete attributeExtensionMgrInstance;
    attributeExtensionMgrInstance = 0;
}

QList<MAttributeExtensionId> MAttributeExtensionManager::attributeExtensionIdList() const
{
    return attributeExtensions.keys();
}

QSharedPointer<MAttributeExtension> MAttributeExtensionManager::attributeExtension(const MAttributeExtensionId &id) const
{
    AttributeExtensionContainer::iterator iterator(attributeExtensions.find(id));

    if (iterator != attributeExtensions.end())
        return iterator.value();
    return QSharedPointer<MAttributeExtension>();
}

QSharedPointer<MToolbarData> MAttributeExtensionManager::toolbarData(const MAttributeExtensionId &id) const
{
    AttributeExtensionContainer::iterator iterator(attributeExtensions.find(id));

    if (iterator != attributeExtensions.end())
        return iterator.value()->toolbarData();
    return QSharedPointer<MToolbarData>();
}

bool  MAttributeExtensionManager::contains(const MAttributeExtensionId &id) const
{
    return attributeExtensions.contains(id);
}

void MAttributeExtensionManager::setCopyPasteState(bool copyAvailable, bool pasteAvailable)
{
    if (!copyPaste) {
        return;
    }

    MInputMethod::CopyPasteState newStatus = MInputMethod::InputMethodNoCopyPaste;
    MInputMethod::ActionType actionType = MInputMethod::ActionUndefined;

    if (copyAvailable) {
        newStatus = MInputMethod::InputMethodCopy;
    } else if (pasteAvailable) {
        newStatus = MInputMethod::InputMethodPaste;
    }

    if (copyPasteStatus == newStatus)
        return;

    bool enabled = false;
    QString textId("qtn_comm_copy");

    copyPasteStatus = newStatus;
    switch (newStatus) {
    case MInputMethod::InputMethodNoCopyPaste:
        break;
    case MInputMethod::InputMethodCopy:
        enabled = true;
        actionType = MInputMethod::ActionCopy;
        break;
    case MInputMethod::InputMethodPaste:
        enabled = true;
        textId = "qtn_comm_paste";
        actionType = MInputMethod::ActionPaste;
        break;
    }
    copyPaste->setTextId(textId);
    copyPaste->setEnabled(enabled);
    if (!copyPaste->actions().isEmpty()) {
        copyPaste->actions().first()->setType(actionType);
    }
}

void MAttributeExtensionManager::createStandardObjects()
{
    // TODO: standard buttons are not used anymore and should be removed
    // This code assumes that StandardToolbar provides exactly two buttons: copy/paste and close.
    // That file is controlled by us, so we can rely on this assertion.
    standardAttributeExtension = QSharedPointer<MAttributeExtension>(new MAttributeExtension(MAttributeExtensionId::standardAttributeExtensionId(),
                                                                                             QString()));

    if (standardAttributeExtension && standardAttributeExtension->toolbarData()) {
        attributeExtensions.insert(MAttributeExtensionId::standardAttributeExtensionId(), standardAttributeExtension);

        foreach (QSharedPointer<MToolbarItem> item, standardAttributeExtension->toolbarData()->items()) {
            item->setCustom(false);
            QList<QSharedPointer<MToolbarItemAction> > actions = item->actions();
            if (actions.isEmpty()) {
                continue; // should never happen
            }

            switch (actions.at(0)->type()) {
            case MInputMethod::ActionClose:
                close = item;
                break;
            case MInputMethod::ActionCopyPaste:
                copyPaste = item;
                // set initial state
                copyPaste->setVisible(true);
                copyPaste->setEnabled(false);
                copyPaste->setTextId("qtn_comm_copy");
                copyPaste->actions().first()->setType(MInputMethod::ActionUndefined);
                break;
            default:
                break;
            }
        }
    }
}

void MAttributeExtensionManager::addStandardButtons(const QSharedPointer<MToolbarData> &toolbarData)
{
    if (!toolbarData || !standardAttributeExtension || !standardAttributeExtension->toolbarData()) {
        return;
    }

    QSharedPointer<MToolbarLayout> landscape = toolbarData->layout(MInputMethod::Landscape).constCast<MToolbarLayout>();
    QSharedPointer<MToolbarLayout> portrait = toolbarData->layout(MInputMethod::Portrait).constCast<MToolbarLayout>();

    if (landscape) {
        addStandardButtons(landscape, toolbarData);
    }

    if (portrait && portrait != landscape) {
        addStandardButtons(portrait, toolbarData);
    }
}

void MAttributeExtensionManager::addStandardButtons(const QSharedPointer<MToolbarLayout> &layout,
                                         const QSharedPointer<MToolbarData> &toolbarData)
{
    foreach (const QSharedPointer<MToolbarItem> &item, standardAttributeExtension->toolbarData()->items()) {
        if (!toolbarData->refusedNames().contains(item->name())) {
            toolbarData->append(layout, item);
        }
    }
}

void MAttributeExtensionManager::handlePreferredDomainUpdate()
{
    foreach (QSharedPointer<MAttributeExtension> attributeExtension, attributeExtensions.values()) {
        QSharedPointer<MToolbarData> toolbar = attributeExtension->toolbarData();
        updateDomain(toolbar);
    }
}

void MAttributeExtensionManager::updateDomain(QSharedPointer<MToolbarData> &toolbar)
{
    const QString domain(preferredDomainSetting.value().toString());
    if (domain.isEmpty()) {
        return;
    }

    QSharedPointer<MToolbarItem> item(toolbar->item(DomainItemName));
    if (!item) {
        return;
    }

    QList<QSharedPointer<MToolbarItemAction> > actions(item->actions());
    if (actions.length() != 1 || actions[0]->type() != MInputMethod::ActionSendString) {
        return;
    }

    actions[0]->setText(domain);
    item->setText(domain);
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
        addStandardButtons(attributeExtension->toolbarData());
        QSharedPointer<MToolbarData> toolbar = attributeExtension->toolbarData();
        updateDomain(toolbar);
        attributeExtensions.insert(id, attributeExtension);
    }
}

void MAttributeExtensionManager::unregisterAttributeExtension(const MAttributeExtensionId &id)
{
    if (!attributeExtensions.contains(id))
        return;

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
    foreach (const QSharedPointer<MKeyOverride> &override, overrides) {
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

        // emit signal to notify the new key override is created.
        if (newKeyOverrideCreated) {
            emit keyOverrideCreated();
        }
    } else if (target == ToolbarExtensionString) {
        QSharedPointer<MToolbarData> toolbar = extension->toolbarData();
        if (!toolbar) {
            qWarning() << "Can not find toolbar data!";
            return;
        }
        QSharedPointer<MToolbarItem> item = toolbar->item(targetItem);
        if (!item) {
            return;
        }
        const QByteArray byteArray = attribute.toLatin1();
        const char * const c_str = byteArray.data();
        item->setProperty(c_str, value);
    } else {
        qWarning() << "Invalid or incompatible attribute extension target:" << target;
    }
}
