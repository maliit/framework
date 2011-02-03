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


#include "mtoolbarmanager.h"
#include "mtoolbarid.h"
#include "mtoolbarlayout.h"
#include "mkeyoverride.h"

#include <MLocale>
#include <QVariant>
#include <QDebug>

namespace {
    const QString StandardToolbar = QString::fromLatin1("/usr/share/meegoimframework/imtoolbars/standard.xml");
    const char * const PreferredDomainSettingName("/meegotouch/inputmethods/preferred_domain");
    const char * const DomainItemName("_domain");
}

MToolbarManager *MToolbarManager::toolbarMgrInstance = 0;

MToolbarManager::MToolbarManager()
    : copyPasteStatus(MInputMethod::InputMethodNoCopyPaste),
      preferredDomainSetting(PreferredDomainSettingName)
{
    createStandardObjects();
    connect(&preferredDomainSetting, SIGNAL(valueChanged()), this, SLOT(handlePreferredDomainUpdate()));
}

MToolbarManager::~MToolbarManager()
{
}

void MToolbarManager::createInstance()
{
    Q_ASSERT(!toolbarMgrInstance);
    if (!toolbarMgrInstance) {
        toolbarMgrInstance = new MToolbarManager;
    }
}

void MToolbarManager::destroyInstance()
{
    Q_ASSERT(toolbarMgrInstance);
    delete toolbarMgrInstance;
    toolbarMgrInstance = 0;
}

QList<MToolbarId> MToolbarManager::toolbarList() const
{
    return toolbars.keys();
}

QSharedPointer<MToolbarData> MToolbarManager::toolbarData(const MToolbarId &id) const
{
    ToolbarContainer::iterator iterator(toolbars.find(id));

    if (iterator != toolbars.end())
        return iterator.value();
    return QSharedPointer<MToolbarData>();
}

bool  MToolbarManager::contains(const MToolbarId &id) const
{
    return toolbars.contains(id);
}

void MToolbarManager::setCopyPasteState(bool copyAvailable, bool pasteAvailable)
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

QSharedPointer<MToolbarData> MToolbarManager::createToolbar(const QString &name)
{
    // load a toolbar
    QSharedPointer<MToolbarData> toolbar(new MToolbarData);
    const bool loaded = toolbar->loadToolbarXml(name);

    if (!loaded) {
        qWarning() << "ToolbarsManager: toolbar load error: "
                   << name;
        toolbar.clear();
    }

    return toolbar;
}

void MToolbarManager::createStandardObjects()
{
    // This code assumes that StandardToolbar provides exactly two buttons: copy/paste and close.
    // That file is controlled by us, so we can rely on this assertion.
    standardToolbar = createToolbar(StandardToolbar);

    if (standardToolbar) {
        toolbars.insert(MToolbarId::standardToolbarId(), standardToolbar);

        foreach (QSharedPointer<MToolbarItem> item, standardToolbar->items()) {
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

void MToolbarManager::addStandardButtons(const QSharedPointer<MToolbarData> &toolbarData)
{
    QSharedPointer<MToolbarLayout> landscape = toolbarData->layout(M::Landscape).constCast<MToolbarLayout>();
    QSharedPointer<MToolbarLayout> portrait = toolbarData->layout(M::Portrait).constCast<MToolbarLayout>();

    if (!toolbarData || !standardToolbar) {
        return;
    }

    if (landscape) {
        addStandardButtons(landscape, toolbarData);
    }

    if (portrait && portrait != landscape) {
        addStandardButtons(portrait, toolbarData);
    }
}

void MToolbarManager::addStandardButtons(const QSharedPointer<MToolbarLayout> &layout,
                                         const QSharedPointer<MToolbarData> &toolbarData)
{
    foreach (const QSharedPointer<MToolbarItem> &item, standardToolbar->items()) {
        if (!toolbarData->refusedNames().contains(item->name())) {
            toolbarData->append(layout, item);
        }
    }
}

void MToolbarManager::handlePreferredDomainUpdate()
{
    foreach (QSharedPointer<MToolbarData> toolbar, toolbars) {
        updateDomain(toolbar);
    }
}

void MToolbarManager::updateDomain(QSharedPointer<MToolbarData> &toolbar)
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

void MToolbarManager::registerToolbar(const MToolbarId &id, const QString &fileName)
{
    qDebug() << __PRETTY_FUNCTION__;
    if (!id.isValid() || fileName.isEmpty() || toolbars.contains(id))
        return;

    QSharedPointer<MToolbarData> toolbarData = createToolbar(fileName);
    if (toolbarData) {
        addStandardButtons(toolbarData);
        updateDomain(toolbarData);
    } else {
        toolbarData = standardToolbar;
    }

    if (toolbarData) {
        toolbars.insert(id, toolbarData);
    }
}

void MToolbarManager::unregisterToolbar(const MToolbarId &id)
{
    qDebug() << __PRETTY_FUNCTION__;
    if (!toolbars.contains(id))
        return;

    toolbars.remove(id);
}

void MToolbarManager::setToolbarItemAttribute(const MToolbarId &id,
                                              const QString &itemName,
                                              const QString &attribute,
                                              const QVariant &value)
{
    qDebug() << __PRETTY_FUNCTION__;
    if (!id.isValid() || attribute.isEmpty() || !value.isValid())
        return;

    QSharedPointer<MToolbarData> toolbar = toolbarData(id);

    if (!toolbar) {
        return;
    }

    QSharedPointer<MToolbarItem> item = toolbar->item(itemName);

    if (!item) {
        return;
    }

    const QByteArray byteArray = attribute.toLatin1();
    const char * const c_str = byteArray.data();
    item->setProperty(c_str, value);
}

QList<QSharedPointer<MKeyOverride> > MToolbarManager::keyOverrides(const MToolbarId &id) const
{
    QList<QSharedPointer<MKeyOverride> > overrides;
    QSharedPointer<MToolbarData> toolbar = toolbarData(id);
    if (toolbar) {
        overrides = toolbar->keyOverrides();
    }

    return overrides;
}

void MToolbarManager::setKeyAttribute(const MToolbarId &id,
                                      const QString &keyId,
                                      const QString &attribute,
                                      const QVariant &value)
{
    qDebug() << __PRETTY_FUNCTION__;
    if (!id.isValid() || attribute.isEmpty() || !value.isValid())
        return;

    QSharedPointer<MToolbarData> toolbar = toolbarData(id);

    if (!toolbar) {
        return;
    }

    // create key override if not exist.
    toolbar->createKeyOverride(keyId);

    QSharedPointer<MKeyOverride> keyOverride = toolbar->keyOverride(keyId);
    if (!keyOverride) {
        return;
    }

    const QByteArray byteArray = attribute.toLatin1();
    const char * const c_str = byteArray.data();
    keyOverride->setProperty(c_str, value);
}
