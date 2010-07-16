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

MToolbarManager *MToolbarManager::toolbarMgrInstance = 0;

MToolbarManager::MToolbarManager()
{
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

void MToolbarManager::registerToolbar(const MToolbarId &id, const QString &fileName)
{
    qDebug() << __PRETTY_FUNCTION__;
    if (!id.isValid() || fileName.isEmpty() || toolbars.contains(id))
        return;

    QSharedPointer<MToolbarData> toolbarData = createToolbar(fileName);
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

