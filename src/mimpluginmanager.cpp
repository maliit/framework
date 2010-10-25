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

#include "mimpluginmanager.h"
#include "mimpluginmanager_p.h"
#include "mplainwindow.h"

#include <MGConfItem>
#include <MKeyboardStateTracker>
#include <MSceneManager>
#include <MLocale>

#include "minputmethodplugin.h"
#include "mtoolbarmanager.h"
#include "mimsettingsdialog.h"

#include <QDir>
#include <QPluginLoader>
#include <QSignalMapper>
#include <QGraphicsLinearLayout>
#include <QStandardItemModel>

#include <QDBusAbstractAdaptor>
#include <QDBusInterface>
#include <QDBusMetaType>

#include <QDebug>

#ifdef QT_DBUS
#include "minputcontextdbusconnection.h"
typedef MInputContextDBusConnection MInputContextConnectionImpl;
#else
#include "minputcontextglibdbusconnection.h"
typedef MInputContextGlibDBusConnection MInputContextConnectionImpl;
#endif


namespace
{
    const int DeleteInputMethodTimeout = 60000;

    const QString DefaultPluginLocation("/usr/lib/meego-im-plugins/");

    const QString ConfigRoot          = "/meegotouch/inputmethods/";
    const QString MImPluginPaths    = ConfigRoot + "paths";
    const QString MImPluginDisabled = ConfigRoot + "disabledpluginfiles";

    const QString PluginRoot          = "/meegotouch/inputmethods/plugins";
    const QString MImAccesoryEnabled  = "/meegotouch/inputmethods/accessoryenabled";

    const char * const DBusServiceName = "com.maemo.inputmethodpluginmanager1";
    const char * const DBusPath = "/com/maemo/inputmethodpluginmanager1";
}

MIMPluginManagerPrivate::MIMPluginManagerPrivate(MInputContextConnection *connection,
                                                 MIMPluginManager *p)
    : parent(p),
      mICConnection(connection),
      imAccessoryEnabledConf(0),
      settingsDialog(0),
      adaptor(0),
      connectionValid(false),
      acceptRegionUpdates(true)
{
    inputSourceToNameMap[MInputMethod::OnScreen] = "onscreen";
    inputSourceToNameMap[MInputMethod::Hardware] = "hardware";
    inputSourceToNameMap[MInputMethod::Accessory] = "accessory";

    for (InputSourceToNameMap::const_iterator i(inputSourceToNameMap.begin());
         i != inputSourceToNameMap.end(); ++i) {
        nameToInputSourceMap[i.value()] = i.key();
    }
}


MIMPluginManagerPrivate::~MIMPluginManagerPrivate()
{
    qDeleteAll(handlerToPluginConfs);
    delete mICConnection;
    delete settingsDialog;
}


void MIMPluginManagerPrivate::loadPlugins()
{
    foreach (QString path, paths) {
        QDir pluginsDir(path);

        foreach (const QString &fileName, pluginsDir.entryList(QDir::Files)) {
            if (blacklist.contains(fileName)) {
                qWarning() << __PRETTY_FUNCTION__ << fileName << "is on the blacklist, skipped.";
                continue;
            }
            loadPlugin(pluginsDir.absoluteFilePath(fileName));
        } // end foreach file in path
    } // end foreach path in paths
}

bool MIMPluginManagerPrivate::loadPlugin(const QString &fileName)
{
    Q_Q(MIMPluginManager);

    bool val = false;
    QPluginLoader load(fileName);
    QObject *pluginInstance = load.instance();

    if (!pluginInstance) {
        qWarning() << __PRETTY_FUNCTION__ << "Error loading plugin from "
                   << fileName << load.errorString();
    } else {
        MInputMethodPlugin *plugin = qobject_cast<MInputMethodPlugin *>(pluginInstance);
        if (plugin) {
            if (!plugin->supportedStates().isEmpty()) {
                MInputMethodHost *inputMethodHost = new MInputMethodHost(mICConnection, q);
                MInputMethodBase *inputMethod = plugin->createInputMethod(inputMethodHost);

                // only add valid plugin descriptions
                if (inputMethod) {
                    PluginDescription desc = { load.fileName(), inputMethod, inputMethodHost,
                                               PluginState(), MInputMethod::SwitchUndefined };
                    plugins[plugin] = desc;
                    val = true;
                    inputMethodHost->setInputMethod(inputMethod);
                } else {
                    delete inputMethodHost;
                }
            } else {
                qWarning() << __PRETTY_FUNCTION__
                           << "Plugin does not support any state: " << fileName;
            }
        } else {
            qWarning() << __PRETTY_FUNCTION__
                       << "Plugin is not MInputMethodPlugin: " << fileName;
        }
    }
    return val;
}

void MIMPluginManagerPrivate::activatePlugin(MInputMethodPlugin *plugin)
{
    Q_Q(MIMPluginManager);
    if (!plugin || activePlugins.contains(plugin)) {
        return;
    }

    MInputMethodBase *inputMethod = 0;

    activePlugins.insert(plugin);
    inputMethod = plugins[plugin].inputMethod;
    plugins[plugin].imHost->setEnabled(true);

    Q_ASSERT(inputMethod);
    QObject::connect(inputMethod, SIGNAL(regionUpdated(const QRegion &)),
                     q, SLOT(updateRegion(const QRegion &)));

    QObject::connect(inputMethod,
                     SIGNAL(inputMethodAreaUpdated(const QRegion &)),
                     mICConnection,
                     SLOT(updateInputMethodArea(const QRegion &)));

    QObject::connect(inputMethod,
                     SIGNAL(activeSubViewChanged(QString, MInputMethod::HandlerState)),
                     q,
                     SLOT(_q_setActiveSubView(QString, MInputMethod::HandlerState)));

    mICConnection->addTarget(inputMethod); // redirect incoming requests

    return;
}


void MIMPluginManagerPrivate::addHandlerMap(MInputMethod::HandlerState state,
                                            const QString &pluginName)
{
    foreach (MInputMethodPlugin *plugin, plugins.keys()) {
        if (plugin->name() == pluginName) {
            handlerToPlugin[state] = plugin;
            break;
        }
    }
}


void MIMPluginManagerPrivate::setActiveHandlers(const QSet<MInputMethod::HandlerState> &states)
{
    QSet<MInputMethodPlugin *> activatedPlugins;
    MInputMethodBase *inputMethod = 0;

    //clear all cached states before activating new one
    for (Plugins::iterator iterator = plugins.begin();
         iterator != plugins.end();
         ++iterator) {
        iterator->state.clear();
    }

    //activate new plugins
    foreach (MInputMethod::HandlerState state, states) {
        HandlerMap::const_iterator iterator = handlerToPlugin.find(state);
        MInputMethodPlugin *plugin = 0;

        if (iterator != handlerToPlugin.end()) {
            plugin = iterator.value();
            if (!activePlugins.contains(plugin)) {
                activatePlugin(plugin);
            }
            inputMethod = plugins[plugin].inputMethod;
            if (inputMethod) {
                plugins[plugin].state << state;
                activatedPlugins.insert(plugin);
            }
        }
    }

    // notify plugins about new states
    foreach (MInputMethodPlugin *plugin, activatedPlugins) {
        plugins[plugin].inputMethod->setState(plugins[plugin].state);
    }

    // deactivate unnecessary plugins
    QSet<MInputMethodPlugin *> previousActive = activePlugins;
    foreach (MInputMethodPlugin *plugin, previousActive) {
        if (!activatedPlugins.contains(plugin)) {
            deactivatePlugin(plugin);  //activePlugins is modified here
        }
    }
}


QSet<MInputMethod::HandlerState> MIMPluginManagerPrivate::activeHandlers() const
{
    QSet<MInputMethod::HandlerState> handlers;
    foreach (MInputMethodPlugin *plugin, activePlugins) {
        handlers << handlerToPlugin.key(plugin);
    }
    return handlers;
}


void MIMPluginManagerPrivate::deactivatePlugin(MInputMethodPlugin *plugin)
{
    Q_Q(MIMPluginManager);
    if (!activePlugins.contains(plugin))
        return;

    activePlugins.remove(plugin);
    MInputMethodBase *inputMethod = plugins[plugin].inputMethod;
    plugins[plugin].imHost->setEnabled(false);

    if (!inputMethod)
        return;

    plugins[plugin].state = PluginState();
    inputMethod->hide();
    inputMethod->reset();
    QObject::disconnect(inputMethod, 0, q, 0),
    QObject::disconnect(inputMethod,
                        SIGNAL(inputMethodAreaUpdated(const QRegion &)),
                        mICConnection,
                        SLOT(updateInputMethodArea(const QRegion &)));
    mICConnection->removeTarget(inputMethod);
}


void MIMPluginManagerPrivate::convertAndFilterHandlers(const QStringList &handlerNames,
                                                       QSet<MInputMethod::HandlerState> *handlers)
{
    bool ok = false;
    bool disableOnscreenKbd = false;

    foreach (const QString &name, handlerNames) {
        int handlerNumber = (MInputMethod::HandlerState)name.toInt(&ok);
        if (ok && handlerNumber >= MInputMethod::OnScreen
            && handlerNumber <= MInputMethod::Accessory) {
            if (!disableOnscreenKbd) {
                disableOnscreenKbd = handlerNumber != MInputMethod::OnScreen;
            }
            handlers->insert((MInputMethod::HandlerState)handlerNumber);
        }
    }

    if (disableOnscreenKbd) {
        handlers->remove(MInputMethod::OnScreen);
    }
}


void MIMPluginManagerPrivate::replacePlugin(MInputMethod::SwitchDirection direction,
                                            Plugins::iterator initiator,
                                            Plugins::iterator replacement)
{
    PluginState state = initiator->state;
    MInputMethodBase *switchedTo = 0;

    deactivatePlugin(initiator.key());
    activatePlugin(replacement.key());
    switchedTo = replacement->inputMethod;;
    replacement->state = state;
    switchedTo->setState(state);
    if (replacement->lastSwitchDirection == direction) {
        switchedTo->switchContext(direction, false);
    }
    initiator->lastSwitchDirection = direction;
    QSharedPointer<const MToolbarData> toolbar =
        MToolbarManager::instance().toolbarData(toolbarId);
    switchedTo->setToolbar(toolbar);
    switchedTo->show();
    switchedTo->showLanguageNotification();
}


bool MIMPluginManagerPrivate::switchPlugin(MInputMethod::SwitchDirection direction,
                                           MInputMethodBase *initiator)
{
    if (direction == MInputMethod::SwitchUndefined) {
        return true; //do nothing for this direction
    }

    //Find plugin initiated this switch
    Plugins::iterator iterator(plugins.begin());

    for (; iterator != plugins.end(); ++iterator) {
        if (iterator->inputMethod == initiator) {
            break;
        }
    }

    if (iterator == plugins.end()) {
        return false;
    }

    Plugins::iterator source = iterator;

    //find next inactive plugin and activate it
    for (int n = 0; n < plugins.size() - 1; ++n) {
        if (direction == MInputMethod::SwitchForward) {
            ++iterator;
            if (iterator == plugins.end()) {
                iterator = plugins.begin();
            }
        } else { // Backward
            if (iterator == plugins.begin()) {
                iterator = plugins.end();
            }
            --iterator;
        }

        if (doSwitchPlugin(direction, source, iterator)) {
            return true;
        }
    }

    return false;
}

bool MIMPluginManagerPrivate::switchPlugin(const QString &name, MInputMethodBase *initiator)
{
    //Find plugin initiated this switch
    Plugins::iterator iterator(plugins.begin());

    for (; iterator != plugins.end(); ++iterator) {
        if (iterator->inputMethod == initiator) {
            break;
        }
    }

    if (iterator == plugins.end()) {
        return false;
    }

    Plugins::iterator source = iterator;

    // find plugin specified by name
    for (iterator = plugins.begin(); iterator != plugins.end(); ++iterator) {
        if (iterator.key()->name() == name) {
            break;
        }
    }

    if (iterator == plugins.end()) {
        return false;
    }

    if (source == iterator) {
        return true;
    }

    return doSwitchPlugin(MInputMethod::SwitchUndefined, source, iterator);
}

bool MIMPluginManagerPrivate::doSwitchPlugin(MInputMethod::SwitchDirection direction,
                                             Plugins::iterator source,
                                             Plugins::iterator replacement)
{
    if (!activePlugins.contains(replacement.key())) {
        const QSet<MInputMethod::HandlerState> intersect(replacement.key()->supportedStates()
                & source->state);
        // switch to other plugin if it could handle any state
        // handled by current plugin just now
        if (intersect == source->state) {
            changeHandlerMap(source.key(), replacement.key(), replacement.key()->supportedStates());
            replacePlugin(direction, source, replacement);
            return true;
        }
    }

    return false;
}

QString MIMPluginManagerPrivate::inputSourceName(MInputMethod::HandlerState source) const
{
    return inputSourceToNameMap.value(source);
}


MInputMethod::HandlerState
MIMPluginManagerPrivate::inputSourceFromName(const QString &name, bool &valid) const
{
    const QString lowercaseName(name.toLower());
    valid = nameToInputSourceMap.contains(lowercaseName);
    return nameToInputSourceMap.value(lowercaseName);
}

void MIMPluginManagerPrivate::changeHandlerMap(MInputMethodPlugin *origin,
                                               MInputMethodPlugin *replacement,
                                               QSet<MInputMethod::HandlerState> states)
{
    foreach (MInputMethod::HandlerState state, states) {
        HandlerMap::iterator iterator = handlerToPlugin.find(state);
        if (iterator != handlerToPlugin.end() && *iterator == origin) {
            *iterator = replacement; //for unit tests
            // Update gconfitem to record new plugin for handler map.
            // This should be done after real changing the handler map,
            // to prevent _q_syncHandlerMap also being called to change handler map.
            MGConfItem gconf(PluginRoot + "/" + inputSourceName(state));
            gconf.set(replacement->name());
        }
    }
}

QStringList MIMPluginManagerPrivate::loadedPluginsNames() const
{
    QStringList result;

    foreach (MInputMethodPlugin *plugin, plugins.keys()) {
        result.append(plugin->name());
    }

    return result;
}


QStringList MIMPluginManagerPrivate::loadedPluginsNames(MInputMethod::HandlerState state) const
{
    QStringList result;

    foreach (MInputMethodPlugin *plugin, plugins.keys()) {
        if (plugin->supportedStates().contains(state))
            result.append(plugin->name());
    }

    return result;
}


QStringList MIMPluginManagerPrivate::activePluginsNames() const
{
    QStringList result;

    foreach (MInputMethodPlugin *plugin, activePlugins) {
        result.append(plugin->name());
    }

    return result;
}


QString MIMPluginManagerPrivate::activePluginsName(MInputMethod::HandlerState state) const
{
    QString result;

    MInputMethodPlugin *plugin = activePlugin(state);
    if (plugin) {
        result = plugin->name();
    }

    return result;
}


void MIMPluginManagerPrivate::loadHandlerMap()
{
    Q_Q(MIMPluginManager);
    QSignalMapper *signalMapper = new QSignalMapper(q);
    // Queries all children under PluginRoot, each is a gconf entry that maps an
    // input source to a plugin that handles it
    foreach (const QString &handler, MGConfItem(PluginRoot).listEntries()) {
        const QStringList path = handler.split("/");
        bool validSource(false);
        const MInputMethod::HandlerState source(inputSourceFromName(path.last(), validSource));
        if (validSource) {
            MGConfItem *handlerItem = new MGConfItem(handler);
            handlerToPluginConfs.append(handlerItem);
            const QString pluginName = handlerItem->value().toString();
            addHandlerMap(source, pluginName);
            QObject::connect(handlerItem, SIGNAL(valueChanged()), signalMapper, SLOT(map()));
            signalMapper->setMapping(handlerItem, source);
        } else {
            qWarning() << "Invalid input source used in a gconf key:" << handler;
        }
    }
    QObject::connect(signalMapper, SIGNAL(mapped(int)), q, SLOT(_q_syncHandlerMap(int)));
}


void MIMPluginManagerPrivate::_q_syncHandlerMap(int state)
{
    const MInputMethod::HandlerState source = static_cast<MInputMethod::HandlerState>(state);

    MInputMethodPlugin *currentPlugin = activePlugin(source);
    MGConfItem gconf(PluginRoot + "/" + inputSourceName(source));
    const QString pluginName = gconf.value().toString();

    // already synchronized.
    if (currentPlugin && pluginName == currentPlugin->name()) {
       return;
    }

    MInputMethodPlugin *replacement = 0;
    foreach (MInputMethodPlugin *plugin, plugins.keys()) {
        if (plugin->name() == pluginName) {
            replacement = plugin;
            break;
        }
    }
    if (replacement) {
        // switch plugin if handler gconf is changed.
        MInputMethodBase *inputMethod = plugins[currentPlugin].inputMethod;
        addHandlerMap(static_cast<MInputMethod::HandlerState>(state), pluginName);
        if (!switchPlugin(pluginName, inputMethod)) {
            qWarning() << __PRETTY_FUNCTION__ << ", switching to plugin:"
                       << pluginName << " failed";
        }
    }

    // need update activeSubview if plugin is switched.
    if (state == MInputMethod::OnScreen) {
        initActiveSubView();
    }
}

MInputMethodPlugin *MIMPluginManagerPrivate::activePlugin(MInputMethod::HandlerState state) const
{
    MInputMethodPlugin *plugin = 0;
    HandlerMap::const_iterator iterator = handlerToPlugin.find(state);
    if (iterator != handlerToPlugin.constEnd()) {
        plugin = iterator.value();
    }
    return plugin;
}

void MIMPluginManagerPrivate::_q_setActiveSubView(const QString &subViewId,
                                                  MInputMethod::HandlerState state)
{
    // now we only support active subview for OnScreen state.
    if (state == MInputMethod::OnScreen && !subViewId.isEmpty()
        && activePlugin(MInputMethod::OnScreen)
        && (activeSubViewIdOnScreen != subViewId)) {

        // check whether this subView is supported by current active plugin.
        MInputMethodBase *inputMethod = plugins[activePlugin(MInputMethod::OnScreen)].inputMethod;
        Q_ASSERT(inputMethod);

        foreach (const MInputMethodBase::MInputMethodSubView &subView,
                 inputMethod->subViews(MInputMethod::OnScreen)) {
            if (subView.subViewId == subViewId) {
                activeSubViewIdOnScreen = subViewId;
                if (inputMethod->activeSubView(MInputMethod::OnScreen) != activeSubViewIdOnScreen) {
                    inputMethod->setActiveSubView(activeSubViewIdOnScreen, MInputMethod::OnScreen);
                }
                if (adaptor) {
                    emit adaptor->activeSubViewChanged(MInputMethod::OnScreen);
                }
                if (settingsDialog) {
                    settingsDialog->refreshUi();
                }
                break;
            }
        }
    }
    if (state != MInputMethod::OnScreen) {
        qWarning() << "Unsupported state:" << state << " for active subview";
    }
}


void MIMPluginManagerPrivate::loadInputMethodSettings()
{
    if (!settingsDialog) {
        MLocale locale;
        // add text-input-settings catalog for the settings translation.
        locale.installTrCatalog("text-input-settings");
        MLocale::setDefault(locale);
        settingsDialog = new MIMSettingsDialog(this, "", M::NoStandardButton);
    }
}

void MIMPluginManagerPrivate::initActiveSubView()
{
    // initialize activeSubViewIdOnScreen
    if (activePlugin(MInputMethod::OnScreen)) {
        MInputMethodBase *inputMethod = plugins[activePlugin(MInputMethod::OnScreen)].inputMethod;
        if (activeSubViewIdOnScreen != inputMethod->activeSubView(MInputMethod::OnScreen)) {
            // activeSubViewIdOnScreen is invalid, should be initialized.
            activeSubViewIdOnScreen = inputMethod->activeSubView(MInputMethod::OnScreen);
            if (adaptor) {
                emit adaptor->activeSubViewChanged(MInputMethod::OnScreen);
            }
            if (settingsDialog) {
                settingsDialog->refreshUi();
            }
        }
    }
}

void MIMPluginManagerPrivate::showActivePlugins()
{
    foreach (MInputMethodPlugin *plugin, activePlugins) {
        plugins[plugin].inputMethod->show();
    }
}

void MIMPluginManagerPrivate::hideActivePlugins()
{
    if (settingsDialog) {
        // disappear the settings dialog without animation before hiding active plugins.
        MPlainWindow::instance()->sceneManager()->disappearSceneWindowNow(settingsDialog);
    }

    foreach (MInputMethodPlugin *plugin, activePlugins) {
        plugins[plugin].inputMethod->hide();
    }
}

QMap<QString, QString>
MIMPluginManagerPrivate::availableSubViews(const QString &plugin,
                                           MInputMethod::HandlerState state) const
{
    QMap<QString, QString> subViews;
    Plugins::iterator iterator(plugins.begin());

    for (; iterator != plugins.end(); ++iterator) {
        if (iterator.key()->name() == plugin) {
            if (iterator->inputMethod) {
                foreach (const MInputMethodBase::MInputMethodSubView &subView,
                         iterator->inputMethod->subViews(state)) {
                    subViews.insert(subView.subViewId, subView.subViewTitle);
                }
            }
            break;
        }
    }
    return subViews;
}

QString MIMPluginManagerPrivate::activeSubView(MInputMethod::HandlerState state) const
{
    QString subView;
    MInputMethodPlugin *currentPlugin = activePlugin(state);
    if (currentPlugin) {
        subView = plugins[currentPlugin].inputMethod->activeSubView(state);
    }
    return subView;
}

void MIMPluginManagerPrivate::setActivePlugin(const QString &pluginName,
                                              MInputMethod::HandlerState state)
{
    MGConfItem currentPluginConf(PluginRoot + "/" + inputSourceName(state));
    if (!pluginName.isEmpty() && currentPluginConf.value().toString() != pluginName) {
        // check whether the pluginName is valid
        foreach (MInputMethodPlugin *plugin, plugins.keys()) {
            if (plugin->name() == pluginName) {
                currentPluginConf.set(pluginName);
                // Force call _q_syncHandlerMap() even though we already connect
                // _q_syncHandlerMap() with gconf valueChanged(). Because if the
                // request comes from different threads, the _q_syncHandlerMap()
                // won't be called at once. This means the synchronization of
                // handler map could be delayed if we don't force call it.
                _q_syncHandlerMap(state);
                break;
            }
        }
    }
}


MIMPluginManagerAdaptor::MIMPluginManagerAdaptor(MIMPluginManager *parent)
    : QDBusAbstractAdaptor(parent),
      owner(parent)
{
    if (!parent) {
        qFatal("Creating MIMPluginManagerAdaptor without a parent");
    }
}


MIMPluginManagerAdaptor::~MIMPluginManagerAdaptor()
{
    // nothing
}

QStringList MIMPluginManagerAdaptor::queryAvailablePlugins()
{
    Q_ASSERT(owner);
    return owner->loadedPluginsNames();
}

QStringList MIMPluginManagerAdaptor::queryAvailablePlugins(int state)
{
    Q_ASSERT(owner);
    return owner->loadedPluginsNames(static_cast<MInputMethod::HandlerState>(state));
}

QString MIMPluginManagerAdaptor::queryActivePlugin(int state)
{
    Q_ASSERT(owner);
    return owner->activePluginsName(static_cast<MInputMethod::HandlerState>(state));
}

QMap<QString, QVariant> MIMPluginManagerAdaptor::queryAvailableSubViews(const QString &plugin,
                                                                        int state)
{
    Q_ASSERT(owner);
    QMap<QString, QVariant> vSubViews;

    QMap<QString, QString> subViews
        = owner->availableSubViews(plugin, static_cast<MInputMethod::HandlerState>(state));
    QMapIterator<QString, QString> subView(subViews);
    while (subView.hasNext()) {
        subView.next();
        vSubViews.insert(subView.key(), subView.value());
    }
    return vSubViews;
}

QMap<QString, QVariant> MIMPluginManagerAdaptor::queryActiveSubView(int state)
{
    Q_ASSERT(owner);
    QMap<QString, QVariant> activeSubbView;
    activeSubbView.insert(owner->activeSubView(static_cast<MInputMethod::HandlerState>(state)),
                          owner->activePluginsName(static_cast<MInputMethod::HandlerState>(state)));
    return activeSubbView;
}

void MIMPluginManagerAdaptor::setActivePlugin(const QString &pluginName, int state,
                                              const QString &subViewId)
{
    Q_ASSERT(owner);
    owner->setActivePlugin(pluginName, static_cast<MInputMethod::HandlerState>(state));
    if (!subViewId.isEmpty()) {
        owner->setActiveSubView(subViewId, static_cast<MInputMethod::HandlerState>(state));
    }
}

void MIMPluginManagerAdaptor::setActiveSubView(const QString &subViewId, int state)
{
    Q_ASSERT(owner);
    owner->setActiveSubView(subViewId, static_cast<MInputMethod::HandlerState>(state));
}

///////////////
// actual class

MIMPluginManager::MIMPluginManager()
    : QObject(),
      d_ptr(new MIMPluginManagerPrivate(new MInputContextConnectionImpl, this))
{
    Q_D(MIMPluginManager);
    d->q_ptr = this;

    connect(d->mICConnection, SIGNAL(showInputMethodRequest()),
            this, SLOT(showActivePlugins()));

    connect(d->mICConnection, SIGNAL(hideInputMethodRequest()),
            this, SLOT(hideActivePlugins()));

    connect(d->mICConnection, SIGNAL(toolbarIdChanged(const MToolbarId &)),
            this, SLOT(setToolbar(const MToolbarId &)));

    MToolbarManager::createInstance();

    d->paths     = MGConfItem(MImPluginPaths).value(QStringList(DefaultPluginLocation)).toStringList();
    d->blacklist = MGConfItem(MImPluginDisabled).value().toStringList();

    d->loadPlugins();

    d->loadHandlerMap();

    if (MKeyboardStateTracker::instance()->isPresent()) {
        connect(MKeyboardStateTracker::instance(), SIGNAL(stateChanged()), this, SLOT(updateInputSource()));
    }

    d->imAccessoryEnabledConf = new MGConfItem(MImAccesoryEnabled, this);
    connect(d->imAccessoryEnabledConf, SIGNAL(valueChanged()), this, SLOT(updateInputSource()));

    updateInputSource();

    d->initActiveSubView();

    d->adaptor = new MIMPluginManagerAdaptor(this);
    bool success = QDBusConnection::sessionBus().registerObject(DBusPath, this);
    d->connectionValid = true;

    if (!success) {
        qDebug() << __PRETTY_FUNCTION__ << " failed to register D-Bus object";
        d->connectionValid = false;
    }

    if (!QDBusConnection::sessionBus().registerService(DBusServiceName)) {
        qDebug() << __PRETTY_FUNCTION__ << " failed to register D-Bus service";
        qDebug() << QDBusConnection::sessionBus().lastError().message();
        d->connectionValid = false;
    }

    qDBusRegisterMetaType<QStringList>();
    qDBusRegisterMetaType<QMap<QString, QVariant> >();
}


MIMPluginManager::~MIMPluginManager()
{
    Q_D(MIMPluginManager);
    MToolbarManager::destroyInstance();
    delete d;
}


QStringList MIMPluginManager::loadedPluginsNames() const
{
    Q_D(const MIMPluginManager);
    return d->loadedPluginsNames();
}


QStringList MIMPluginManager::loadedPluginsNames(MInputMethod::HandlerState state) const
{
    Q_D(const MIMPluginManager);
    return d->loadedPluginsNames(state);
}


QStringList MIMPluginManager::activePluginsNames() const
{
    Q_D(const MIMPluginManager);
    return d->activePluginsNames();
}


QString MIMPluginManager::activePluginsName(MInputMethod::HandlerState state) const
{
    Q_D(const MIMPluginManager);
    return d->activePluginsName(state);
}

void MIMPluginManager::updateInputSource()
{
    Q_D(MIMPluginManager);
    // Hardware and Accessory can work together.
    // OnScreen is mutually exclusive to Hardware and Accessory.
    QSet<MInputMethod::HandlerState> handlers = d->activeHandlers();
    if (MKeyboardStateTracker::instance()->isOpen()) {
        // hw keyboard is on
        handlers.remove(MInputMethod::OnScreen);
        handlers.insert(MInputMethod::Hardware);
    } else {
        // hw keyboard is off
        handlers.remove(MInputMethod::Hardware);
        handlers.insert(MInputMethod::OnScreen);
    }

    if (d->imAccessoryEnabledConf->value().toBool()) {
        handlers.remove(MInputMethod::OnScreen);
        handlers.insert(MInputMethod::Accessory);
    } else {
        handlers.remove(MInputMethod::Accessory);
    }

    if (!handlers.isEmpty()) {
        d->setActiveHandlers(handlers);
    }
}

void MIMPluginManager::switchPlugin(MInputMethod::SwitchDirection direction,
                                    MInputMethodBase *initiator)
{
    Q_D(MIMPluginManager);

    if (initiator) {
        if (!d->switchPlugin(direction, initiator)) {
            // no next plugin, just switch context
            initiator->switchContext(direction, true);
        }
    }
}

void MIMPluginManager::switchPlugin(const QString &name,
                                    MInputMethodBase *initiator)
{
    Q_D(MIMPluginManager);

    if (initiator) {
        if (!d->switchPlugin(name, initiator)) {
            qWarning() << __PRETTY_FUNCTION__ << ", switching to plugin:"
                       << name << " failed";
        }
    }
}

void MIMPluginManager::showInputMethodSettings()
{
    Q_D(MIMPluginManager);
    // require the whole screen area as the keyboard area for setting
    const QSize visibleSceneSize = MPlainWindow::instance()->visibleSceneSize(M::Landscape);
    emit regionUpdated(QRegion(0, 0, visibleSceneSize.width(), visibleSceneSize.height()));
    d->loadInputMethodSettings();
    MPlainWindow::instance()->sceneManager()->execDialog(d->settingsDialog);
    // restore the region
    emit regionUpdated(d->activeImRegion);
}


void MIMPluginManager::updateRegion(const QRegion &region)
{
    Q_D(MIMPluginManager);

    // Record input method object's region.
    d->activeImRegion = region;

    // If settings dialog is visible, don't update region. Don't update region
    // when no region updates from the plugin side are expected.
    if (d->acceptRegionUpdates &&
        (!d->settingsDialog || !d->settingsDialog->isVisible())) {
        emit regionUpdated(region);
    }
}

void MIMPluginManager::setToolbar(const MToolbarId &id)
{
    Q_D(MIMPluginManager);

    // Record MToolbarId for switch Plugin
    d->toolbarId = id;

    QSharedPointer<const MToolbarData> toolbar =
        MToolbarManager::instance().toolbarData(id);

    foreach (MInputMethodPlugin *plugin, d->activePlugins) {
        d->plugins[plugin].inputMethod->setToolbar(toolbar);
    }
}

void MIMPluginManager::showActivePlugins()
{
    Q_D(MIMPluginManager);

    d->acceptRegionUpdates = true;
    d->showActivePlugins();
}

void MIMPluginManager::hideActivePlugins()
{
    Q_D(MIMPluginManager);

    d->hideActivePlugins();

    // Do not accept region updates from hidden plugins. Emit an empty region
    // update, because we cannot assume that a plugin sends a region update
    // immediately after hiding it. It could play some animation, for example,
    // and only then send the region update (which would then be ignored).
    d->acceptRegionUpdates = false;
    emit regionUpdated(QRegion());
}

QMap<QString, QString> MIMPluginManager::availableSubViews(const QString &plugin,
                                                           MInputMethod::HandlerState state) const
{
    Q_D(const MIMPluginManager);
    return d->availableSubViews(plugin, state);
}

QString MIMPluginManager::activeSubView(MInputMethod::HandlerState state) const
{
    Q_D(const MIMPluginManager);
    return d->activeSubView(state);
}

void MIMPluginManager::setActivePlugin(const QString &pluginName, MInputMethod::HandlerState state)
{
    Q_D(MIMPluginManager);
    d->setActivePlugin(pluginName, state);
}

void MIMPluginManager::setActiveSubView(const QString &subViewId, MInputMethod::HandlerState state)
{
    Q_D(MIMPluginManager);
    d->_q_setActiveSubView(subViewId, state);
}

bool MIMPluginManager::isDBusConnectionValid() const
{
    Q_D(const MIMPluginManager);
    return d->connectionValid;
}

#include "moc_mimpluginmanager.cpp"
