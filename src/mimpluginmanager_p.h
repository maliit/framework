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
#include <QSet>
#include <QRegion>
#include <QDBusAbstractAdaptor>
#include <MDialog>

#include "mimhandlerstate.h"
#include "mimdirection.h"

class MInputMethodPlugin;
class MInputContextConnection;
class MIMPluginManager;
class MGConfItem;
class MInputMethodBase;
class MIMSettingsDialog;
class MIMPluginManagerAdaptor;

/* Internal class only! Interfaces here change, internal developers only*/
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
    QStringList loadedPluginsNames(MIMHandlerState state) const;
    QStringList activePluginsNames() const;
    QString activePluginsName(MIMHandlerState state) const;
    void loadHandlerMap();
    MInputMethodPlugin *activePlugin(MIMHandlerState state) const;
    void loadInputMethodSettings();
    void hideActivePlugins();
    void initActiveSubView();

    void _q_syncHandlerMap(int);
    void _q_setActiveSubView(const QString &, MIMHandlerState);

    QMap<QString, QString> availableSubViews(const QString &plugin, MIMHandlerState state = OnScreen) const;
    QString activeSubView(MIMHandlerState state) const;
    void setActivePlugin(const QString &pluginName, MIMHandlerState state);

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
    QString activeSubViewIdOnScreen;

    QRegion activeImRegion;

    MIMSettingsDialog *settingsDialog;
    MIMPluginManagerAdaptor *adaptor;

    MIMPluginManager *q_ptr;
    bool connectionValid;
};

class MIMPluginManagerAdaptor: public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.maemo.inputmethodpluginmanager1")
public:
    explicit MIMPluginManagerAdaptor(MIMPluginManager *parent);
    virtual ~MIMPluginManagerAdaptor();

public slots:
    QStringList queryAvailablePlugins();
    QStringList queryAvailablePlugins(int state);
    QString queryActivePlugin(int state);
    //! Returns all available subviews (IDs and titles).
    QMap<QString, QVariant> queryAvailableSubViews(const QString &pluginName, int state);
    //! Returns the active subview ID and the plugin which it belongs.
    QMap<QString, QVariant> queryActiveSubView(int state);
    Q_NOREPLY void setActivePlugin(const QString &pluginname, int state, const QString &subviewId = "");
    Q_NOREPLY void setActiveSubView(const QString &subViewId, int state);

signals:
    //! This signal is emitted when the active subview of \a state is changed.
    void activeSubViewChanged(int state);

private:
    MIMPluginManager *owner;
    friend class MIMPluginManager;
    friend class MIMPluginManagerPrivate;
};

#endif
