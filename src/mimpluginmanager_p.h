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

#include "minputmethodnamespace.h"
#include "mtoolbarid.h"

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
    typedef QSet<MInputMethod::HandlerState> PluginState;
    struct PluginDescription {
        QString fileName;
        MInputMethodBase *inputMethod;
        PluginState state;
        MInputMethod::SwitchDirection lastSwitchDirection;
    };
    typedef QMap<MInputMethodPlugin *, PluginDescription> Plugins;
    typedef QSet<MInputMethodPlugin *> ActivePlugins;
    typedef QMap<MInputMethod::HandlerState, MInputMethodPlugin *> HandlerMap;

    MIMPluginManagerPrivate(MInputContextConnection *connection, MIMPluginManager *p);
    virtual ~MIMPluginManagerPrivate();

    void activatePlugin(MInputMethodPlugin *plugin);
    void loadPlugins();
    bool loadPlugin(const QString &fileName);
    void addHandlerMap(MInputMethod::HandlerState state, const QString &pluginName);
    void setActiveHandlers(const QSet<MInputMethod::HandlerState> &states);
    QSet<MInputMethod::HandlerState> activeHandlers() const;
    void deactivatePlugin(MInputMethodPlugin *plugin);
    void convertAndFilterHandlers(const QStringList &handlerNames,
                                  QSet<MInputMethod::HandlerState> *handlers);

    void replacePlugin(MInputMethod::SwitchDirection direction, Plugins::iterator initiator,
                       Plugins::iterator replacement);
    bool switchPlugin(MInputMethod::SwitchDirection direction, MInputMethodBase *initiator);
    bool switchPlugin(const QString &name, MInputMethodBase *initiator);
    bool doSwitchPlugin(MInputMethod::SwitchDirection direction,
                        Plugins::iterator source,
                        Plugins::iterator replacement);
    void changeHandlerMap(MInputMethodPlugin *origin,
                          MInputMethodPlugin *replacement,
                          QSet<MInputMethod::HandlerState> states);

    QStringList loadedPluginsNames() const;
    QStringList loadedPluginsNames(MInputMethod::HandlerState state) const;
    QStringList activePluginsNames() const;
    QString activePluginsName(MInputMethod::HandlerState state) const;
    void loadHandlerMap();
    MInputMethodPlugin *activePlugin(MInputMethod::HandlerState state) const;
    void loadInputMethodSettings();
    void initActiveSubView();
    void hideActivePlugins();
    void showActivePlugins();

    /*!
     * This method is called when one of the gconf about handler map is changed
     * to synchronize the handlerToPluginConfs.
     * \param state (can be cast to MInputMethod::HandlerState) indicates which state of the
     * handler map is changed.
     */
    void _q_syncHandlerMap(int state);

    /*!
     * \brief This method is called when activeSubview is changed by settings or plugin.
     */
    void _q_setActiveSubView(const QString &, MInputMethod::HandlerState);

    QMap<QString, QString> availableSubViews(const QString &plugin,
                                             MInputMethod::HandlerState state
                                              = MInputMethod::OnScreen) const;
    QString activeSubView(MInputMethod::HandlerState state) const;
    void setActivePlugin(const QString &pluginName, MInputMethod::HandlerState state);

    QString inputSourceName(MInputMethod::HandlerState source) const;
    MInputMethod::HandlerState inputSourceFromName(const QString &name, bool &valid) const;

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

    bool acceptRegionUpdates;

    typedef QMap<MInputMethod::HandlerState, QString> InputSourceToNameMap;
    QMap<MInputMethod::HandlerState, QString> inputSourceToNameMap;
    QMap<QString, MInputMethod::HandlerState> nameToInputSourceMap;

    MToolbarId toolbarId;
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
