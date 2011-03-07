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
#include "mimpluginmanageradaptor.h"
#include "minputmethodplugin.h"
#include "mattributeextensionmanager.h"
#include "mabstractinputmethod.h"
#include "mimsettings.h"
#include "mimhwkeyboardtracker.h"
#include "mimapplication.h"
#include "mimremotewindow.h"

#include <QDir>
#include <QPluginLoader>
#include <QSignalMapper>
#include <QGraphicsLinearLayout>
#include <QStandardItemModel>

#include <QDBusAbstractAdaptor>
#include <QDBusInterface>
#include <QDBusMetaType>
#include <QWeakPointer>

#include <QDebug>
#include <deque>

#include "minputcontextglibdbusconnection.h"
typedef MInputContextGlibDBusConnection MInputContextConnectionImpl;


namespace
{
    const QString DefaultPluginLocation("/usr/lib/meego-im-plugins/");

    const QString ConfigRoot           = "/meegotouch/inputmethods/";
    const QString MImPluginPaths       = ConfigRoot + "paths";
    const QString MImPluginDisabled    = ConfigRoot + "disabledpluginfiles";

    const QString PluginRoot           = "/meegotouch/inputmethods/plugins";
    const QString MImAccesoryEnabled   = "/meegotouch/inputmethods/accessoryenabled";
    const QString LastActiveSubView    = "/meegotouch/inputmethods/virtualkeyboard/lastactivesubview";

    const char * const DBusServiceName = "com.meego.inputmethodpluginmanager1";
    const char * const DBusPath        = "/com/meego/inputmethodpluginmanager1";

    const int MaxPluginHideTransitionTime(2*1000);
}

MIMPluginManagerPrivate::MIMPluginManagerPrivate(MInputContextConnection *connection,
                                                 MIMPluginManager *p)
    : parent(p),
      mICConnection(connection),
      imAccessoryEnabledConf(0),
      adaptor(0),
      connectionValid(false),
      acceptRegionUpdates(false),
      indicatorService(0),
      lastActiveSubViewConf(LastActiveSubView)
{
    inputSourceToNameMap[MInputMethod::OnScreen] = "onscreen";
    inputSourceToNameMap[MInputMethod::Hardware] = "hardware";
    inputSourceToNameMap[MInputMethod::Accessory] = "accessory";

    for (InputSourceToNameMap::const_iterator i(inputSourceToNameMap.begin());
         i != inputSourceToNameMap.end(); ++i) {
        nameToInputSourceMap[i.value()] = i.key();
    }

    ensureEmptyRegionWhenHiddenTimer.setSingleShot(true);
    ensureEmptyRegionWhenHiddenTimer.setInterval(MaxPluginHideTransitionTime);
    QObject::connect(&ensureEmptyRegionWhenHiddenTimer, SIGNAL(timeout()),
                     parent, SLOT(_q_ensureEmptyRegionWhenHidden()));
}


MIMPluginManagerPrivate::~MIMPluginManagerPrivate()
{
    qDeleteAll(handlerToPluginConfs);
    delete mICConnection;
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
                MInputMethodHost *host = new MInputMethodHost(mICConnection, q, indicatorService);
                MAbstractInputMethod *im = plugin->createInputMethod(host, mApp->passThruWindow());

                // only add valid plugin descriptions
                if (im) {
                    PluginDescription desc = { load.fileName(), im, host, PluginState(),
                                               MInputMethod::SwitchUndefined };
                    plugins[plugin] = desc;
                    val = true;
                    host->setInputMethod(im);
                    MIMPluginManagerPrivate::configureWidgetsForCompositing(mApp->passThruWindow(),
                                                                            mApp->selfComposited());
                } else {
                    qWarning() << __PRETTY_FUNCTION__
                               << "Plugin loading failed:" << fileName;
                    delete host;
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

    MAbstractInputMethod *inputMethod = 0;

    activePlugins.insert(plugin);
    inputMethod = plugins[plugin].inputMethod;
    plugins[plugin].imHost->setEnabled(true);

    Q_ASSERT(inputMethod);

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
    MAbstractInputMethod *inputMethod = 0;

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
    MAbstractInputMethod *inputMethod = plugins[plugin].inputMethod;
    plugins[plugin].imHost->setEnabled(false);

    if (!inputMethod)
        return;

    plugins[plugin].state = PluginState();
    inputMethod->hide();
    inputMethod->reset();
    QObject::disconnect(inputMethod, 0, q, 0),
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
    MAbstractInputMethod *switchedTo = 0;

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
        MAttributeExtensionManager::instance().toolbarData(toolbarId);
    switchedTo->setToolbar(toolbar);

    QMap<QString, QSharedPointer<MKeyOverride> > keyOverrides =
        MAttributeExtensionManager::instance().keyOverrides(toolbarId);
    switchedTo->setKeyOverrides(keyOverrides);

    // TODO: show/hide from IC matches SIP show/hide requests but here show is used (and
    // hide in deactivatePlugin) in a sense completely unrelated to SIP requests.  Should
    // there be separte methods for plugin activation/deactivation?
    if (acceptRegionUpdates) {
        switchedTo->show();
        switchedTo->showLanguageNotification();
    }

    // When switching plugin, there is no activeSubViewChanged signal emitted,
    // but the active subview for OnScreen is really changed. So we update the recorded
    // active subview here.
    if (state.contains(MInputMethod::OnScreen)) {
        activeSubViewIdOnScreen = switchedTo->activeSubView(MInputMethod::OnScreen);
        setLastActiveSubView(switchedTo->activeSubView(MInputMethod::OnScreen));
        if (adaptor) {
            emit adaptor->activeSubViewChanged(MInputMethod::OnScreen);
        }
    }
}


bool MIMPluginManagerPrivate::switchPlugin(MInputMethod::SwitchDirection direction,
                                           MAbstractInputMethod *initiator)
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

bool MIMPluginManagerPrivate::switchPlugin(const QString &name, MAbstractInputMethod *initiator)
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
            // if plugin which is to be switched needs to support OnScreen, the subviews should not be empty.
            if (source->state.contains(MInputMethod::OnScreen)
                && replacement->inputMethod->subViews(MInputMethod::OnScreen).count() <= 0) {
                return false;
            }
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
            MImSettings gconf(PluginRoot + "/" + inputSourceName(state));
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
    foreach (const QString &handler, MImSettings(PluginRoot).listEntries()) {
        const QStringList path = handler.split("/");
        bool validSource(false);
        const MInputMethod::HandlerState source(inputSourceFromName(path.last(), validSource));
        if (validSource) {
            MImSettings *handlerItem = new MImSettings(handler);
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
    MImSettings gconf(PluginRoot + "/" + inputSourceName(source));
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
        MAbstractInputMethod *inputMethod = plugins[currentPlugin].inputMethod;
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
        MAbstractInputMethod *inputMethod
            = plugins[activePlugin(MInputMethod::OnScreen)].inputMethod;
        Q_ASSERT(inputMethod);

        foreach (const MAbstractInputMethod::MInputMethodSubView &subView,
                 inputMethod->subViews(MInputMethod::OnScreen)) {
            if (subView.subViewId == subViewId) {
                activeSubViewIdOnScreen = subViewId;
                if (inputMethod->activeSubView(MInputMethod::OnScreen) != activeSubViewIdOnScreen) {
                    inputMethod->setActiveSubView(activeSubViewIdOnScreen, MInputMethod::OnScreen);
                }
                // Save the last active subview
                setLastActiveSubView(subViewId);
                if (adaptor) {
                    emit adaptor->activeSubViewChanged(MInputMethod::OnScreen);
                }

                break;
            }
        }
    }
    if (state != MInputMethod::OnScreen) {
        qWarning() << "Unsupported state:" << state << " for active subview";
    }
}


void MIMPluginManagerPrivate::_q_ensureEmptyRegionWhenHidden()
{
    Q_Q(MIMPluginManager);
    // Do not accept region updates from hidden plugins. Emit an empty region
    // update, because we cannot trust that a plugin sends a region update
    // after it's hidden.
    acceptRegionUpdates = false;
    emit q->regionUpdated(QRegion());
}

void MIMPluginManagerPrivate::initActiveSubView()
{
    // initialize activeSubViewIdOnScreen
    if (activePlugin(MInputMethod::OnScreen)) {
        MAbstractInputMethod *inputMethod
            = plugins[activePlugin(MInputMethod::OnScreen)].inputMethod;
        if (activeSubViewIdOnScreen != inputMethod->activeSubView(MInputMethod::OnScreen)) {
            // activeSubViewIdOnScreen is invalid, should be initialized.
            activeSubViewIdOnScreen = inputMethod->activeSubView(MInputMethod::OnScreen);
            if (adaptor) {
                emit adaptor->activeSubViewChanged(MInputMethod::OnScreen);
            }
        }
    }
}

void MIMPluginManagerPrivate::showActivePlugins()
{
    ensureEmptyRegionWhenHiddenTimer.stop();
    acceptRegionUpdates = true;

    foreach (MInputMethodPlugin *plugin, activePlugins) {
        plugins[plugin].inputMethod->show();
    }
}

void MIMPluginManagerPrivate::hideActivePlugins()
{
    foreach (MInputMethodPlugin *plugin, activePlugins) {
        plugins[plugin].inputMethod->hide();
    }

    ensureEmptyRegionWhenHiddenTimer.start();
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
                foreach (const MAbstractInputMethod::MInputMethodSubView &subView,
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
    MImSettings currentPluginConf(PluginRoot + "/" + inputSourceName(state));
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

QString MIMPluginManagerPrivate::lastActiveSubView() const
{
    return lastActiveSubViewConf.value().toString();
}

void MIMPluginManagerPrivate::setLastActiveSubView(const QString &subview)
{
    lastActiveSubViewConf.set(subview);
}

void MIMPluginManagerPrivate::configureWidgetsForCompositing(QWidget *mainWindow,
                                                             bool selfCompositing)
{
    std::deque<QWidget *> unvisited;
    unvisited.push_back(mainWindow);

    // Breadth-first traversal of widget hierarchy, until no more
    // unvisited widgets remain. Will find viewports of QGraphicsViews,
    // as QAbstractScrollArea reparents the viewport to itself.
    while (not unvisited.empty()) {
        QWidget *current = unvisited.front();
        unvisited.pop_front();

        // Configure widget attributes:
        current->setAttribute(Qt::WA_OpaquePaintEvent);
        current->setAttribute(Qt::WA_NoSystemBackground);
        current->setAutoFillBackground(false);
        // Be aware that one cannot verify whether the background role *is*
        // QPalette::NoRole - see QTBUG-17924.
        current->setBackgroundRole(QPalette::NoRole);

        if (not selfCompositing) {
            // Careful: This flag can trigger a call to
            // qt_x11_recreateNativeWidgetsRecursive
            // - which will crash when it tries to get the effective WId
            // (as none of widgets have been mapped yet).
            current->setAttribute(Qt::WA_TranslucentBackground);
        }

        // Mark children of current widget as unvisited:
        foreach (QObject *obj, current->children()) {
            if (QWidget *w = qobject_cast<QWidget *>(obj)) {
                unvisited.push_back(w);
            }
        }
    }
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

    connect(d->mICConnection, SIGNAL(toolbarIdChanged(const MAttributeExtensionId &)),
            this, SLOT(setToolbar(const MAttributeExtensionId &)));

    connect(d->mICConnection, SIGNAL(keyOverrideCreated()),
            this, SLOT(updateKeyOverrides()));

    d->paths     = MImSettings(MImPluginPaths).value(QStringList(DefaultPluginLocation)).toStringList();
    d->blacklist = MImSettings(MImPluginDisabled).value().toStringList();

    d->loadPlugins();

    d->loadHandlerMap();

    if (MImHwKeyboardTracker::instance()->isPresent()) {
        connect(MImHwKeyboardTracker::instance(), SIGNAL(stateChanged()),
                this,                             SLOT(updateInputSource()),
                Qt::UniqueConnection);
    }

    d->imAccessoryEnabledConf = new MImSettings(MImAccesoryEnabled, this);
    connect(d->imAccessoryEnabledConf, SIGNAL(valueChanged()), this, SLOT(updateInputSource()));

    updateInputSource();

    // Set the last active subview
    if (!d->lastActiveSubView().isEmpty())
        d->_q_setActiveSubView(d->lastActiveSubView(), MInputMethod::OnScreen);

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
    MAttributeExtensionManager::destroyInstance();
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

    if (MImHwKeyboardTracker::instance()->isOpen()) {
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
                                    MAbstractInputMethod *initiator)
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
                                    MAbstractInputMethod *initiator)
{
    Q_D(MIMPluginManager);

    if (initiator) {
        if (!d->switchPlugin(name, initiator)) {
            qWarning() << __PRETTY_FUNCTION__ << ", switching to plugin:"
                       << name << " failed";
        }
    }
}

void MIMPluginManager::updateRegion(const QRegion &region)
{
    Q_D(MIMPluginManager);

    // Record input method object's region.
    d->activeImRegion = region;

    // Don't update region when no region updates from the plugin side are
    // expected.
    if (d->acceptRegionUpdates) {
        emit regionUpdated(region);
    }
}

void MIMPluginManager::setToolbar(const MAttributeExtensionId &id)
{
    Q_D(MIMPluginManager);

    // Record MAttributeExtensionId for switch Plugin
    d->toolbarId = id;

    QSharedPointer<const MToolbarData> toolbar =
        MAttributeExtensionManager::instance().toolbarData(id);

    QMap<QString, QSharedPointer<MKeyOverride> > keyOverrides =
        MAttributeExtensionManager::instance().keyOverrides(id);

    foreach (MInputMethodPlugin *plugin, d->activePlugins) {
        d->plugins[plugin].inputMethod->setToolbar(toolbar);
        d->plugins[plugin].inputMethod->setKeyOverrides(keyOverrides);
    }
}

void MIMPluginManager::showActivePlugins()
{
    Q_D(MIMPluginManager);

    d->showActivePlugins();
}

void MIMPluginManager::hideActivePlugins()
{
    Q_D(MIMPluginManager);

    d->hideActivePlugins();
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

void MIMPluginManager::updateKeyOverrides()
{
    Q_D(MIMPluginManager);
    QMap<QString, QSharedPointer<MKeyOverride> > keyOverrides =
        MAttributeExtensionManager::instance().keyOverrides(d->toolbarId);

    foreach (MInputMethodPlugin *plugin, d->activePlugins) {
        d->plugins[plugin].inputMethod->setKeyOverrides(keyOverrides);
    }
}

#include "moc_mimpluginmanager.cpp"
