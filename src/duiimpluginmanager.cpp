/* * This file is part of dui-im-framework *
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

#include "duiimpluginmanager.h"
#include "duiimpluginmanager_p.h"

#include <DuiGConfItem>

#include "duiinputmethodplugin.h"
#include "duiinputcontextdbusconnection.h"

namespace
{
    const QString DefaultPluginLocation("/usr/lib/dui-im-plugins/");

    const QString ConfigRoot          = "/Dui/InputMethods/";
    const QString DuiImPluginPaths    = ConfigRoot + "Paths";
    const QString DuiImPluginActive   = ConfigRoot + "ActivePlugins";
    const QString DuiImPluginDisabled = ConfigRoot + "DisabledPluginFiles";

    const QString PluginRoot          = "/Dui/InputMethods/Plugins/";
    const QString DuiImHandlerToPlugin  = PluginRoot + "Handler";
    const QString DuiImActualHandler    = PluginRoot + "ActualHandler";
}


DuiIMPluginManager::DuiIMPluginManager()
    : QObject(),
      d(new DuiIMPluginManagerPrivate(new DuiInputContextDBusConnection, this))
{
    d->paths     = DuiGConfItem(DuiImPluginPaths).value(QStringList(DefaultPluginLocation)).toStringList();
    d->blacklist = DuiGConfItem(DuiImPluginDisabled).value().toStringList();
    d->active    = DuiGConfItem(DuiImPluginActive).value().toStringList();

    d->loadPlugins();

    d->handlerToPluginConf = new DuiGConfItem(DuiImHandlerToPlugin, this);
    d->actualHandlerConf = new DuiGConfItem(DuiImActualHandler, this);

    connect(d->handlerToPluginConf, SIGNAL(valueChanged()), this, SLOT(reloadHandlerMap()));
    connect(d->actualHandlerConf, SIGNAL(valueChanged()), this, SLOT(activateHandler()));

    reloadHandlerMap();
    activateHandler();

    connect(&d->deleteImTimer, SIGNAL(timeout()), this, SLOT(deleteInactiveIM()));
}


DuiIMPluginManager::~DuiIMPluginManager()
{
    delete d;
    d = 0;
}


void DuiIMPluginManager::reloadHandlerMap()
{
    QList<QString> handlers = d->handlerToPluginConf->listEntries();
    const QString key = d->handlerToPluginConf->key() + "/";

    foreach (const QString &handlerName, handlers) {
        QStringList path = handlerName.split("/");
        QString pluginName = DuiGConfItem(handlerName).value().toString();
        d->addHandlerMap((DuiIMHandlerState)path.last().toInt(), pluginName);
    }
}


void DuiIMPluginManager::activateHandler()
{
    QStringList values = d->actualHandlerConf->value().toStringList();
    QSet<DuiIMHandlerState> handlers;

    d->convertAndFilterHandlers(values, &handlers);

    if (!handlers.isEmpty()) {
        d->setActiveHandlers(handlers);
        d->deleteImTimer.start();
    }
}


void DuiIMPluginManager::deleteInactiveIM()
{
    d->deleteInactiveIM();
}


QStringList DuiIMPluginManager::loadedPluginsNames() const
{
    return d->loadedPluginsNames();
}


QStringList DuiIMPluginManager::activePluginsNames() const
{
    return d->activePluginsNames();
}


QStringList DuiIMPluginManager::activeInputMethodsNames() const
{
    return d->activeInputMethodsNames();
}


void DuiIMPluginManager::setDeleteIMTimeout(int timeout)
{
    d->deleteImTimer.setInterval(timeout);
}

