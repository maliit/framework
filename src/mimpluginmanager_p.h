/* * This file is part of Maliit framework *
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 *
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 */

#ifndef MIMPLUGINMANAGER_P_H
#define MIMPLUGINMANAGER_P_H

#include <maliit/namespace.h>

#include "mattributeextensionid.h"
#include "minputmethodhost.h"
#include "mimonscreenplugins.h"
#include "mimsettings.h"
#include "mimhwkeyboardtracker.h"
#include <maliit/settingdata.h>
#include "windowgroup.h"
#include "abstractplatform.h"

#include <QtCore>

namespace Maliit {
namespace Plugins {
    class InputMethodPlugin;
}
}

class MInputContextConnection;
class MIMPluginManager;
class MAttributeExtensionManager;
class MSharedAttributeExtensionManager;
class MImSettings;
class MAbstractInputMethod;
class MIMPluginManagerAdaptor;

/* Internal class only! Interfaces here change, internal developers only*/
class MIMPluginManagerPrivate
{
    Q_DECLARE_PUBLIC(MIMPluginManager)
public:
    typedef QSet<Maliit::HandlerState> PluginState;

    enum ShowInputMethodRequest {
        DontShowInputMethod,
        ShowInputMethod
    };

    struct PluginDescription {
        MAbstractInputMethod *inputMethod;
        MInputMethodHost *imHost;
        PluginState state;
        Maliit::SwitchDirection lastSwitchDirection;
        QString pluginId; // the library filename is used as ID
        QSharedPointer<Maliit::WindowGroup> windowGroup;
    };

    typedef QMap<Maliit::Plugins::InputMethodPlugin *, PluginDescription> Plugins;
    typedef QSet<Maliit::Plugins::InputMethodPlugin *> ActivePlugins;
    typedef QMap<Maliit::HandlerState, Maliit::Plugins::InputMethodPlugin *> HandlerMap;

    MIMPluginManagerPrivate(const QSharedPointer<MInputContextConnection>& connection,
                            const QSharedPointer<Maliit::AbstractPlatform> &platform,
                            MIMPluginManager *p);
    virtual ~MIMPluginManagerPrivate();

    void autoDetectEnabledSubViews(const QString &plugin);

    void activatePlugin(Maliit::Plugins::InputMethodPlugin *plugin);
    void loadPlugins();
    bool loadPlugin(const QDir &dir, const QString &fileName);
    void addHandlerMap(Maliit::HandlerState state, const QString &pluginName);
    void setActiveHandlers(const QSet<Maliit::HandlerState> &states);
    QSet<Maliit::HandlerState> activeHandlers() const;
    void deactivatePlugin(Maliit::Plugins::InputMethodPlugin *plugin);

    void replacePlugin(Maliit::SwitchDirection direction, Maliit::Plugins::InputMethodPlugin *source,
                       Plugins::iterator replacement, const QString &subViewId);
    bool switchPlugin(Maliit::SwitchDirection direction, MAbstractInputMethod *initiator);
    bool switchPlugin(const QString &name,
                      MAbstractInputMethod *initiator,
                      const QString &subViewId = QString());
    bool trySwitchPlugin(Maliit::SwitchDirection direction,
                         Maliit::Plugins::InputMethodPlugin *source,
                         Plugins::iterator replacement,
                         const QString &subViewId = QString());
    void changeHandlerMap(Maliit::Plugins::InputMethodPlugin *origin,
                          Maliit::Plugins::InputMethodPlugin *replacement,
                          QSet<Maliit::HandlerState> states);

    QStringList loadedPluginsNames() const;
    QStringList loadedPluginsNames(Maliit::HandlerState state) const;
    QList<MImPluginDescription> pluginDescriptions(Maliit::HandlerState) const;
    Plugins::const_iterator findEnabledPlugin(Plugins::const_iterator current,
                                              Maliit::SwitchDirection direction,
                                              Maliit::HandlerState state) const;
    void filterEnabledSubViews(QMap<QString, QString> &subViews,
                               const QString &pluginId,
                               Maliit::HandlerState state) const;
    void append(QList<MImSubViewDescription> &list,
                const QMap<QString, QString> &map,
                const QString &pluginId) const;
    QList<MImSubViewDescription> surroundingSubViewDescriptions(Maliit::HandlerState state) const;
    QStringList activePluginsNames() const;
    QString activePluginsName(Maliit::HandlerState state) const;
    void loadHandlerMap();
    Maliit::Plugins::InputMethodPlugin *activePlugin(Maliit::HandlerState state) const;
    void hideActivePlugins();
    void showActivePlugins();
    void ensureActivePluginsVisible(ShowInputMethodRequest request);

    /*!
     * This method is called when one of the handler map settings have changed
     * to synchronize the handlerToPluginConfs.
     * \param state (can be cast to Maliit::HandlerState) indicates which state of the
     * handler map is changed.
     */
    void _q_syncHandlerMap(int state);

    /*!
     * \brief This method is called when activeSubview is changed by settings or plugin.
     */
    void _q_setActiveSubView(const QString &, Maliit::HandlerState);

    /*!
     * \brief Called in response to changed active on screen subview key change
     */
    void _q_onScreenSubViewChanged();

    QMap<QString, QString> availableSubViews(const QString &plugin,
                                             Maliit::HandlerState state
                                              = Maliit::OnScreen) const;
    QList<MImOnScreenPlugins::SubView> availablePluginsAndSubViews(Maliit::HandlerState state
                                                                    = Maliit::OnScreen) const;
    QString activeSubView(Maliit::HandlerState state) const;
    void setActivePlugin(const QString &pluginName, Maliit::HandlerState state);

    QString inputSourceName(Maliit::HandlerState source) const;

    MIMPluginManager *parent;
    QSharedPointer<MInputContextConnection> mICConnection;

    Plugins plugins;
    ActivePlugins activePlugins;
    QSet<MAbstractInputMethod *> targets;

    QStringList paths;
    QStringList blacklist;
    HandlerMap handlerToPlugin;

    QList<MImSettings *> handlerToPluginConfs;
    MImSettings *imAccessoryEnabledConf;
    QString activeSubViewIdOnScreen;

    MIMPluginManagerAdaptor *adaptor;

    MIMPluginManager *q_ptr;

    bool visible;

    typedef QMap<Maliit::HandlerState, QString> InputSourceToNameMap;
    InputSourceToNameMap inputSourceToNameMap;

    MAttributeExtensionId toolbarId;

    MImOnScreenPlugins onScreenPlugins;
    MImHwKeyboardTracker hwkbTracker;

    int lastOrientation;

    QScopedPointer<MAttributeExtensionManager> attributeExtensionManager;
    QScopedPointer<MSharedAttributeExtensionManager> sharedAttributeExtensionManager;

    QSharedPointer<Maliit::AbstractPlatform> m_platform;
};

#endif
