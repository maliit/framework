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

#include "minputmethodnamespace.h"
#include "mattributeextensionid.h"
#include "minputmethodhost.h"
#include "mindicatorserviceclient.h"
#include "mimsettings.h"

#include <QObject>
#include <QWeakPointer>
#include <QWidget>
#include <QList>
#include <QStringList>
#include <QMap>
#include <QSet>
#include <QRegion>
#include <QTimer>

class MInputMethodPlugin;
class MInputContextConnection;
class MIMPluginManager;
class MImSettings;
class MAbstractInputMethod;
class MIMPluginManagerAdaptor;


/* Internal class only! Interfaces here change, internal developers only*/
class MIMPluginManagerPrivate
{
    Q_DECLARE_PUBLIC(MIMPluginManager)
public:
    typedef QSet<MInputMethod::HandlerState> PluginState;
    typedef QWeakPointer<QWidget> WeakWidget;

    enum ShowInputMethodRequest {
        DontShowInputMethod,
        ShowInputMethod
    };

    struct PluginDescription {
        MAbstractInputMethod *inputMethod;
        MInputMethodHost *imHost;
        PluginState state;
        MInputMethod::SwitchDirection lastSwitchDirection;
        WeakWidget centralWidget;
    };

    typedef QMap<MInputMethodPlugin *, PluginDescription> Plugins;
    typedef QSet<MInputMethodPlugin *> ActivePlugins;
    typedef QMap<MInputMethod::HandlerState, MInputMethodPlugin *> HandlerMap;

    MIMPluginManagerPrivate(MInputContextConnection *connection, MIMPluginManager *p);
    virtual ~MIMPluginManagerPrivate();

    void activatePlugin(MInputMethodPlugin *plugin);
    void loadPlugins();
    bool loadPlugin(MInputMethodPlugin *plugin);
    void addHandlerMap(MInputMethod::HandlerState state, const QString &pluginName);
    void setActiveHandlers(const QSet<MInputMethod::HandlerState> &states);
    QSet<MInputMethod::HandlerState> activeHandlers() const;
    void deactivatePlugin(MInputMethodPlugin *plugin);
    void convertAndFilterHandlers(const QStringList &handlerNames,
                                  QSet<MInputMethod::HandlerState> *handlers);

    void replacePlugin(MInputMethod::SwitchDirection direction, Plugins::iterator initiator,
                       Plugins::iterator replacement);
    bool switchPlugin(MInputMethod::SwitchDirection direction, MAbstractInputMethod *initiator);
    bool switchPlugin(const QString &name, MAbstractInputMethod *initiator);
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
    void initActiveSubView();
    void hideActivePlugins();
    void showActivePlugins();
    void ensureActivePluginsVisible(ShowInputMethodRequest request);

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

    //! Called a moment after hideActivePlugins is called to disable region
    //! updates and force an empty region in case of badly behaving plugins.
    void _q_ensureEmptyRegionWhenHidden();

    QMap<QString, QString> availableSubViews(const QString &plugin,
                                             MInputMethod::HandlerState state
                                              = MInputMethod::OnScreen) const;
    QString activeSubView(MInputMethod::HandlerState state) const;
    void setActivePlugin(const QString &pluginName, MInputMethod::HandlerState state);

    QString inputSourceName(MInputMethod::HandlerState source) const;
    MInputMethod::HandlerState inputSourceFromName(const QString &name, bool &valid) const;

    //! Returns the string value of the last active subview.
    QString lastActiveSubView() const;

    //! Sets last active subview.
    void setLastActiveSubView(const QString &subview);

    //! Configures a widget (and its widget hierarchy) for (self) compositing.
    static void configureWidgetsForCompositing(QWidget *mainWindow,
                                               bool selfCompositing);

    MIMPluginManager *parent;
    MInputContextConnection *mICConnection;

    Plugins plugins;
    ActivePlugins activePlugins;

    QStringList paths;
    QStringList blacklist;
    HandlerMap handlerToPlugin;

    QList<MImSettings *> handlerToPluginConfs;
    MImSettings *imAccessoryEnabledConf;
    QString activeSubViewIdOnScreen;
    QRegion activeImRegion;

    MIMPluginManagerAdaptor *adaptor;

    MIMPluginManager *q_ptr;
    bool connectionValid;

    bool acceptRegionUpdates;

    typedef QMap<MInputMethod::HandlerState, QString> InputSourceToNameMap;
    QMap<MInputMethod::HandlerState, QString> inputSourceToNameMap;
    QMap<QString, MInputMethod::HandlerState> nameToInputSourceMap;

    MAttributeExtensionId toolbarId;

    MIndicatorServiceClient indicatorService;

    MImSettings lastActiveSubViewConf;
    QTimer ensureEmptyRegionWhenHiddenTimer;
};

#endif
