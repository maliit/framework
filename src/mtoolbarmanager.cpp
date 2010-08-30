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

#include <MLocale>
#include <QVariant>
#include <QDebug>

namespace {
    const QString StandardToolbar = QString::fromLatin1("/usr/share/meegotouch/imtoolbars/imframework/standard.xml");
}

MToolbarManager *MToolbarManager::toolbarMgrInstance = 0;

MToolbarManager::MToolbarManager()
    : copyPasteStatus(MInputMethod::InputMethodNoCopyPaste)
{
    createStandardObjects();
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
    MInputMethod::CopyPasteState newStatus = MInputMethod::InputMethodNoCopyPaste;
    MInputMethod::ActionType actionType = MInputMethod::ActionUndefined;

    if (!copyPaste) {
        return;
    }

    if (copyAvailable) {
        newStatus = MInputMethod::InputMethodCopy;
    } else if (pasteAvailable) {
        newStatus = MInputMethod::InputMethodPaste;
    }

    if (copyPasteStatus == newStatus)
        return;

    copyPasteStatus = newStatus;
    switch (newStatus) {
    case MInputMethod::InputMethodNoCopyPaste:
        copyPaste->setVisible(false);
        break;
    case MInputMethod::InputMethodCopy:
        copyPaste->setVisible(true);
        //% "Copy"
        copyPaste->setTextId("qtn_comm_copy");
        actionType = MInputMethod::ActionCopy;
        break;
    case MInputMethod::InputMethodPaste:
        copyPaste->setVisible(true);
        //% "Paste"
        copyPaste->setTextId("qtn_comm_paste");
        actionType = MInputMethod::ActionPaste;
        break;
    }
    if (!copyPaste->actions().isEmpty()) {
        copyPaste->actions().at(0)->setType(actionType);
    }
}

QSharedPointer<MToolbarData> MToolbarManager::createToolbar(const QString &name)
{
    // load a toolbar
    QSharedPointer<MToolbarData> toolbar(new MToolbarData);
    const bool loaded = toolbar->loadNokiaToolbarXml(name);

    if (!loaded) {
        qWarning() << "ToolbarsManager: toolbar load error: "
                   << name;
        toolbar.clear();
    } else {
        toolbar->sort();
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

        foreach (QSharedPointer<MToolbarItem> item, standardToolbar->allItems()) {
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
                copyPaste->setVisible(false);
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

    if (landscape) {
        addStandardButtons(landscape);
    }

    if (portrait && portrait != landscape) {
        addStandardButtons(portrait);
    }
}

void MToolbarManager::addStandardButtons(const QSharedPointer<MToolbarLayout> &layout)
{
    if (layout->rows().isEmpty()) {
        return;
    }

    QSharedPointer<MToolbarRow> row = layout->rows().last();
    if (copyPaste) {
        row->append(copyPaste);
    }
    if (close) {
        row->append(close);
    }
    row->sort();
}

void MToolbarManager::registerToolbar(const MToolbarId &id, const QString &fileName)
{
    qDebug() << __PRETTY_FUNCTION__;
    if (!id.isValid() || fileName.isEmpty() || toolbars.contains(id))
        return;

    QSharedPointer<MToolbarData> toolbarData = createToolbar(fileName);
    if (toolbarData) {
        addStandardButtons(toolbarData);
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

