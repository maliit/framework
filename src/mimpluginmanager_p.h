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
#include "mimonscreenplugins.h"
#include "mimsettings.h"
#include "mimhwkeyboardtracker.h"
#include "mindicatorserviceclient.h"

#ifdef HAVE_LEGACY_NAMES
#include "mimmeegoindicator.h"
#endif

#include <QtCore>
#include <tr1/memory>

class MInputMethodPlugin;
class MImAbstractPluginFactory;
class MInputContextConnection;
class MIMPluginManager;
class MAttributeExtensionManager;
class MImSettings;
class MAbstractInputMethod;
class MIMPluginManagerAdaptor;

using namespace std::tr1;

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
        QString pluginId; // the library filename is used as ID
    };

    typedef QMap<MInputMethodPlugin *, PluginDescription> Plugins;
    typedef QSet<MInputMethodPlugin *> ActivePlugins;
    typedef QMap<MInputMethod::HandlerState, MInputMethodPlugin *> HandlerMap;
    typedef QMap<QString, MImAbstractPluginFactory*> PluginsFactory;

    MIMPluginManagerPrivate(shared_ptr<MInputContextConnection> connection, WeakWidget proxyWidget, MIMPluginManager *p);
    virtual ~MIMPluginManagerPrivate();

    void activatePlugin(MInputMethodPlugin *plugin);
    void loadPlugins();
    bool loadPlugin(const QDir &dir, const QString &fileName);
    bool loadFactoryPlugin(const QDir &dir, const QString &fileName);
    void addHandlerMap(MInputMethod::HandlerState state, const QString &pluginName);
    void setActiveHandlers(const QSet<MInputMethod::HandlerState> &states);
    QSet<MInputMethod::HandlerState> activeHandlers() const;
    void deactivatePlugin(MInputMethodPlugin *plugin);

    void replacePlugin(MInputMethod::SwitchDirection direction, MInputMethodPlugin *source,
                       Plugins::iterator replacement, const QString &subViewId);
    bool switchPlugin(MInputMethod::SwitchDirection direction, MAbstractInputMethod *initiator);
    bool switchPlugin(const QString &name,
                      MAbstractInputMethod *initiator,
                      const QString &subViewId = QString());
    bool trySwitchPlugin(MInputMethod::SwitchDirection direction,
                         MInputMethodPlugin *source,
                         Plugins::iterator replacement,
                         const QString &subViewId = QString());
    void changeHandlerMap(MInputMethodPlugin *origin,
                          MInputMethodPlugin *replacement,
                          QSet<MInputMethod::HandlerState> states);

    QStringList loadedPluginsNames() const;
    QStringList loadedPluginsNames(MInputMethod::HandlerState state) const;
    QList<MImPluginDescription> pluginDescriptions(MInputMethod::HandlerState) const;
    Plugins::const_iterator findEnabledPlugin(Plugins::const_iterator current,
                                              MInputMethod::SwitchDirection direction,
                                              MInputMethod::HandlerState state) const;
    void filterEnabledSubViews(QMap<QString, QString> &subViews,
                               const QString &pluginId,
                               MInputMethod::HandlerState state) const;
    void append(QList<MImSubViewDescription> &list,
                const QMap<QString, QString> &map,
                const QString &pluginId) const;
    QList<MImSubViewDescription> surroundingSubViewDescriptions(MInputMethod::HandlerState state) const;
    QStringList activePluginsNames() const;
    QString activePluginsName(MInputMethod::HandlerState state) const;
    void loadHandlerMap();
    MInputMethodPlugin *activePlugin(MInputMethod::HandlerState state) const;
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

    /*!
     * \brief Called in response to changed active on screen subview key change
     */
    void _q_onScreenSubViewChanged();

    QMap<QString, QString> availableSubViews(const QString &plugin,
                                             MInputMethod::HandlerState state
                                              = MInputMethod::OnScreen) const;
    QList<MImOnScreenPlugins::SubView> availablePluginsAndSubViews(MInputMethod::HandlerState state
                                                                    = MInputMethod::OnScreen) const;
    QString activeSubView(MInputMethod::HandlerState state) const;
    void setActivePlugin(const QString &pluginName, MInputMethod::HandlerState state);

    QString inputSourceName(MInputMethod::HandlerState source) const;

    MIMPluginManager *parent;
    shared_ptr<MInputContextConnection> mICConnection;

    Plugins plugins;
    ActivePlugins activePlugins;
    QSet<MAbstractInputMethod *> targets;
    PluginsFactory factories;

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
    bool visible;

    typedef QMap<MInputMethod::HandlerState, QString> InputSourceToNameMap;
    InputSourceToNameMap inputSourceToNameMap;

    MAttributeExtensionId toolbarId;

#ifdef HAVE_LEGACY_NAMES
    MImMeegoIndicator indicatorService;
#else
    MIndicatorServiceClient indicatorService;
#endif

    QTimer ensureEmptyRegionWhenHiddenTimer;

    MImOnScreenPlugins onScreenPlugins;
    MImHwKeyboardTracker hwkbTracker;

    WeakWidget proxyWidget;
    int lastOrientation;

    QScopedPointer<MAttributeExtensionManager> attributeExtensionManager;
};

#endif
