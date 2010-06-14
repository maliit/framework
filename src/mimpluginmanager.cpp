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
#include "minputcontextdbusconnection.h"
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


namespace
{
    const int DeleteInputMethodTimeout = 60000;

    const QString DefaultPluginLocation("/usr/lib/meego-im-plugins/");

    const QString ConfigRoot          = "/meegotouch/inputmethods/";
    const QString MImPluginPaths    = ConfigRoot + "paths";
    const QString MImPluginDisabled = ConfigRoot + "disabledpluginfiles";

    const QString PluginRoot          = "/meegotouch/inputmethods/plugins/";
    const QString MImHandlerToPlugin  = PluginRoot + "handler";
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
      adaptor(0)
{
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
                MInputMethodBase *inputMethod = plugin->createInputMethod(mICConnection);
                // only add valid plugin descriptions
                if (inputMethod) {
                    PluginDescription desc = { load.fileName(), inputMethod, PluginState(), M::SwitchUndefined };
                    plugins[plugin] = desc;
                    val = true;
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

bool MIMPluginManagerPrivate::activatePlugin(const QString &name)
{
    foreach (MInputMethodPlugin *plugin, activePlugins) {
        if (plugin->name() == name) {
            return true;
        }
    }

    foreach (MInputMethodPlugin *plugin, plugins.keys()) {
        if (plugin->name() == name) {
            activatePlugin(plugin);
            return true;
        }
    }
    return false;
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


    Q_ASSERT(inputMethod);
    QObject::connect(inputMethod, SIGNAL(regionUpdated(const QRegion &)),
                     q, SLOT(updateRegion(const QRegion &)));

    QObject::connect(inputMethod,
                     SIGNAL(inputMethodAreaUpdated(const QRegion &)),
                     mICConnection,
                     SLOT(updateInputMethodArea(const QRegion &)));

    QObject::connect(inputMethod,
                     SIGNAL(pluginSwitchRequired(M::InputMethodSwitchDirection)),
                     q,
                     SLOT(switchPlugin(M::InputMethodSwitchDirection)));

    QObject::connect(inputMethod,
                     SIGNAL(pluginSwitchRequired(const QString&)),
                     q,
                     SLOT(switchPlugin(const QString&)));

    QObject::connect(inputMethod,
                     SIGNAL(settingsRequested()),
                     q,
                     SLOT(showInputMethodSettings()));

    QObject::connect(inputMethod,
                     SIGNAL(activeSubViewChanged(QString, MIMHandlerState)),
                     q,
                     SLOT(_q_setActiveSubView(QString, MIMHandlerState)));

    mICConnection->addTarget(inputMethod); // redirect incoming requests

    return;
}


void MIMPluginManagerPrivate::addHandlerMap(MIMHandlerState state, const QString &pluginName)
{
    foreach (MInputMethodPlugin *plugin, plugins.keys()) {
        if (plugin->name() == pluginName) {
            handlerToPlugin[state] = plugin;
            break;
        }
    }
}


void MIMPluginManagerPrivate::setActiveHandlers(const QSet<MIMHandlerState> &states)
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
    foreach (MIMHandlerState state, states) {
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


QSet<MIMHandlerState> MIMPluginManagerPrivate::activeHandlers() const
{
    QSet<MIMHandlerState> handlers;
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
                                                       QSet<MIMHandlerState> *handlers)
{
    bool ok = false;
    bool disableOnscreenKbd = false;

    foreach (const QString &name, handlerNames) {
        int handlerNumber = (MIMHandlerState)name.toInt(&ok);
        if (ok && handlerNumber >= OnScreen && handlerNumber <= Accessory) {
            if (!disableOnscreenKbd) {
                disableOnscreenKbd = handlerNumber != OnScreen;
            }
            handlers->insert((MIMHandlerState)handlerNumber);
        }
    }

    if (disableOnscreenKbd) {
        handlers->remove(OnScreen);
    }
}


void MIMPluginManagerPrivate::replacePlugin(M::InputMethodSwitchDirection direction,
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
    switchedTo->show();
}


bool MIMPluginManagerPrivate::switchPlugin(M::InputMethodSwitchDirection direction,
                                           MInputMethodBase *initiator)
{
    if (direction == M::SwitchUndefined) {
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
        if (direction == M::SwitchForward) {
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

    return doSwitchPlugin(M::SwitchUndefined, source, iterator);
}

bool MIMPluginManagerPrivate::doSwitchPlugin(M::InputMethodSwitchDirection direction,
                                             Plugins::iterator source,
                                             Plugins::iterator replacement)
{
    if (!activePlugins.contains(replacement.key())) {
        const QSet<MIMHandlerState> intersect(replacement.key()->supportedStates()
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

void MIMPluginManagerPrivate::changeHandlerMap(MInputMethodPlugin *origin,
                                               MInputMethodPlugin *replacement,
                                               QSet<MIMHandlerState> states)
{
    foreach (MIMHandlerState state, states) {
        HandlerMap::iterator iterator = handlerToPlugin.find(state);
        if (iterator != handlerToPlugin.end() && *iterator == origin) {
            *iterator = replacement; //for unit tests
            // Update gconfitem to record new plugin for handler map.
            // This should be done after real changing the handler map,
            // to prevent _q_syncHandlerMap also being called to change handler map.
            MGConfItem gconf(QString(MImHandlerToPlugin + QString("/%1").arg(static_cast<int>(state))));
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


QStringList MIMPluginManagerPrivate::loadedPluginsNames(MIMHandlerState state) const
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


QString MIMPluginManagerPrivate::activePluginsName(MIMHandlerState state) const
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
    MGConfItem handlerToPluginConf(MImHandlerToPlugin);
    QList<QString> handlers = handlerToPluginConf.listEntries();
    // Queries all children under MImHandlerToPlugin,
    // each is a gconf for one state of the handler map.
    foreach (const QString &handler, handlers) {
        QStringList path = handler.split("/");
        MGConfItem *handlerItem = new MGConfItem(handler);
        handlerToPluginConfs.append(handlerItem);
        QString pluginName = handlerItem->value().toString();
        addHandlerMap((MIMHandlerState)path.last().toInt(), pluginName);
        QObject::connect(handlerItem, SIGNAL(valueChanged()), signalMapper, SLOT(map()));
        signalMapper->setMapping(handlerItem, (MIMHandlerState)path.last().toInt());
    }
    QObject::connect(signalMapper, SIGNAL(mapped(int)), q, SLOT(_q_syncHandlerMap(int)));
}


void MIMPluginManagerPrivate::_q_syncHandlerMap(int state)
{
     //  This method is called when one of the gconf about handler map is changed.
     //  \a state (can be cast to MIMHandlerState) indicates which state of the handler map is changed.
    HandlerMap::iterator iterator = handlerToPlugin.find(static_cast<MIMHandlerState>(state));
    MGConfItem gconf(QString(MImHandlerToPlugin + QString("/%1").arg(state)));
    QString pluginName = gconf.value().toString();
    MInputMethodPlugin *replacement = 0;
    foreach (MInputMethodPlugin *plugin, plugins.keys()) {
        if (plugin->name() == pluginName) {
            replacement = plugin;
            break;
        }
    }
    if (replacement
        && iterator != handlerToPlugin.end()
        && iterator.value()->name() != pluginName) {
        // switch plugin if handler gconf is changed.
        MInputMethodBase *inputMethod = plugins[iterator.value()].inputMethod;
        addHandlerMap(static_cast<MIMHandlerState>(state), pluginName);
        if (!switchPlugin(pluginName, inputMethod)) {
            qWarning() << __PRETTY_FUNCTION__ << ", switching to plugin:"
                       << pluginName << " failed";
        }
    }

    // need update activeSubview if plugin is switched.
    if (state == OnScreen) {
        initActiveSubView();
    }
}

MInputMethodPlugin *MIMPluginManagerPrivate::activePlugin(MIMHandlerState state) const
{
    MInputMethodPlugin *plugin = 0;
    HandlerMap::const_iterator iterator = handlerToPlugin.find(state);
    if (iterator != handlerToPlugin.constEnd()) {
        plugin = iterator.value();
    }
    return plugin;
}

void MIMPluginManagerPrivate::_q_setActiveSubView(const QString &subViewId, MIMHandlerState state)
{
    // now we only support active subview for OnScreen state.
    if (state == OnScreen && !subViewId.isEmpty() && activePlugin(OnScreen)
        && (activeSubViewIdOnScreen != subViewId)) {
        // check whether this subView is supported by current active plugin.
        MInputMethodBase *inputMethod = plugins[activePlugin(OnScreen)].inputMethod;
        Q_ASSERT(inputMethod);
        foreach (const MInputMethodBase::MInputMethodSubView &subView, inputMethod->subViews(OnScreen)) {
            if (subView.subViewId == subViewId) {
                activeSubViewIdOnScreen = subViewId;
                if (inputMethod->activeSubView(OnScreen) != activeSubViewIdOnScreen) {
                    inputMethod->setActiveSubView(activeSubViewIdOnScreen, OnScreen);
                }
                emit adaptor->activeSubViewChanged(OnScreen);
                if (settingsDialog) {
                    settingsDialog->refreshUi();
                }
                break;
            }
        }
    }
    if (state != OnScreen) {
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
    if (activePlugin(OnScreen)) {
        MInputMethodBase *inputMethod = plugins[activePlugin(OnScreen)].inputMethod;
        if (activeSubViewIdOnScreen != inputMethod->activeSubView(OnScreen)) {
            // activeSubViewIdOnScreen is invalid, should be initialized.
            activeSubViewIdOnScreen = inputMethod->activeSubView(OnScreen);
        }
    }
}

void MIMPluginManagerPrivate::hideActivePlugins()
{
    if (settingsDialog && settingsDialog->isVisible()) {
        // disappear the settings dialog without animation before hiding active plugins.
        MPlainWindow::instance()->sceneManager()->disappearSceneWindowNow(settingsDialog);
    }
    foreach (MInputMethodPlugin *plugin, activePlugins) {
        plugins[plugin].inputMethod->hide();
    }
}

QMap<QString, QString> MIMPluginManagerPrivate::availableSubViews(const QString &plugin, MIMHandlerState state) const
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

QString MIMPluginManagerPrivate::activeSubView(MIMHandlerState state) const
{
    QString subView;
    MInputMethodPlugin *currentPlugin = activePlugin(state);
    if (currentPlugin) {
        subView = plugins[currentPlugin].inputMethod->activeSubView(state);
    }
    return subView;
}

void MIMPluginManagerPrivate::setActivePlugin(const QString &pluginName, MIMHandlerState state)
{
    MGConfItem currentPluginConf(QString(MImHandlerToPlugin
                                 + QString("/%1").arg(static_cast<MIMHandlerState>(state))));
    if (!pluginName.isEmpty() && currentPluginConf.value().toString() != pluginName) {
        // check whether the pluginName is valid
        foreach (MInputMethodPlugin *plugin, plugins.keys()) {
            if (plugin->name() == pluginName) {
                currentPluginConf.set(pluginName);
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
    return owner->loadedPluginsNames(static_cast<MIMHandlerState>(state));
}

QString MIMPluginManagerAdaptor::queryActivePlugin(int state)
{
    Q_ASSERT(owner);
    return owner->activePluginsName(static_cast<MIMHandlerState>(state));
}

QMap<QString, QVariant> MIMPluginManagerAdaptor::queryAvailableSubViews(const QString &plugin, int state)
{
    Q_ASSERT(owner);
    QMap<QString, QVariant> vSubViews;

    QMap<QString, QString> subViews = owner->availableSubViews(plugin, static_cast<MIMHandlerState>(state));
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
    activeSubbView.insert(owner->activeSubView(static_cast<MIMHandlerState>(state)),
                          owner->activePluginsName(static_cast<MIMHandlerState>(state)));
    return activeSubbView;
}

void MIMPluginManagerAdaptor::setActivePlugin(const QString &pluginName, int state, const QString &subViewId)
{
    Q_ASSERT(owner);
    owner->setActivePlugin(pluginName, static_cast<MIMHandlerState>(state));
    if (!subViewId.isEmpty()) {
        owner->setActiveSubView(subViewId, static_cast<MIMHandlerState>(state));
    }
}

void MIMPluginManagerAdaptor::setActiveSubView(const QString &subViewId, int state)
{
    Q_ASSERT(owner);
    owner->setActiveSubView(subViewId, static_cast<MIMHandlerState>(state));
}

///////////////
// actual class

MIMPluginManager::MIMPluginManager()
    : QObject(),
      d_ptr(new MIMPluginManagerPrivate(new MInputContextDBusConnection, this))
{
    Q_D(MIMPluginManager);
    d->q_ptr = this;

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

    if (!success) {
        qDebug() << __PRETTY_FUNCTION__ << " failed to register D-Bus object";
    }

    if (!QDBusConnection::sessionBus().registerService(DBusServiceName)) {
        qDebug() << __PRETTY_FUNCTION__ << " failed to register D-Bus service";
        qDebug() << QDBusConnection::sessionBus().lastError().message();
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


QStringList MIMPluginManager::loadedPluginsNames(MIMHandlerState state) const
{
    Q_D(const MIMPluginManager);
    return d->loadedPluginsNames(state);
}


QStringList MIMPluginManager::activePluginsNames() const
{
    Q_D(const MIMPluginManager);
    return d->activePluginsNames();
}


QString MIMPluginManager::activePluginsName(MIMHandlerState state) const
{
    Q_D(const MIMPluginManager);
    return d->activePluginsName(state);
}

void MIMPluginManager::updateInputSource()
{
    Q_D(MIMPluginManager);
    // Hardware and Accessory can work together.
    // OnScreen is mutually exclusive to Hardware and Accessory.
    QSet<MIMHandlerState> handlers = d->activeHandlers();
    if (MKeyboardStateTracker::instance()->isOpen()) {
        // hw keyboard is on
        handlers.remove(OnScreen);
        handlers.insert(Hardware);
    } else {
        // hw keyboard is off
        handlers.remove(Hardware);
        handlers.insert(OnScreen);
    }

    if (d->imAccessoryEnabledConf->value().toBool()) {
        handlers.remove(OnScreen);
        handlers.insert(Accessory);
    } else {
        handlers.remove(Accessory);
    }

    if (!handlers.isEmpty()) {
        d->setActiveHandlers(handlers);
    }
}

void MIMPluginManager::switchPlugin(M::InputMethodSwitchDirection direction)
{
    Q_D(MIMPluginManager);
    MInputMethodBase *initiator = qobject_cast<MInputMethodBase*>(sender());

    if (initiator) {
        if (!d->switchPlugin(direction, initiator)) {
            // no next plugin, just switch context
            initiator->switchContext(direction, true);
        }
    }
}

void MIMPluginManager::switchPlugin(const QString &name)
{
    Q_D(MIMPluginManager);
    MInputMethodBase *initiator = qobject_cast<MInputMethodBase*>(sender());

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
    // record input method object's region.
    if (d->activeImRegion != region)
        d->activeImRegion = region;
    if (!d->settingsDialog || !d->settingsDialog->isVisible()) {
        //if settings dialog is visible, don't update region.
        emit regionUpdated(region);
    }
}

void MIMPluginManager::hideActivePlugins()
{
    Q_D(MIMPluginManager);
    d->hideActivePlugins();
}

QMap<QString, QString> MIMPluginManager::availableSubViews(const QString &plugin, MIMHandlerState state) const
{
    Q_D(const MIMPluginManager);
    return d->availableSubViews(plugin, state);
}

QString MIMPluginManager::activeSubView(MIMHandlerState state) const
{
    Q_D(const MIMPluginManager);
    return d->activeSubView(state);
}

void MIMPluginManager::setActivePlugin(const QString &pluginName, MIMHandlerState state)
{
    Q_D(MIMPluginManager);
    d->setActivePlugin(pluginName, state);
}

void MIMPluginManager::setActiveSubView(const QString &subViewId, MIMHandlerState state)
{
    Q_D(MIMPluginManager);
    d->_q_setActiveSubView(subViewId, state);
}

#include "moc_mimpluginmanager.cpp"
