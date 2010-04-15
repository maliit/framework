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

#ifndef MIMPLUGINMANAGER_P_H
#define MIMPLUGINMANAGER_P_H

#include <QList>
#include <QStringList>
#include <QMap>
#include <QTimer>
#include <QSet>

#include "mimhandlerstate.h"

class MInputMethodPlugin;
class MInputContextConnection;
class MIMPluginManager;
class MGConfItem;
class MInputMethodBase;

/* Internal class only! Interfaces here change, internal developers only*/
class MIMPluginManagerPrivate
{

public:
    MIMPluginManagerPrivate(MInputContextConnection *connection, MIMPluginManager *p);
    virtual ~MIMPluginManagerPrivate();

    bool activatePlugin(const QString &name);
    void activatePlugin(MInputMethodPlugin *plugin);
    void loadPlugins();
    void addHandlerMap(MIMHandlerState state, const QString &pluginName);
    void setActiveHandlers(const QSet<MIMHandlerState> &states);
    QSet<MIMHandlerState> activeHandlers() const;
    void deleteInactiveIM();
    void deactivatePlugin(MInputMethodPlugin *plugin);
    void convertAndFilterHandlers(const QStringList &handlerNames, QSet<MIMHandlerState> *handlers);

    QStringList loadedPluginsNames() const;
    QStringList activePluginsNames() const;
    QStringList activeInputMethodsNames() const;

public:
    MIMPluginManager *parent;
    MInputContextConnection *mICConnection;

    typedef QMap<MInputMethodPlugin *, MInputMethodBase *> Plugins;
    Plugins plugins;
    typedef QSet<MInputMethodPlugin *> ActivePlugins;
    ActivePlugins activePlugins;

    QStringList paths;
    QStringList blacklist;
    QStringList active;
    typedef QMap<MIMHandlerState, MInputMethodPlugin *> HandlerMap;
    HandlerMap handlerToPlugin;

    MGConfItem *handlerToPluginConf;
    MGConfItem *imAccessoryEnabledConf;

    QTimer deleteImTimer;
};

#endif
