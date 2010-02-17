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

#ifndef DUIIMPLUGINMANAGER_P_H
#define DUIIMPLUGINMANAGER_P_H

#include <QList>
#include <QStringList>
#include <QMap>
#include <QTimer>
#include <QSet>

#include "duiimhandlerstate.h"

class DuiInputMethodPlugin;
class DuiInputContextConnection;
class DuiIMPluginManager;
class DuiGConfItem;
class DuiInputMethodBase;

/* Internal class only! Interfaces here change, internal developers only*/
class DuiIMPluginManagerPrivate
{

public:
    DuiIMPluginManagerPrivate(DuiInputContextConnection *connection, DuiIMPluginManager *p);
    virtual ~DuiIMPluginManagerPrivate();

    bool activatePlugin(const QString &name);
    void activatePlugin(DuiInputMethodPlugin *plugin);
    void loadPlugins();
    void addHandlerMap(DuiIMHandlerState state, const QString &pluginName);
    void setActiveHandlers(const QSet<DuiIMHandlerState> &states);
    void deleteInactiveIM();
    void deactivatePlugin(DuiInputMethodPlugin *plugin);
    void convertAndFilterHandlers(const QStringList &handlerNames, QSet<DuiIMHandlerState> *handlers);

    QStringList loadedPluginsNames() const;
    QStringList activePluginsNames() const;
    QStringList activeInputMethodsNames() const;

public:
    DuiIMPluginManager *parent;
    DuiInputContextConnection *duiICConnection;

    typedef QMap<DuiInputMethodPlugin *, DuiInputMethodBase *> Plugins;
    Plugins plugins;
    typedef QSet<DuiInputMethodPlugin *> ActivePlugins;
    ActivePlugins activePlugins;

    QStringList paths;
    QStringList blacklist;
    QStringList active;
    typedef QMap<DuiIMHandlerState, DuiInputMethodPlugin *> HandlerMap;
    HandlerMap handlerToPlugin;

    DuiGConfItem *handlerToPluginConf;
    DuiGConfItem *actualHandlerConf;

    QTimer deleteImTimer;
};

#endif
