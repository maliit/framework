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
#include "mimabstractpluginfactory.h"
#include "mattributeextensionmanager.h"
#include "mabstractinputmethod.h"
#include "mimsettings.h"
#include "mimhwkeyboardtracker.h"
#include "mimapplication.h"
#include "mimpluginsproxywidget.h"
#include "mimremotewindow.h"
#include "mimrotationanimation.h"

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
    const QString DefaultPluginLocation(M_IM_PLUGINS_DIR);
    const QString DefaultFactoryPluginLocation(M_IM_FACTORY_PLUGINS_DIR);

    const QString ConfigRoot           = MALIIT_CONFIG_ROOT;
    const QString MImPluginPaths       = ConfigRoot + "paths";
    const QString MImPluginDisabled    = ConfigRoot + "disabledpluginfiles";
    const QString MImPluginFactories   = ConfigRoot + "factories";

    const QString PluginRoot           = MALIIT_CONFIG_ROOT"plugins";
    const QString MImAccesoryEnabled   = MALIIT_CONFIG_ROOT"accessoryenabled";

    const char * const DBusServiceName = "com.meego.inputmethodpluginmanager1";
    const char * const DBusPath        = "/com/meego/inputmethodpluginmanager1";

    const int MaxPluginHideTransitionTime(2*1000);

    // this function is used to detect the file suffix used to associate with specific factory
    static QString getFileMimeType(const QString& fileName)
    {
        QFileInfo fi(fileName);
        return fi.suffix();
    }
}

MIMPluginManagerPrivate::MIMPluginManagerPrivate(MInputContextConnection *connection,
                                                 MIMPluginManager *p)
    : parent(p),
      mICConnection(connection),
      imAccessoryEnabledConf(0),
      adaptor(0),
      q_ptr(0),
      connectionValid(false),
      acceptRegionUpdates(false),
      indicatorService(0),
      onScreenPlugins()
{
    inputSourceToNameMap[MInputMethod::Hardware] = "hardware";
    inputSourceToNameMap[MInputMethod::Accessory] = "accessory";

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
    Q_Q(MIMPluginManager);

    MImOnScreenPlugins::SubView activeSubView = onScreenPlugins.activeSubView();

    //load factories
    const QDir &dir(DefaultFactoryPluginLocation);
    foreach (QString factoryName, dir.entryList(QDir::Files))
        loadFactoryPlugin(dir, factoryName);

    foreach (QString path, paths) {
        const QDir &dir(path);

        if (loadPlugin(dir, activeSubView.plugin))
            break;
    }

    foreach (QString path, paths) {
        const QDir &dir(path);

        QStringList pluginFiles = dir.entryList(QDir::Files);
        foreach (const QString &fileName, pluginFiles) {
            if  (fileName == activeSubView.plugin)
                continue;

            loadPlugin(dir, fileName);
        } // end foreach file in path
    } // end foreach path in paths

    emit q->pluginsChanged();
}

bool MIMPluginManagerPrivate::loadFactoryPlugin(const QDir &dir, const QString &fileName)
{
    Q_ASSERT(mApp);

    if (blacklist.contains(fileName)) {
        qWarning() << __PRETTY_FUNCTION__ << fileName << "is on the blacklist, skipped.";
        return false;
    }

    // TODO: skip already loaded plugin ids (fileName)
    QPluginLoader load(dir.absoluteFilePath(fileName));

    QObject *pluginInstance = load.instance();
    if (!pluginInstance) {
        qWarning() << __PRETTY_FUNCTION__
                   << "Error loading factory plugin from" << dir.absoluteFilePath(fileName) << load.errorString();
        return false;
    }

    // check if the plugin is a factory
    MImAbstractPluginFactory *factory = qobject_cast<MImAbstractPluginFactory *>(pluginInstance);
    if (!factory) {
        qWarning() << __PRETTY_FUNCTION__
                   << "Could not cast" << pluginInstance->metaObject()->className() << "into MImAbstractPluginFactory.";
        return false;
    }
    factories.insert(factory->fileExtension(), factory);
    return true;
}

bool MIMPluginManagerPrivate::loadPlugin(const QDir &dir, const QString &fileName)
{
    Q_Q(MIMPluginManager);
    Q_ASSERT(mApp);

    if (blacklist.contains(fileName)) {
        qWarning() << __PRETTY_FUNCTION__ << fileName << "is on the blacklist, skipped.";
        return false;
    }

    MInputMethodPlugin *plugin = 0;

    // Check if we have a specific factory for this plugin
    QString mimeType = getFileMimeType(fileName);
    if (factories.contains(mimeType)) {
        plugin = factories[mimeType]->create(dir.filePath(fileName));
        if (!plugin) {
            qWarning() << __PRETTY_FUNCTION__
                       << "Could not create a plugin for: " << fileName;
        }
    } else {
        // TODO: skip already loaded plugin ids (fileName)
        QPluginLoader load(dir.absoluteFilePath(fileName));

        QObject *pluginInstance = load.instance();
        if (!pluginInstance) {
            qWarning() << __PRETTY_FUNCTION__
                       << "Error loading plugin from" << dir.absoluteFilePath(fileName) << load.errorString();
            return false;
        }

        plugin = qobject_cast<MInputMethodPlugin *>(pluginInstance);
        if (!plugin) {
            qWarning() << __PRETTY_FUNCTION__
                       << "Could not cast" << pluginInstance->metaObject()->className() << "into MInputMethodPlugin.";
            return false;
        }
    }

    if (plugin->supportedStates().isEmpty()) {
        qWarning() << __PRETTY_FUNCTION__
                   << "Plugin does not support any state." << plugin->name() << dir.absoluteFilePath(fileName);
        return false;
    }

    WeakWidget centralWidget(new QWidget(mApp->pluginsProxyWidget()));

    MInputMethodHost *host = new MInputMethodHost(mICConnection, q, indicatorService);
    MAbstractInputMethod *im = plugin->createInputMethod(host, centralWidget.data());

    QObject::connect(q, SIGNAL(pluginsChanged()), host, SIGNAL(pluginsChanged()));

    // only add valid plugin descriptions
    if (!im) {
        qWarning() << __PRETTY_FUNCTION__
                   << "Creation of InputMethod failed:" << plugin->name() << dir.absoluteFilePath(fileName);
        delete host;
        return false;
    }

    PluginDescription desc = { im, host, PluginState(),
                               MInputMethod::SwitchUndefined, centralWidget, fileName };
    plugins.insert(plugin, desc);
    host->setInputMethod(im);
    MIMApplication::configureWidgetsForCompositing();

    return true;
}

void MIMPluginManagerPrivate::activatePlugin(MInputMethodPlugin *plugin)
{
    Q_Q(MIMPluginManager);
    if (!plugin || activePlugins.contains(plugin)) {
        return;
    }

    MAbstractInputMethod *inputMethod = 0;

    activePlugins.insert(plugin);
    inputMethod = plugins.value(plugin).inputMethod;
    plugins.value(plugin).imHost->setEnabled(true);

    Q_ASSERT(inputMethod);

    QObject::connect(inputMethod,
                     SIGNAL(activeSubViewChanged(QString, MInputMethod::HandlerState)),
                     q,
                     SLOT(_q_setActiveSubView(QString, MInputMethod::HandlerState)));

    mICConnection->addTarget(inputMethod); // redirect incoming requests

    return;
}


void MIMPluginManagerPrivate::addHandlerMap(MInputMethod::HandlerState state,
                                            const QString &pluginId)
{
    foreach (MInputMethodPlugin *plugin, plugins.keys()) {
        if (plugins.value(plugin).pluginId == pluginId) {
            handlerToPlugin[state] = plugin;
            return;
        }
    }
    qWarning() << __PRETTY_FUNCTION__ << "Could not find plugin:" << pluginId;
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
            inputMethod = plugins.value(plugin).inputMethod;
            if (plugin && inputMethod) {
                plugins[plugin].state << state;
                activatedPlugins.insert(plugin);
            }
        }
    }

    // notify plugins about new states
    foreach (MInputMethodPlugin *plugin, activatedPlugins) {
        plugins.value(plugin).inputMethod->setState(plugins.value(plugin).state);
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
    if (!plugin)
        return;

    if (!activePlugins.contains(plugin))
        return;

    activePlugins.remove(plugin);
    MAbstractInputMethod *inputMethod = plugins.value(plugin).inputMethod;
    plugins.value(plugin).imHost->setEnabled(false);

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
                                            MInputMethodPlugin *source,
                                            Plugins::iterator replacement,
                                            const QString &subViewId)
{
    PluginState state;
    if (source)
        state = plugins.value(source).state;
    else
        state << MInputMethod::OnScreen;
    MAbstractInputMethod *switchedTo = 0;

    deactivatePlugin(source);
    activatePlugin(replacement.key());
    switchedTo = replacement->inputMethod;
    replacement->state = state;
    switchedTo->setState(state);
    if (state.contains(MInputMethod::OnScreen) && !subViewId.isNull()) {
        switchedTo->setActiveSubView(subViewId);
    } else if (replacement->lastSwitchDirection == direction
               || (replacement->lastSwitchDirection == MInputMethod::SwitchUndefined
                   && direction == MInputMethod::SwitchBackward)) {
        // we should enforce plugin to switch context if one of following conditions is true:
        // 1) if we have plugin A and B, and subviews A.0, A.1, A.2 and B.0, and B.0 is active,
        // and A.2 was active before B.0, then if we switch forward to plugin A, we want to start with subview A.0, not A.2
        // 2) if we have plugin A and B, and subviews A.0, A.1, A.2 and B.0, and B.0 is active,
        // and plugin A was not active since start of meego-im-uiserver,
        // then if we switch back to plugin A, we want to start with subview A.2, not A.0
        switchedTo->switchContext(direction, false);
    }
    if (source) {
        plugins[source].lastSwitchDirection = direction;
    }
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
        ensureActivePluginsVisible(DontShowInputMethod);
        switchedTo->show();
        switchedTo->showLanguageNotification();
    }

    if (state.contains(MInputMethod::OnScreen)) {
        if (activeSubViewIdOnScreen != switchedTo->activeSubView(MInputMethod::OnScreen)) {
            // activeSubViewIdOnScreen is invalid, should be initialized.
            activeSubViewIdOnScreen = switchedTo->activeSubView(MInputMethod::OnScreen);
            if (adaptor) {
                emit adaptor->activeSubViewChanged(MInputMethod::OnScreen);
            }
        }
        // Save the last active subview
        onScreenPlugins.setActiveSubView(MImOnScreenPlugins::SubView(replacement->pluginId, activeSubViewIdOnScreen));
    }
}


bool MIMPluginManagerPrivate::switchPlugin(MInputMethod::SwitchDirection direction,
                                           MAbstractInputMethod *initiator)
{
    if (direction == MInputMethod::SwitchUndefined) {
        return true; //do nothing for this direction
    }

    if (direction == MInputMethod::SwitchCanceled) {
        mApp->disableBackgroundMask(true);
        return true;
    }

    if (direction == MInputMethod::SwitchPreparationForward ||
        direction == MInputMethod::SwitchPreparationBackward) {
        mApp->enableBackgroundMask();
        return true;
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

    mApp->disableBackgroundMask(false);

    Plugins::iterator source = iterator;

    //find next inactive plugin and activate it
    for (int n = 0; n < plugins.size() - 1; ++n) {
        if (direction == MInputMethod::SwitchForward) {
            ++iterator;
            if (iterator == plugins.end()) {
                iterator = plugins.begin();
            }
        } else if (direction == MInputMethod::SwitchBackward) { // Backward
            if (iterator == plugins.begin()) {
                iterator = plugins.end();
            }
            --iterator;
        }

        if (trySwitchPlugin(direction, source.key(), iterator)) {
            return true;
        }
    }

    return false;
}

bool MIMPluginManagerPrivate::switchPlugin(const QString &pluginId,
                                           MAbstractInputMethod *initiator,
                                           const QString &subViewId)
{
    //Find plugin initiated this switch
    Plugins::iterator iterator(plugins.begin());

    for (; iterator != plugins.end(); ++iterator) {
        if (iterator->inputMethod == initiator) {
            break;
        }
    }

    Plugins::iterator source = iterator;

    // find plugin specified by name
    for (iterator = plugins.begin(); iterator != plugins.end(); ++iterator) {
        if (plugins.value(iterator.key()).pluginId == pluginId) {
            break;
        }
    }

    mApp->disableBackgroundMask(false);

    if (iterator == plugins.end()) {
        qWarning() << __PRETTY_FUNCTION__ << pluginId << "could not be found";
        return false;
    }

    if (source == iterator) {
        return true;
    }

    if (source == plugins.end()) {
        qDebug() << __PRETTY_FUNCTION__ << pluginId << "could not find initiator";
        return trySwitchPlugin(MInputMethod::SwitchUndefined, 0, iterator, subViewId);
    }

    return trySwitchPlugin(MInputMethod::SwitchUndefined, source.key(), iterator, subViewId);
}

bool MIMPluginManagerPrivate::trySwitchPlugin(MInputMethod::SwitchDirection direction,
                                              MInputMethodPlugin *source,
                                              Plugins::iterator replacement,
                                              const QString &subViewId)
{
    MInputMethodPlugin *newPlugin = replacement.key();

    if (activePlugins.contains(newPlugin)) {
        qDebug() << __PRETTY_FUNCTION__ << plugins.value(newPlugin).pluginId
                 << "is already active";
        return false;
    }

    if (!newPlugin) {
        qWarning() << __PRETTY_FUNCTION__ << "new plugin invalid";
        return false;
    }


    // switch to other plugin if it could handle any state
    // handled by current plugin just now
    PluginState currentState;
    if (source) {
        currentState = plugins.value(source).state;
    }

    const PluginState &supportedStates = newPlugin->supportedStates();
    if (!supportedStates.contains(currentState)) {
        qDebug() << __PRETTY_FUNCTION__ << plugins.value(newPlugin).pluginId
                 << "does not contain state";
        return false;
    }

    if (plugins.value(source).state.contains(MInputMethod::OnScreen)) {
        // if plugin which is to be switched needs to support OnScreen, the subviews should not be empty.
        if (!onScreenPlugins.isEnabled(plugins.value(newPlugin).pluginId)) {
            qDebug() << __PRETTY_FUNCTION__ << plugins.value(newPlugin).pluginId << "not enabled";
            return false;
        }
    }

    changeHandlerMap(source, newPlugin, newPlugin->supportedStates());
    replacePlugin(direction, source, replacement, subViewId);

    return true;
}

QString MIMPluginManagerPrivate::inputSourceName(MInputMethod::HandlerState source) const
{
    return inputSourceToNameMap.value(source);
}

void MIMPluginManagerPrivate::changeHandlerMap(MInputMethodPlugin *origin,
                                               MInputMethodPlugin *replacement,
                                               QSet<MInputMethod::HandlerState> states)
{
    foreach (MInputMethod::HandlerState state, states) {
        if (state == MInputMethod::OnScreen) {
            continue;
        }
        HandlerMap::iterator iterator = handlerToPlugin.find(state);
        if (iterator != handlerToPlugin.end() && *iterator == origin) {
            *iterator = replacement; //for unit tests
            // Update gconfitem to record new plugin for handler map.
            // This should be done after real changing the handler map,
            // to prevent _q_syncHandlerMap also being called to change handler map.
            MImSettings gconf(PluginRoot + "/" + inputSourceName(state));
            gconf.set(plugins.value(replacement).pluginId);
        }
    }
}

QStringList MIMPluginManagerPrivate::loadedPluginsNames() const
{
    QStringList result;

    foreach (const PluginDescription &desc, plugins.values()) {
        result.append(desc.pluginId);
    }

    return result;
}


QStringList MIMPluginManagerPrivate::loadedPluginsNames(MInputMethod::HandlerState state) const
{
    QStringList result;

    foreach (MInputMethodPlugin *plugin, plugins.keys()) {
        if (plugin->supportedStates().contains(state))
            result.append(plugins.value(plugin).pluginId);
    }

    return result;
}


QList<MImPluginDescription> MIMPluginManagerPrivate::pluginDescriptions(MInputMethod::HandlerState state) const
{
    QList<MImPluginDescription> result;

    const Plugins::const_iterator end = plugins.constEnd();
    for (Plugins::const_iterator iterator(plugins.constBegin());
         iterator != end;
         ++iterator) {
        const MInputMethodPlugin * const plugin = iterator.key();
        if (plugin && plugin->supportedStates().contains(state)) {
            result.append(MImPluginDescription(*plugin));

            if (state == MInputMethod::OnScreen) {
                result.last().setEnabled(onScreenPlugins.isEnabled(iterator->pluginId));
            }
        }
    }

    return result;
}

MIMPluginManagerPrivate::Plugins::const_iterator
MIMPluginManagerPrivate::findEnabledPlugin(Plugins::const_iterator current,
                                           MInputMethod::SwitchDirection direction,
                                           MInputMethod::HandlerState state) const
{
    MIMPluginManagerPrivate::Plugins::const_iterator iterator = current;
    MIMPluginManagerPrivate::Plugins::const_iterator result = plugins.constEnd();

    for (int n = 0; n < plugins.size() - 1; ++n) {
        if (direction == MInputMethod::SwitchForward) {
            ++iterator;
            if (iterator == plugins.end()) {
                iterator = plugins.begin();
            }
        } else if (direction == MInputMethod::SwitchBackward) { // Backward
            if (iterator == plugins.begin()) {
                iterator = plugins.end();
            }
            --iterator;
        }

        MInputMethodPlugin *otherPlugin = iterator.key();
        Q_ASSERT(otherPlugin);
        const MIMPluginManagerPrivate::PluginState &supportedStates = otherPlugin->supportedStates();
        if (!supportedStates.contains(state)) {
            continue;
        }

        if (state == MInputMethod::OnScreen
            && not onScreenPlugins.isEnabled(iterator->pluginId)) {
            continue;
        }

        result = iterator;
        break;
    }

    return result;
}

void MIMPluginManagerPrivate::filterEnabledSubViews(QMap<QString, QString> &subViews,
                                                    const QString &pluginId,
                                                    MInputMethod::HandlerState state) const
{
    QMap<QString, QString>::iterator iterator = subViews.begin();
    while(iterator != subViews.end()) {
        MImOnScreenPlugins::SubView subView(pluginId, iterator.key());
        if (state != MInputMethod::OnScreen || onScreenPlugins.isSubViewEnabled(subView)) {
            ++iterator;
        } else {
            iterator = subViews.erase(iterator);
        }
    }
}

void MIMPluginManagerPrivate::append(QList<MImSubViewDescription> &list,
                                     const QMap<QString, QString> &map,
                                     const QString &pluginId) const
{
    for(QMap<QString, QString>::const_iterator iterator = map.constBegin();
        iterator != map.constEnd();
        ++iterator) {
        MImSubViewDescription desc(pluginId, iterator.key(), iterator.value());

        list.append(desc);
    }
}

QList<MImSubViewDescription>
MIMPluginManagerPrivate::surroundingSubViewDescriptions(MInputMethod::HandlerState state) const
{
    QList<MImSubViewDescription> result;
    MInputMethodPlugin *plugin = activePlugin(state);

    if (!plugin) {
        return result;
    }

    Plugins::const_iterator iterator = plugins.find(plugin);
    Q_ASSERT(iterator != plugins.constEnd());

    QString pluginId = iterator->pluginId;
    QString subViewId = iterator->inputMethod->activeSubView(state);
    QMap<QString, QString> subViews = availableSubViews(pluginId, state);
    filterEnabledSubViews(subViews, pluginId, state);

    if (plugins.size() == 1 && subViews.size() == 1) {
        // there is one subview only
        return result;
    }

    QList<MImSubViewDescription> enabledSubViews;

    Plugins::const_iterator otherPlugin;
    otherPlugin = findEnabledPlugin(iterator,
                                    MInputMethod::SwitchBackward,
                                    state);

    if (otherPlugin != plugins.constEnd()) {
        QMap<QString, QString> prevSubViews = availableSubViews(otherPlugin->pluginId);
        filterEnabledSubViews(prevSubViews, otherPlugin->pluginId, state);
        append(enabledSubViews, prevSubViews, otherPlugin->pluginId);
    }

    append(enabledSubViews, subViews, pluginId);

    otherPlugin = findEnabledPlugin(iterator,
                                    MInputMethod::SwitchForward,
                                    state);

    if (otherPlugin != plugins.constEnd()) {
        QMap<QString, QString> prevSubViews = availableSubViews(otherPlugin->pluginId);
        filterEnabledSubViews(prevSubViews, otherPlugin->pluginId, state);
        append(enabledSubViews, prevSubViews, otherPlugin->pluginId);
    }

    if (enabledSubViews.size() == 1) {
        return result; //there is no other enabled subview
    }

    const QMap<QString, QString>::const_iterator subViewIterator = subViews.find(subViewId);
    if (subViewIterator == subViews.constEnd()) {
        return result;
    }

    MImSubViewDescription current(pluginId, subViewId, *subViewIterator);

    const int index = enabledSubViews.indexOf(current);
    Q_ASSERT(index >= 0);

    const int prevIndex = index > 0 ? index - 1 : enabledSubViews.size() - 1;
    result.append(enabledSubViews.at(prevIndex));

    const int nextIndex = index < (enabledSubViews.size() - 1) ? index + 1 : 0;
    result.append(enabledSubViews.at(nextIndex));

    return result;
}

QStringList MIMPluginManagerPrivate::activePluginsNames() const
{
    QStringList result;

    foreach (MInputMethodPlugin *plugin, activePlugins) {
        result.append(plugins.value(plugin).pluginId);
    }

    return result;
}


QString MIMPluginManagerPrivate::activePluginsName(MInputMethod::HandlerState state) const
{
    MInputMethodPlugin *plugin = activePlugin(state);
    if (!plugin)
        return QString();

    return plugins.value(plugin).pluginId;
}


void MIMPluginManagerPrivate::loadHandlerMap()
{
    Q_Q(MIMPluginManager);
    QSignalMapper *signalMapper = new QSignalMapper(q);

    // Queries all children under PluginRoot, each is a gconf entry that maps an
    // input source to a plugin that handles it
    const QStringList &handler(MImSettings(PluginRoot).listEntries());

    InputSourceToNameMap::const_iterator end = inputSourceToNameMap.constEnd();
    for (InputSourceToNameMap::const_iterator i(inputSourceToNameMap.constBegin()); i != end; ++i) {
        const QString &settingsKey(PluginRoot + "/" + i.value());

        if (!handler.contains(settingsKey))
            continue;

        MImSettings *handlerItem = new MImSettings(settingsKey);
        handlerToPluginConfs.append(handlerItem);
        const QString &pluginName = handlerItem->value().toString();
        addHandlerMap(i.key(), pluginName);
        QObject::connect(handlerItem, SIGNAL(valueChanged()), signalMapper, SLOT(map()));
        signalMapper->setMapping(handlerItem, i.key());
    }
    QObject::connect(signalMapper, SIGNAL(mapped(int)), q, SLOT(_q_syncHandlerMap(int)));
}


void MIMPluginManagerPrivate::_q_syncHandlerMap(int state)
{
    const MInputMethod::HandlerState source = static_cast<MInputMethod::HandlerState>(state);

    MInputMethodPlugin *currentPlugin = activePlugin(source);
    MImSettings gconf(PluginRoot + "/" + inputSourceName(source));
    const QString pluginId = gconf.value().toString();

    // already synchronized.
    if (currentPlugin && pluginId == plugins.value(currentPlugin).pluginId) {
       return;
    }

    MInputMethodPlugin *replacement = 0;
    foreach (MInputMethodPlugin *plugin, plugins.keys()) {
        if (plugins.value(plugin).pluginId == pluginId) {
            replacement = plugin;
            break;
        }
    }
    if (replacement) {
        // switch plugin if handler gconf is changed.
        MAbstractInputMethod *inputMethod = plugins.value(currentPlugin).inputMethod;
        addHandlerMap(static_cast<MInputMethod::HandlerState>(state), pluginId);
        if (!switchPlugin(pluginId, inputMethod)) {
            qWarning() << __PRETTY_FUNCTION__ << ", switching to plugin:"
                       << pluginId << " failed";
        }
    }
}

void MIMPluginManagerPrivate::_q_onScreenSubViewChanged()
{
    const MImOnScreenPlugins::SubView &subView = onScreenPlugins.activeSubView();

    if (!onScreenPlugins.isSubViewEnabled(subView)) {
        qWarning() << __PRETTY_FUNCTION__
                   <<  "Cannot switch to" << subView.plugin << subView.id << "because it is not enabled";
        return;
    }

    MInputMethodPlugin *currentPlugin = activePlugin(MInputMethod::OnScreen);

    if (currentPlugin && subView.plugin == plugins.value(currentPlugin).pluginId && activePlugins.contains(currentPlugin)) {
        qDebug() << __PRETTY_FUNCTION__ << "just switch subview";
        _q_setActiveSubView(subView.id, MInputMethod::OnScreen);
        return;
    }

    MInputMethodPlugin *replacement = 0;
    foreach (MInputMethodPlugin *plugin, plugins.keys()) {
        if (plugins.value(plugin).pluginId == subView.plugin) {
            replacement = plugin;
            break;
        }
    }

    if (replacement) {
        // switch plugin if handler gconf is changed.
        MAbstractInputMethod *inputMethod = 0;
        if (activePlugins.contains(currentPlugin))
            inputMethod = plugins.value(currentPlugin).inputMethod;
        addHandlerMap(MInputMethod::OnScreen, subView.plugin);
        if (!switchPlugin(subView.plugin, inputMethod, subView.id)) {
            qWarning() << __PRETTY_FUNCTION__ << ", switching to plugin:"
                       << subView.plugin << " failed";
        }
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
    if (state != MInputMethod::OnScreen) {
        qWarning() << "Unsupported state:" << state << " for active subview";
        return;
    }

    if (subViewId.isEmpty()) {
        return;
    }

    MInputMethodPlugin *plugin = activePlugin(MInputMethod::OnScreen);
    if (!plugin) {
        qDebug() << __PRETTY_FUNCTION__ << "No active plugin";
        return;
    }

    // Check whether active plugin is matching
    const QString &activePluginId = plugins.value(plugin).pluginId;
    if (activePluginId != onScreenPlugins.activeSubView().plugin) {
        // TODO?
        qWarning() << __PRETTY_FUNCTION__ << plugins.value(plugin).pluginId << "!=" << onScreenPlugins.activeSubView().plugin;
        return;
    }

    // Check whether subView is enabled
    if (!onScreenPlugins.isSubViewEnabled(MImOnScreenPlugins::SubView(activePluginId, subViewId))) {
        qWarning() << __PRETTY_FUNCTION__ << activePluginId << subViewId << "is not enabled";
        return;
    }

    // Check whether this subView is supported by current active plugin.
    MAbstractInputMethod *inputMethod = plugins.value(plugin).inputMethod;
    Q_ASSERT(inputMethod);
    if (!inputMethod) {
        qDebug() << __PRETTY_FUNCTION__ << "No input method";
        return;
    }

    foreach (const MAbstractInputMethod::MInputMethodSubView &subView,
             inputMethod->subViews(MInputMethod::OnScreen)) {
        if (subView.subViewId == subViewId) {
            activeSubViewIdOnScreen = subViewId;
            if (inputMethod->activeSubView(MInputMethod::OnScreen) != activeSubViewIdOnScreen) {
                inputMethod->setActiveSubView(activeSubViewIdOnScreen, MInputMethod::OnScreen);
            }
            // Save the last active subview
            if (onScreenPlugins.activeSubView().id != subViewId) {
                onScreenPlugins.setActiveSubView(MImOnScreenPlugins::SubView(activePluginId, subViewId));
            }
            if (adaptor) {
                emit adaptor->activeSubViewChanged(MInputMethod::OnScreen);
            }

            break;
        }
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

void MIMPluginManagerPrivate::showActivePlugins()
{
    ensureEmptyRegionWhenHiddenTimer.stop();
    acceptRegionUpdates = true;

    ensureActivePluginsVisible(ShowInputMethod);
}

void MIMPluginManagerPrivate::hideActivePlugins()
{
    foreach (MInputMethodPlugin *plugin, activePlugins) {
        plugins.value(plugin).inputMethod->hide();
    }

    ensureEmptyRegionWhenHiddenTimer.start();
}

void MIMPluginManagerPrivate::ensureActivePluginsVisible(ShowInputMethodRequest request)
{
    if (not mApp || not mApp->passThruWindow() || not mApp->pluginsProxyWidget()) {
        return;
    }

    foreach (QObject *obj, mApp->pluginsProxyWidget()->children()) {
        if (QWidget *w = qobject_cast<QWidget *>(obj)) {
            w->hide();
        }
    }

    foreach (MInputMethodPlugin *plugin, activePlugins) {
        const WeakWidget &w = plugins.value(plugin).centralWidget;
        if (w) {
            w.data()->show();
        }

        if (request == ShowInputMethod) {
            plugins.value(plugin).inputMethod->show();
        }
    }
}

QMap<QString, QString>
MIMPluginManagerPrivate::availableSubViews(const QString &plugin,
                                           MInputMethod::HandlerState state) const
{
    QMap<QString, QString> subViews;
    Plugins::iterator iterator(plugins.begin());

    for (; iterator != plugins.end(); ++iterator) {
        if (plugins.value(iterator.key()).pluginId == plugin) {
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
        subView = plugins.value(currentPlugin).inputMethod->activeSubView(state);
    }
    return subView;
}

void MIMPluginManagerPrivate::setActivePlugin(const QString &pluginId,
                                              MInputMethod::HandlerState state)
{
    if (state == MInputMethod::OnScreen) {
        const QList<MImOnScreenPlugins::SubView> &subViews = onScreenPlugins.enabledSubViews(pluginId);
        if (subViews.empty()) {
            qDebug() << __PRETTY_FUNCTION__ << pluginId << "has no enabled subviews";
            return;
        }

        const MImOnScreenPlugins::SubView &subView = subViews.first();
        onScreenPlugins.setActiveSubView(subView);

        // Even when the onScreen plugins where the same it does not mean the onScreen plugin
        // is the active one (that could be a plugin from another state) so make sure the current
        // onScreen plugin get loaded.
        _q_onScreenSubViewChanged();

        return;
    }
    MImSettings currentPluginConf(PluginRoot + "/" + inputSourceName(state));
    if (!pluginId.isEmpty() && currentPluginConf.value().toString() != pluginId) {
        // check whether the pluginName is valid
        foreach (MInputMethodPlugin *plugin, plugins.keys()) {
            if (plugins.value(plugin).pluginId == pluginId) {
                currentPluginConf.set(pluginId);
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

///////////////
// actual class

MIMPluginManager::MIMPluginManager(MImRotationAnimation* rotationAnimation)
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

    // connect orientation signals
    if (rotationAnimation) {
        connect(d->mICConnection, SIGNAL(appOrientationAboutToChange(int)),
                rotationAnimation, SLOT(appOrientationAboutToChange(int)));
        connect(d->mICConnection, SIGNAL(appOrientationChanged(int)),
                rotationAnimation, SLOT(appOrientationChangeFinished(int)));
    }

    d->paths        = MImSettings(MImPluginPaths).value(QStringList(DefaultPluginLocation)).toStringList();
    d->blacklist    = MImSettings(MImPluginDisabled).value().toStringList();

    d->loadPlugins();

    d->loadHandlerMap();

    connect(&d->onScreenPlugins, SIGNAL(activeSubViewChanged()),
            this, SLOT(_q_onScreenSubViewChanged()));
    d->_q_onScreenSubViewChanged();

    connect(&d->onScreenPlugins, SIGNAL(enabledPluginsChanged()),
            this, SIGNAL(pluginsChanged()));

    if (d->hwkbTracker.isPresent()) {
        connect(&d->hwkbTracker, SIGNAL(stateChanged()),
                this,            SLOT(updateInputSource()),
                Qt::UniqueConnection);
    }

    d->imAccessoryEnabledConf = new MImSettings(MImAccesoryEnabled, this);
    connect(d->imAccessoryEnabledConf, SIGNAL(valueChanged()), this, SLOT(updateInputSource()));

    updateInputSource();

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


QList<MImPluginDescription> MIMPluginManager::pluginDescriptions(MInputMethod::HandlerState state) const
{
    Q_D(const MIMPluginManager);
    return d->pluginDescriptions(state);
}

QList<MImSubViewDescription>
MIMPluginManager::surroundingSubViewDescriptions(MInputMethod::HandlerState state) const
{
    Q_D(const MIMPluginManager);
    return d->surroundingSubViewDescriptions(state);
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

    if (d->hwkbTracker.isOpen()) {
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
        d->plugins.value(plugin).inputMethod->setToolbar(toolbar);
        d->plugins.value(plugin).inputMethod->setKeyOverrides(keyOverrides);
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

void MIMPluginManager::hideActivePluginsIfWindowGone(MImRemoteWindow *window)
{
    if (not window) {
        hideActivePlugins();
    }
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
        d->plugins.value(plugin).inputMethod->setKeyOverrides(keyOverrides);
    }
}

#include "moc_mimpluginmanager.cpp"
