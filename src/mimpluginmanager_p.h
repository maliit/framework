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

#include <QObject>
#include <QList>
#include <QStringList>
#include <QMap>
#include <QTimer>
#include <QSet>
#include <QRegion>
#include <MDialog>

#include "mimhandlerstate.h"
#include "mimdirection.h"

class MInputMethodPlugin;
class MInputContextConnection;
class MIMPluginManager;
class MGConfItem;
class MInputMethodBase;
class MContentItem;
class MContainer;
class MInputMethodSettingsBase;

/* Internal class only! Interfaces here change, internal developers only*/
class MIMSettingDialog : public MDialog
{
    Q_OBJECT
public:
    MIMSettingDialog(const QString &title, M::StandardButtons buttons);

Q_SIGNALS:
    void languageChanged();

protected:
    //! reimp
    virtual void retranslateUi();
    virtual void orientationChangeEvent(MOrientationChangeEvent *event);
    //! reimp_end
};

class MIMPluginManagerPrivate
{
    Q_DECLARE_PUBLIC(MIMPluginManager)
public:
    typedef QSet<MIMHandlerState> PluginState;
    struct PluginDescription {
        QString fileName;
        MInputMethodBase *inputMethod;
        PluginState state;
        M::InputMethodSwitchDirection lastSwitchDirection;
    };
    typedef QMap<MInputMethodPlugin *, PluginDescription> Plugins;
    typedef QSet<MInputMethodPlugin *> ActivePlugins;
    typedef QMap<MIMHandlerState, MInputMethodPlugin *> HandlerMap;

    MIMPluginManagerPrivate(MInputContextConnection *connection, MIMPluginManager *p);
    virtual ~MIMPluginManagerPrivate();

    bool activatePlugin(const QString &name);
    void activatePlugin(MInputMethodPlugin *plugin);
    void loadPlugins();
    bool loadPlugin(const QString &fileName);
    void addHandlerMap(MIMHandlerState state, const QString &pluginName);
    void setActiveHandlers(const QSet<MIMHandlerState> &states);
    QSet<MIMHandlerState> activeHandlers() const;
    void deleteInactiveIM();
    void deactivatePlugin(MInputMethodPlugin *plugin);
    void convertAndFilterHandlers(const QStringList &handlerNames, QSet<MIMHandlerState> *handlers);

    void replacePlugin(M::InputMethodSwitchDirection direction, Plugins::iterator initiator,
                       Plugins::iterator replacement);
    bool switchPlugin(M::InputMethodSwitchDirection direction, MInputMethodBase *initiator);
    bool switchPlugin(const QString &name, MInputMethodBase *initiator);
    bool doSwitchPlugin(M::InputMethodSwitchDirection direction,
                        Plugins::iterator source,
                        Plugins::iterator replacement);
    void changeHandlerMap(MInputMethodPlugin *origin,
                          MInputMethodPlugin *replacement,
                          QSet<MIMHandlerState> states);

    QStringList loadedPluginsNames() const;
    QStringList activePluginsNames() const;
    QStringList activeInputMethodsNames() const;
    void loadHandlerMap();
    MInputMethodPlugin *activePlugin(MIMHandlerState state) const;
    void loadInputMethodSettings();

    void _q_syncHandlerMap(int);
    void _q_retranslateSettingsUi();

public:
    MIMPluginManager *parent;
    MInputContextConnection *mICConnection;

    Plugins plugins;
    ActivePlugins activePlugins;

    QStringList paths;
    QStringList blacklist;
    HandlerMap handlerToPlugin;

    QList<MGConfItem *> handlerToPluginConfs;
    MGConfItem *imAccessoryEnabledConf;

    QTimer deleteImTimer;
    QRegion activeImRegion;

    MIMSettingDialog *settingsDialog;
    QMap<MInputMethodSettingsBase *, MContainer *> settingsContainerMap;

    MIMPluginManager *q_ptr;
};

#endif
