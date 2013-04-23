/* * This file is part of Maliit framework *
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 *
 * Contact: maliit-discuss@lists.maliit.org
 *
 * Copyright (C) 2012 Canonical Ltd
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 */

#include "mimpluginmanager.h"
#include "mimpluginmanager_p.h"
#include <maliit/plugins/inputmethodplugin.h>
#include "mattributeextensionmanager.h"
#include "msharedattributeextensionmanager.h"
#include <maliit/plugins/abstractinputmethod.h>
#include "mimsettings.h"
#include "mimhwkeyboardtracker.h"
#include <maliit/plugins/updateevent.h>
#include "mimsubviewoverride.h"
#include "maliit/namespaceinternal.h"
#include <maliit/settingdata.h>
#include "windowgroup.h"

#include <quick/inputmethodquickplugin.h>

#include <QDir>
#include <QPluginLoader>
#include <QSignalMapper>
#include <QWeakPointer>

#include <QDebug>
#include <deque>

namespace
{
    const QString DefaultPluginLocation(MALIIT_PLUGINS_DIR);

    const char * const VisualizationAttribute = "visualizationPriority";
    const char * const FocusStateAttribute = "focusState";

    const QString ConfigRoot           = MALIIT_CONFIG_ROOT;
    const QString MImPluginPaths       = ConfigRoot + "paths";
    const QString MImPluginDisabled    = ConfigRoot + "disabledpluginfiles";

    const QString PluginRoot           = MALIIT_CONFIG_ROOT"plugins";
    const QString PluginSettings       = MALIIT_CONFIG_ROOT"pluginsettings";
    const QString MImAccesoryEnabled   = MALIIT_CONFIG_ROOT"accessoryenabled";

    const char * const InputMethodItem = "inputMethod";
    const char * const LoadAll = "loadAll";
}

MIMPluginManagerPrivate::MIMPluginManagerPrivate(const QSharedPointer<MInputContextConnection> &connection,
                                                 const QSharedPointer<Maliit::AbstractPlatform> &platform,
                                                 MIMPluginManager *p)
    : parent(p),
      mICConnection(connection),
      imAccessoryEnabledConf(0),
      q_ptr(0),
      visible(false),
      onScreenPlugins(),
      lastOrientation(0),
      attributeExtensionManager(new MAttributeExtensionManager),
      sharedAttributeExtensionManager(new MSharedAttributeExtensionManager),
      m_platform(platform)
{
    inputSourceToNameMap[Maliit::Hardware] = "hardware";
    inputSourceToNameMap[Maliit::Accessory] = "accessory";
}


MIMPluginManagerPrivate::~MIMPluginManagerPrivate()
{
    qDeleteAll(handlerToPluginConfs);
}

void MIMPluginManagerPrivate::loadPlugins()
{
    Q_Q(MIMPluginManager);

    MImOnScreenPlugins::SubView activeSubView = onScreenPlugins.activeSubView();

    // Load active plugin first
    Q_FOREACH (QString path, paths) {
        const QDir &dir(path);

        if (loadPlugin(dir, activeSubView.plugin))
            break;
    }

    // Load all other plugins
    Q_FOREACH (QString path, paths) {
        const QDir &dir(path);

        QStringList pluginFiles = dir.entryList(QDir::Files);
        Q_FOREACH (const QString &fileName, pluginFiles) {
            if  (fileName == activeSubView.plugin)
                continue;

            loadPlugin(dir, fileName);
        } // end Q_FOREACH file in path
    } // end Q_FOREACH path in paths

    if (plugins.empty()) {
        qWarning("No plugins were found. Stopping.");
        std::exit(0);
    }

    const QList<MImOnScreenPlugins::SubView> &availableSubViews = availablePluginsAndSubViews();
    onScreenPlugins.updateAvailableSubViews(availableSubViews);

    Q_EMIT q->pluginsChanged();
}

bool MIMPluginManagerPrivate::loadPlugin(const QDir &dir, const QString &fileName)
{
    Q_Q(MIMPluginManager);

    if (blacklist.contains(fileName)) {
        qWarning() << __PRETTY_FUNCTION__ << fileName << "is on the blacklist, skipped.";
        return false;
    }

    Maliit::Plugins::InputMethodPlugin *plugin = 0;

    if (QFileInfo(fileName).suffix() == "qml") {
        plugin = new Maliit::InputMethodQuickPlugin(dir.filePath(fileName), m_platform);
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

        plugin = qobject_cast<Maliit::Plugins::InputMethodPlugin *>(pluginInstance);
        if (!plugin) {
            qWarning() << __PRETTY_FUNCTION__
                       << pluginInstance->metaObject()->className() << "is not a Maliit::Server::InputMethodPlugin.";
            return false;
        }
    }

    if (plugin->supportedStates().isEmpty()) {
        qWarning() << __PRETTY_FUNCTION__
                   << "Plugin does not support any state." << plugin->name() << dir.absoluteFilePath(fileName);
        return false;
    }

    QSharedPointer<Maliit::WindowGroup> windowGroup(new Maliit::WindowGroup(m_platform));
    MInputMethodHost *host = new MInputMethodHost(mICConnection, q, windowGroup,
                                                  fileName, plugin->name());

    MAbstractInputMethod *im = plugin->createInputMethod(host);

    QObject::connect(q, SIGNAL(pluginsChanged()), host, SIGNAL(pluginsChanged()));

    // only add valid plugin descriptions
    if (!im) {
        qWarning() << __PRETTY_FUNCTION__
                   << "Creation of InputMethod failed:" << plugin->name() << dir.absoluteFilePath(fileName);
        delete host;
        return false;
    }

    PluginDescription desc = { im, host, PluginState(),
                               Maliit::SwitchUndefined, fileName, windowGroup };

    // Connect surface group signals
    QObject::connect(windowGroup.data(), SIGNAL(inputMethodAreaChanged(QRegion)),
                     mICConnection.data(), SLOT(updateInputMethodArea(QRegion)));

    plugins.insert(plugin, desc);
    host->setInputMethod(im);

    Q_EMIT q->pluginLoaded();

    return true;
}

void MIMPluginManagerPrivate::activatePlugin(Maliit::Plugins::InputMethodPlugin *plugin)
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
                     SIGNAL(activeSubViewChanged(QString, Maliit::HandlerState)),
                     q,
                     SLOT(_q_setActiveSubView(QString, Maliit::HandlerState)));


    inputMethod->handleAppOrientationChanged(lastOrientation);
    targets.insert(inputMethod);
}


void MIMPluginManagerPrivate::addHandlerMap(Maliit::HandlerState state,
                                            const QString &pluginId)
{
    Q_FOREACH (Maliit::Plugins::InputMethodPlugin *plugin, plugins.keys()) {
        if (plugins.value(plugin).pluginId == pluginId) {
            handlerToPlugin[state] = plugin;
            return;
        }
    }
    qWarning() << __PRETTY_FUNCTION__ << "Could not find plugin:" << pluginId;
}


MImPluginSettingsInfo MIMPluginManagerPrivate::globalSettings() const
{
    QStringList domain, descriptions;

    Q_FOREACH (Maliit::Plugins::InputMethodPlugin *plugin, plugins.keys()) {
        const MIMPluginManagerPrivate::PluginDescription &descr = plugins[plugin];
        QList<MAbstractInputMethod::MInputMethodSubView> subviews = descr.inputMethod->subViews();

        Q_FOREACH (const MAbstractInputMethod::MInputMethodSubView &subview, subviews) {
            domain.append(descr.pluginId + ":" + subview.subViewId);
            descriptions.append(plugin->name() + " - " + subview.subViewTitle);
        }
    }

    MImPluginSettingsEntry active_subviews;

    active_subviews.extension_key = MALIIT_CONFIG_ROOT"onscreen/active";
    active_subviews.description = QT_TRANSLATE_NOOP("maliit", "Active subview");
    active_subviews.type = Maliit::StringType;
    active_subviews.attributes[Maliit::SettingEntryAttributes::valueDomain] = domain;
    active_subviews.attributes[Maliit::SettingEntryAttributes::valueDomainDescriptions] = descriptions;

    MImPluginSettingsEntry enabled_subviews;

    enabled_subviews.extension_key = MALIIT_CONFIG_ROOT"onscreen/enabled";
    enabled_subviews.description = QT_TRANSLATE_NOOP("maliit", "Enabled subviews");
    enabled_subviews.type = Maliit::StringListType;
    enabled_subviews.attributes[Maliit::SettingEntryAttributes::valueDomain] = domain;
    enabled_subviews.attributes[Maliit::SettingEntryAttributes::valueDomainDescriptions] = descriptions;

    MImPluginSettingsInfo global;

    global.plugin_description = QT_TRANSLATE_NOOP("maliit", "Global");
    global.plugin_name = "server";
    global.extension_id = MSharedAttributeExtensionManager::PluginSettings;
    global.entries.append(active_subviews);
    global.entries.append(enabled_subviews);

    return global;
}


void MIMPluginManagerPrivate::registerSettings()
{
    // not exposed to final user, merely used to listen for settings list changes
    MImPluginSettingsInfo settings_list;

    settings_list.plugin_name = "@settings";
    settings_list.extension_id = MSharedAttributeExtensionManager::PluginSettingsList;

    registerSettings(settings_list);

    // global settings
    registerSettings(globalSettings());
}


void MIMPluginManagerPrivate::registerSettings(const MImPluginSettingsInfo &info)
{
    bool found = false;

    for (int i = 0; i < settings.size(); ++i) {
        if (settings[i].plugin_name == info.plugin_name) {
            found = true;
            settings[i].entries.append(info.entries);
            break;
        }
    }

    // No setting info for this plugin yet: add the whole entry
    if (!found) {
        settings.append(info);
    }

    Q_FOREACH (const MImPluginSettingsEntry &entry, info.entries) {
        sharedAttributeExtensionManager->registerPluginSetting(entry.extension_key, entry.type, entry.attributes);
    }
}


void MIMPluginManagerPrivate::setActiveHandlers(const QSet<Maliit::HandlerState> &states)
{
    QSet<Maliit::Plugins::InputMethodPlugin *> activatedPlugins;
    MAbstractInputMethod *inputMethod = 0;

    //clear all cached states before activating new one
    for (Plugins::iterator iterator = plugins.begin();
         iterator != plugins.end();
         ++iterator) {
        iterator->state.clear();
    }

    //activate new plugins
    Q_FOREACH (Maliit::HandlerState state, states) {
        HandlerMap::const_iterator iterator = handlerToPlugin.find(state);
        Maliit::Plugins::InputMethodPlugin *plugin = 0;

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
    Q_FOREACH (Maliit::Plugins::InputMethodPlugin *plugin, activatedPlugins) {
        plugins.value(plugin).inputMethod->setState(plugins.value(plugin).state);
    }

    // deactivate unnecessary plugins
    QSet<Maliit::Plugins::InputMethodPlugin *> previousActive = activePlugins;
    Q_FOREACH (Maliit::Plugins::InputMethodPlugin *plugin, previousActive) {
        if (!activatedPlugins.contains(plugin)) {
            deactivatePlugin(plugin);  //activePlugins is modified here
        }
    }
}


QSet<Maliit::HandlerState> MIMPluginManagerPrivate::activeHandlers() const
{
    QSet<Maliit::HandlerState> handlers;
    Q_FOREACH (Maliit::Plugins::InputMethodPlugin *plugin, activePlugins) {
        handlers << handlerToPlugin.key(plugin);
    }
    return handlers;
}


void MIMPluginManagerPrivate::deactivatePlugin(Maliit::Plugins::InputMethodPlugin *plugin)
{
    Q_Q(MIMPluginManager);
    if (!plugin || !activePlugins.contains(plugin)) {
        return;
    }

    MAbstractInputMethod *inputMethod = 0;

    activePlugins.remove(plugin);
    inputMethod = plugins.value(plugin).inputMethod;

    Q_ASSERT(inputMethod);

    inputMethod->hide();
    inputMethod->reset();

    // this call disables normal behaviour on inputMethod->hide
    plugins.value(plugin).imHost->setEnabled(false);

    plugins[plugin].state = PluginState();
    QObject::disconnect(inputMethod, 0, q, 0);
    targets.remove(inputMethod);
}

void MIMPluginManagerPrivate::replacePlugin(Maliit::SwitchDirection direction,
                                            Maliit::Plugins::InputMethodPlugin *source,
                                            Plugins::iterator replacement,
                                            const QString &subViewId)
{
    PluginState state;
    if (source)
        state = plugins.value(source).state;
    else
        state << Maliit::OnScreen;
    MAbstractInputMethod *switchedTo = 0;

    deactivatePlugin(source);
    activatePlugin(replacement.key());
    switchedTo = replacement->inputMethod;
    replacement->state = state;
    switchedTo->setState(state);
    if (state.contains(Maliit::OnScreen) && !subViewId.isNull()) {
        switchedTo->setActiveSubView(subViewId);
    } else if (replacement->lastSwitchDirection == direction
               || (replacement->lastSwitchDirection == Maliit::SwitchUndefined
                   && direction == Maliit::SwitchBackward)) {
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
    QMap<QString, QSharedPointer<MKeyOverride> > keyOverrides =
        attributeExtensionManager->keyOverrides(toolbarId);
    switchedTo->setKeyOverrides(keyOverrides);

    if (visible) {
        ensureActivePluginsVisible(DontShowInputMethod);
        switchedTo->show();
        switchedTo->showLanguageNotification();
    }

    if (state.contains(Maliit::OnScreen)) {
        if (activeSubViewIdOnScreen != switchedTo->activeSubView(Maliit::OnScreen)) {
            // activeSubViewIdOnScreen is invalid, should be initialized.
            activeSubViewIdOnScreen = switchedTo->activeSubView(Maliit::OnScreen);
        }
        // Save the last active subview
        onScreenPlugins.setActiveSubView(MImOnScreenPlugins::SubView(replacement->pluginId, activeSubViewIdOnScreen));
    }
}


bool MIMPluginManagerPrivate::switchPlugin(Maliit::SwitchDirection direction,
                                           MAbstractInputMethod *initiator)
{
    if (direction != Maliit::SwitchForward
        && direction != Maliit::SwitchBackward) {
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
        if (direction == Maliit::SwitchForward) {
            ++iterator;
            if (iterator == plugins.end()) {
                iterator = plugins.begin();
            }
        } else if (direction == Maliit::SwitchBackward) { // Backward
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

    if (iterator == plugins.end()) {
        qWarning() << __PRETTY_FUNCTION__ << pluginId << "could not be found";
        return false;
    }

    if (source == iterator) {
        return true;
    }

    if (source == plugins.end()) {
        qDebug() << __PRETTY_FUNCTION__ << pluginId << "could not find initiator";
        return trySwitchPlugin(Maliit::SwitchUndefined, 0, iterator, subViewId);
    }

    return trySwitchPlugin(Maliit::SwitchUndefined, source.key(), iterator, subViewId);
}

bool MIMPluginManagerPrivate::trySwitchPlugin(Maliit::SwitchDirection direction,
                                              Maliit::Plugins::InputMethodPlugin *source,
                                              Plugins::iterator replacement,
                                              const QString &subViewId)
{
    Maliit::Plugins::InputMethodPlugin *newPlugin = replacement.key();

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

    if (plugins.value(source).state.contains(Maliit::OnScreen)) {
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

QString MIMPluginManagerPrivate::inputSourceName(Maliit::HandlerState source) const
{
    return inputSourceToNameMap.value(source);
}

void MIMPluginManagerPrivate::changeHandlerMap(Maliit::Plugins::InputMethodPlugin *origin,
                                               Maliit::Plugins::InputMethodPlugin *replacement,
                                               QSet<Maliit::HandlerState> states)
{
    Q_FOREACH (Maliit::HandlerState state, states) {
        if (state == Maliit::OnScreen) {
            continue;
        }
        HandlerMap::iterator iterator = handlerToPlugin.find(state);
        if (iterator != handlerToPlugin.end() && *iterator == origin) {
            *iterator = replacement; //for unit tests
            // Update settings entry to record new plugin for handler map.
            // This should be done after real changing the handler map,
            // to prevent _q_syncHandlerMap also being called to change handler map.
            MImSettings setting(PluginRoot + "/" + inputSourceName(state));
            setting.set(plugins.value(replacement).pluginId);
        }
    }
}

QStringList MIMPluginManagerPrivate::loadedPluginsNames() const
{
    QStringList result;

    Q_FOREACH (const PluginDescription &desc, plugins.values()) {
        result.append(desc.pluginId);
    }

    return result;
}


QStringList MIMPluginManagerPrivate::loadedPluginsNames(Maliit::HandlerState state) const
{
    QStringList result;

    Q_FOREACH (Maliit::Plugins::InputMethodPlugin *plugin, plugins.keys()) {
        if (plugin->supportedStates().contains(state))
            result.append(plugins.value(plugin).pluginId);
    }

    return result;
}


QList<MImPluginDescription> MIMPluginManagerPrivate::pluginDescriptions(Maliit::HandlerState state) const
{
    QList<MImPluginDescription> result;

    const Plugins::const_iterator end = plugins.constEnd();
    for (Plugins::const_iterator iterator(plugins.constBegin());
         iterator != end;
         ++iterator) {
        const Maliit::Plugins::InputMethodPlugin * const plugin = iterator.key();
        if (plugin && plugin->supportedStates().contains(state)) {
            result.append(MImPluginDescription(*plugin));

            if (state == Maliit::OnScreen) {
                result.last().setEnabled(onScreenPlugins.isEnabled(iterator->pluginId));
            }
        }
    }

    return result;
}

MIMPluginManagerPrivate::Plugins::const_iterator
MIMPluginManagerPrivate::findEnabledPlugin(Plugins::const_iterator current,
                                           Maliit::SwitchDirection direction,
                                           Maliit::HandlerState state) const
{
    MIMPluginManagerPrivate::Plugins::const_iterator iterator = current;
    MIMPluginManagerPrivate::Plugins::const_iterator result = plugins.constEnd();

    for (int n = 0; n < plugins.size() - 1; ++n) {
        if (direction == Maliit::SwitchForward) {
            ++iterator;
            if (iterator == plugins.end()) {
                iterator = plugins.begin();
            }
        } else if (direction == Maliit::SwitchBackward) { // Backward
            if (iterator == plugins.begin()) {
                iterator = plugins.end();
            }
            --iterator;
        }

        Maliit::Plugins::InputMethodPlugin *otherPlugin = iterator.key();
        Q_ASSERT(otherPlugin);
        const MIMPluginManagerPrivate::PluginState &supportedStates = otherPlugin->supportedStates();
        if (!supportedStates.contains(state)) {
            continue;
        }

        if (state == Maliit::OnScreen
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
                                                    Maliit::HandlerState state) const
{
    QMap<QString, QString>::iterator iterator = subViews.begin();
    while(iterator != subViews.end()) {
        MImOnScreenPlugins::SubView subView(pluginId, iterator.key());
        if (state != Maliit::OnScreen || onScreenPlugins.isSubViewEnabled(subView)) {
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
MIMPluginManagerPrivate::surroundingSubViewDescriptions(Maliit::HandlerState state) const
{
    QList<MImSubViewDescription> result;
    Maliit::Plugins::InputMethodPlugin *plugin = activePlugin(state);

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
                                    Maliit::SwitchBackward,
                                    state);

    if (otherPlugin != plugins.constEnd()) {
        QMap<QString, QString> prevSubViews = availableSubViews(otherPlugin->pluginId);
        filterEnabledSubViews(prevSubViews, otherPlugin->pluginId, state);
        append(enabledSubViews, prevSubViews, otherPlugin->pluginId);
    }

    append(enabledSubViews, subViews, pluginId);

    otherPlugin = findEnabledPlugin(iterator,
                                    Maliit::SwitchForward,
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

    Q_FOREACH (Maliit::Plugins::InputMethodPlugin *plugin, activePlugins) {
        result.append(plugins.value(plugin).pluginId);
    }

    return result;
}


QString MIMPluginManagerPrivate::activePluginsName(Maliit::HandlerState state) const
{
    Maliit::Plugins::InputMethodPlugin *plugin = activePlugin(state);
    if (!plugin)
        return QString();

    return plugins.value(plugin).pluginId;
}


void MIMPluginManagerPrivate::loadHandlerMap()
{
    Q_Q(MIMPluginManager);
    QSignalMapper *signalMapper = new QSignalMapper(q);

    // Queries all children under PluginRoot, each is a setting entry that maps an
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
    const Maliit::HandlerState source = static_cast<Maliit::HandlerState>(state);

    Maliit::Plugins::InputMethodPlugin *currentPlugin = activePlugin(source);
    MImSettings setting(PluginRoot + "/" + inputSourceName(source));
    const QString pluginId = setting.value().toString();

    // already synchronized.
    if (currentPlugin && pluginId == plugins.value(currentPlugin).pluginId) {
       return;
    }

    Maliit::Plugins::InputMethodPlugin *replacement = 0;
    Q_FOREACH (Maliit::Plugins::InputMethodPlugin *plugin, plugins.keys()) {
        if (plugins.value(plugin).pluginId == pluginId) {
            replacement = plugin;
            break;
        }
    }
    if (replacement) {
        // switch plugin if handler is changed.
        MAbstractInputMethod *inputMethod = plugins.value(currentPlugin).inputMethod;
        addHandlerMap(static_cast<Maliit::HandlerState>(state), pluginId);
        if (!switchPlugin(pluginId, inputMethod)) {
            qWarning() << __PRETTY_FUNCTION__ << ", switching to plugin:"
                       << pluginId << " failed";
        }
    }
}

void MIMPluginManagerPrivate::_q_onScreenSubViewChanged()
{
    const MImOnScreenPlugins::SubView &subView = onScreenPlugins.activeSubView();

    Maliit::Plugins::InputMethodPlugin *currentPlugin = activePlugin(Maliit::OnScreen);

    if (currentPlugin && subView.plugin == plugins.value(currentPlugin).pluginId && activePlugins.contains(currentPlugin)) {
        qDebug() << __PRETTY_FUNCTION__ << "just switch subview";
        _q_setActiveSubView(subView.id, Maliit::OnScreen);
        return;
    }

    Maliit::Plugins::InputMethodPlugin *replacement = 0;
    Q_FOREACH (Maliit::Plugins::InputMethodPlugin *plugin, plugins.keys()) {
        if (plugins.value(plugin).pluginId == subView.plugin) {
            replacement = plugin;
            break;
        }
    }

    if (replacement) {
        // switch plugin if handler is changed.
        MAbstractInputMethod *inputMethod = 0;
        if (activePlugins.contains(currentPlugin))
            inputMethod = plugins.value(currentPlugin).inputMethod;
        addHandlerMap(Maliit::OnScreen, subView.plugin);
        if (!switchPlugin(subView.plugin, inputMethod, subView.id)) {
            qWarning() << __PRETTY_FUNCTION__ << ", switching to plugin:"
                       << subView.plugin << " failed";
        }
    }
}

Maliit::Plugins::InputMethodPlugin *MIMPluginManagerPrivate::activePlugin(Maliit::HandlerState state) const
{
    Maliit::Plugins::InputMethodPlugin *plugin = 0;
    HandlerMap::const_iterator iterator = handlerToPlugin.find(state);
    if (iterator != handlerToPlugin.constEnd()) {
        plugin = iterator.value();
    }
    return plugin;
}

void MIMPluginManagerPrivate::_q_setActiveSubView(const QString &subViewId,
                                                  Maliit::HandlerState state)
{
    // now we only support active subview for OnScreen state.
    if (state != Maliit::OnScreen) {
        qWarning() << "Unsupported state:" << state << " for active subview";
        return;
    }

    if (subViewId.isEmpty()) {
        return;
    }

    Maliit::Plugins::InputMethodPlugin *plugin = activePlugin(Maliit::OnScreen);
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

    Q_FOREACH (const MAbstractInputMethod::MInputMethodSubView &subView,
             inputMethod->subViews(Maliit::OnScreen)) {
        if (subView.subViewId == subViewId) {
            activeSubViewIdOnScreen = subViewId;
            if (inputMethod->activeSubView(Maliit::OnScreen) != activeSubViewIdOnScreen) {
                inputMethod->setActiveSubView(activeSubViewIdOnScreen, Maliit::OnScreen);
            }
            // Save the last active subview
            if (onScreenPlugins.activeSubView().id != subViewId) {
                onScreenPlugins.setActiveSubView(MImOnScreenPlugins::SubView(activePluginId, subViewId));
            }

            break;
        }
    }
}

void MIMPluginManagerPrivate::showActivePlugins()
{
    visible = true;
    ensureActivePluginsVisible(ShowInputMethod);
}

void MIMPluginManagerPrivate::hideActivePlugins()
{
    visible = false;
    Q_FOREACH (Maliit::Plugins::InputMethodPlugin *plugin, activePlugins) {
        plugins.value(plugin).inputMethod->hide();
        plugins.value(plugin).windowGroup->deactivate(Maliit::WindowGroup::HideDelayed);
    }
}

void MIMPluginManagerPrivate::ensureActivePluginsVisible(ShowInputMethodRequest request)
{
    Plugins::iterator iterator(plugins.begin());

    for (; iterator != plugins.end(); ++iterator) {
        if (activePlugins.contains(iterator.key())) {
            iterator.value().windowGroup->activate();
            if (request == ShowInputMethod) {
                iterator.value().inputMethod->show();
            }
        } else {
            iterator.value().windowGroup->deactivate(Maliit::WindowGroup::HideImmediate);
        }
    }
}

QMap<QString, QString>
MIMPluginManagerPrivate::availableSubViews(const QString &plugin,
                                           Maliit::HandlerState state) const
{
    QMap<QString, QString> subViews;
    Plugins::const_iterator iterator(plugins.constBegin());

    for (; iterator != plugins.constEnd(); ++iterator) {
        if (plugins.value(iterator.key()).pluginId == plugin) {
            if (iterator->inputMethod) {
                Q_FOREACH (const MAbstractInputMethod::MInputMethodSubView &subView,
                         iterator->inputMethod->subViews(state)) {
                    subViews.insert(subView.subViewId, subView.subViewTitle);
                }
            }
            break;
        }
    }
    return subViews;
}

QList<MImOnScreenPlugins::SubView>
MIMPluginManagerPrivate::availablePluginsAndSubViews(Maliit::HandlerState state) const
{
    QList<MImOnScreenPlugins::SubView> pluginsAndSubViews;
    Plugins::const_iterator iterator(plugins.constBegin());

    for (; iterator != plugins.constEnd(); ++iterator) {
        if (iterator->inputMethod) {
            const QString plugin = plugins.value(iterator.key()).pluginId;
            Q_FOREACH (const MAbstractInputMethod::MInputMethodSubView &subView,
                     iterator->inputMethod->subViews(state)) {
                pluginsAndSubViews.append(MImOnScreenPlugins::SubView(plugin, subView.subViewId));
            }
        }
    }

    return pluginsAndSubViews;
}

QString MIMPluginManagerPrivate::activeSubView(Maliit::HandlerState state) const
{
    QString subView;
    Maliit::Plugins::InputMethodPlugin *currentPlugin = activePlugin(state);
    if (currentPlugin) {
        subView = plugins.value(currentPlugin).inputMethod->activeSubView(state);
    }
    return subView;
}

void MIMPluginManagerPrivate::setActivePlugin(const QString &pluginId,
                                              Maliit::HandlerState state)
{
    if (state == Maliit::OnScreen) {
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
        Q_FOREACH (Maliit::Plugins::InputMethodPlugin *plugin, plugins.keys()) {
            if (plugins.value(plugin).pluginId == pluginId) {
                currentPluginConf.set(pluginId);
                // Force call _q_syncHandlerMap() even though we already connect
                // _q_syncHandlerMap() with MImSettings valueChanged(). Because if the
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

MIMPluginManager::MIMPluginManager(const QSharedPointer<MInputContextConnection>& icConnection,
                                   const QSharedPointer<Maliit::AbstractPlatform> &platform)
    : QObject(),
      d_ptr(new MIMPluginManagerPrivate(icConnection, platform, this))
{
    Q_D(MIMPluginManager);
    d->q_ptr = this;

    // Connect connection to our handlers
    connect(d->mICConnection.data(), SIGNAL(showInputMethodRequest()),
            this, SLOT(showActivePlugins()));

    connect(d->mICConnection.data(), SIGNAL(hideInputMethodRequest()),
            this, SLOT(hideActivePlugins()));

    connect(d->mICConnection.data(), SIGNAL(resetInputMethodRequest()),
            this, SLOT(resetInputMethods()));

    connect(d->mICConnection.data(), SIGNAL(activeClientDisconnected()),
            this, SLOT(handleClientChange()));

    connect(d->mICConnection.data(), SIGNAL(clientActivated(uint)),
            this, SLOT(handleClientChange()));

    connect(d->mICConnection.data(), SIGNAL(contentOrientationAboutToChangeCompleted(int)),
            this, SLOT(handleAppOrientationAboutToChange(int)));

    connect(d->mICConnection.data(), SIGNAL(contentOrientationChangeCompleted(int)),
            this, SLOT(handleAppOrientationChanged(int)));

    connect(d->mICConnection.data(), SIGNAL(preeditChanged(QString,int)),
            this, SLOT(handlePreeditChanged(QString,int)));

    connect(d->mICConnection.data(), SIGNAL(mouseClickedOnPreedit(QPoint,QRect)),
            this, SLOT(handleMouseClickOnPreedit(QPoint,QRect)));

    connect(d->mICConnection.data(), SIGNAL(receivedKeyEvent(QEvent::Type,Qt::Key,Qt::KeyboardModifiers,QString,bool,int,quint32,quint32,ulong)),
            this, SLOT(processKeyEvent(QEvent::Type,Qt::Key,Qt::KeyboardModifiers,QString,bool,int,quint32,quint32,ulong)));

    connect(d->mICConnection.data(), SIGNAL(widgetStateChanged(uint,QMap<QString,QVariant>,QMap<QString,QVariant>,bool)),
            this, SLOT(handleWidgetStateChanged(uint,QMap<QString,QVariant>,QMap<QString,QVariant>,bool)));

    // Connect connection and MAttributeExtensionManager
    connect(d->mICConnection.data(), SIGNAL(copyPasteStateChanged(bool,bool)),
            d->attributeExtensionManager.data(), SLOT(setCopyPasteState(bool, bool)));

    connect(d->mICConnection.data(), SIGNAL(widgetStateChanged(uint,QMap<QString,QVariant>,QMap<QString,QVariant>,bool)),
            d->attributeExtensionManager.data(), SLOT(handleWidgetStateChanged(uint,QMap<QString,QVariant>,QMap<QString,QVariant>,bool)));

    connect(d->mICConnection.data(), SIGNAL(attributeExtensionRegistered(uint, int, QString)),
            d->attributeExtensionManager.data(), SLOT(handleAttributeExtensionRegistered(uint, int, QString)));

    connect(d->mICConnection.data(), SIGNAL(attributeExtensionUnregistered(uint, int)),
            d->attributeExtensionManager.data(), SLOT(handleAttributeExtensionUnregistered(uint, int)));

    connect(d->mICConnection.data(), SIGNAL(extendedAttributeChanged(uint, int, QString, QString, QString, QVariant)),
            d->attributeExtensionManager.data(), SLOT(handleExtendedAttributeUpdate(uint, int, QString, QString, QString, QVariant)));

    connect(d->attributeExtensionManager.data(), SIGNAL(notifyExtensionAttributeChanged(int, QString, QString, QString, QVariant)),
            d->mICConnection.data(), SLOT(notifyExtendedAttributeChanged(int, QString, QString, QString, QVariant)));

    connect(d->mICConnection.data(), SIGNAL(clientDisconnected(uint)),
            d->attributeExtensionManager.data(), SLOT(handleClientDisconnect(uint)));

    connect(d->mICConnection.data(), SIGNAL(attributeExtensionRegistered(uint, int, QString)),
            d->sharedAttributeExtensionManager.data(), SLOT(handleAttributeExtensionRegistered(uint, int, QString)));

    connect(d->mICConnection.data(), SIGNAL(attributeExtensionUnregistered(uint, int)),
            d->sharedAttributeExtensionManager.data(), SLOT(handleAttributeExtensionUnregistered(uint, int)));

    connect(d->mICConnection.data(), SIGNAL(extendedAttributeChanged(uint, int, QString, QString, QString, QVariant)),
            d->sharedAttributeExtensionManager.data(), SLOT(handleExtendedAttributeUpdate(uint, int, QString, QString, QString, QVariant)));

    connect(d->sharedAttributeExtensionManager.data(), SIGNAL(notifyExtensionAttributeChanged(QList<int>, int, QString, QString, QString, QVariant)),
            d->mICConnection.data(), SLOT(notifyExtendedAttributeChanged(QList<int>, int, QString, QString, QString, QVariant)));

    connect(d->mICConnection.data(), SIGNAL(clientDisconnected(uint)),
            d->sharedAttributeExtensionManager.data(), SLOT(handleClientDisconnect(uint)));

    connect(d->mICConnection.data(), SIGNAL(pluginSettingsRequested(int,QString)),
            this, SLOT(pluginSettingsRequested(int,QString)));

    connect(d->mICConnection.data(), SIGNAL(focusChanged(WId)),
            this, SLOT(handleAppFocusChanged(WId)));

    // Connect from MAttributeExtensionManager to our handlers
    connect(d->attributeExtensionManager.data(), SIGNAL(attributeExtensionIdChanged(const MAttributeExtensionId &)),
            this, SLOT(setToolbar(const MAttributeExtensionId &)));

    connect(d->attributeExtensionManager.data(), SIGNAL(keyOverrideCreated()),
            this, SLOT(updateKeyOverrides()));

    connect(d->attributeExtensionManager.data(), SIGNAL(globalAttributeChanged(MAttributeExtensionId,QString,QString,QVariant)),
            this, SLOT(onGlobalAttributeChanged(MAttributeExtensionId,QString,QString,QVariant)));

    d->paths        = MImSettings(MImPluginPaths).value(QStringList(DefaultPluginLocation)).toStringList();
    d->blacklist    = MImSettings(MImPluginDisabled).value().toStringList();

    d->loadPlugins();

    d->loadHandlerMap();

    d->registerSettings();

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
}


MIMPluginManager::~MIMPluginManager()
{
    Q_D(MIMPluginManager);
    delete d;
}


QStringList MIMPluginManager::loadedPluginsNames() const
{
    Q_D(const MIMPluginManager);
    return d->loadedPluginsNames();
}


QStringList MIMPluginManager::loadedPluginsNames(Maliit::HandlerState state) const
{
    Q_D(const MIMPluginManager);
    return d->loadedPluginsNames(state);
}


QList<MImPluginDescription> MIMPluginManager::pluginDescriptions(Maliit::HandlerState state) const
{
    Q_D(const MIMPluginManager);
    return d->pluginDescriptions(state);
}

QList<MImSubViewDescription>
MIMPluginManager::surroundingSubViewDescriptions(Maliit::HandlerState state) const
{
    Q_D(const MIMPluginManager);
    return d->surroundingSubViewDescriptions(state);
}

QStringList MIMPluginManager::activePluginsNames() const
{
    Q_D(const MIMPluginManager);
    return d->activePluginsNames();
}


QString MIMPluginManager::activePluginsName(Maliit::HandlerState state) const
{
    Q_D(const MIMPluginManager);
    return d->activePluginsName(state);
}

void MIMPluginManager::updateInputSource()
{
    Q_D(MIMPluginManager);
    // Hardware and Accessory can work together.
    // OnScreen is mutually exclusive to Hardware and Accessory.
    QSet<Maliit::HandlerState> handlers = d->activeHandlers();

    if (d->hwkbTracker.isOpen()) {
        // hw keyboard is on
        handlers.remove(Maliit::OnScreen);
        handlers.insert(Maliit::Hardware);
    } else {
        // hw keyboard is off
        handlers.remove(Maliit::Hardware);
        handlers.insert(Maliit::OnScreen);
    }

    if (d->imAccessoryEnabledConf->value().toBool()) {
        handlers.remove(Maliit::OnScreen);
        handlers.insert(Maliit::Accessory);
    } else {
        handlers.remove(Maliit::Accessory);
    }

    if (!handlers.isEmpty()) {
        d->setActiveHandlers(handlers);
    }
}

void MIMPluginManager::switchPlugin(Maliit::SwitchDirection direction,
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

void MIMPluginManager::setAllSubViewsEnabled(bool enable)
{
    Q_D(MIMPluginManager);
    d->onScreenPlugins.setAllSubViewsEnabled(enable);
}

void MIMPluginManager::setToolbar(const MAttributeExtensionId &id)
{
    Q_D(MIMPluginManager);

    // Record MAttributeExtensionId for switch Plugin
    d->toolbarId = id;

    QMap<QString, QSharedPointer<MKeyOverride> > keyOverrides =
        d->attributeExtensionManager->keyOverrides(id);

    bool focusStateOk(false);
    const bool focusState(d->mICConnection->focusState(focusStateOk));

    if (!focusStateOk)
    {
        qCritical() << __PRETTY_FUNCTION__ << ": focus state is invalid.";
    }

    const bool mapEmpty(keyOverrides.isEmpty());
    // setKeyOverrides are not called when keyOverrides map is empty
    // and no widget is focused - we do not want to update keys because either
    // vkb is not visible or another input widget is focused in, so its
    // extension attribute will be used in a moment. without this, some vkbs
    // may have some flickering - first it could show default label and a
    // fraction of second later - an overriden label.
    const bool callKeyOverrides(!(!focusState && mapEmpty));

    Q_FOREACH (Maliit::Plugins::InputMethodPlugin *plugin, d->activePlugins) {
        if (callKeyOverrides)
        {
            d->plugins.value(plugin).inputMethod->setKeyOverrides(keyOverrides);
        }
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
                                                           Maliit::HandlerState state) const
{
    Q_D(const MIMPluginManager);
    return d->availableSubViews(plugin, state);
}

QString MIMPluginManager::activeSubView(Maliit::HandlerState state) const
{
    Q_D(const MIMPluginManager);
    return d->activeSubView(state);
}

void MIMPluginManager::setActivePlugin(const QString &pluginName, Maliit::HandlerState state)
{
    Q_D(MIMPluginManager);
    d->setActivePlugin(pluginName, state);
}

void MIMPluginManager::setActiveSubView(const QString &subViewId, Maliit::HandlerState state)
{
    Q_D(MIMPluginManager);
    d->_q_setActiveSubView(subViewId, state);
}

void MIMPluginManager::updateKeyOverrides()
{
    Q_D(MIMPluginManager);
    QMap<QString, QSharedPointer<MKeyOverride> > keyOverrides =
        d->attributeExtensionManager->keyOverrides(d->toolbarId);

    Q_FOREACH (Maliit::Plugins::InputMethodPlugin *plugin, d->activePlugins) {
        d->plugins.value(plugin).inputMethod->setKeyOverrides(keyOverrides);
    }
}

void MIMPluginManager::handleAppOrientationAboutToChange(int angle)
{
    Q_FOREACH (MAbstractInputMethod *target, targets()) {
        target->handleAppOrientationAboutToChange(angle);
    }
}

void MIMPluginManager::handleAppOrientationChanged(int angle)
{
    Q_D(MIMPluginManager);

    d->lastOrientation = angle;

    Q_FOREACH (MAbstractInputMethod *target, targets()) {
        target->handleAppOrientationChanged(angle);
    }
}

void MIMPluginManager::handleAppFocusChanged(WId id)
{
    Q_D(MIMPluginManager);

    MIMPluginManagerPrivate::Plugins::iterator i = d->plugins.begin();
    while (i != d->plugins.end()) {
        i.value().windowGroup.data()->setApplicationWindow(id);
        ++i;
    }
}

void MIMPluginManager::handleClientChange()
{
    // notify plugins
    Q_FOREACH (MAbstractInputMethod *target, targets()) {
        target->handleClientChange();
    }
}

void MIMPluginManager::handleWidgetStateChanged(unsigned int clientId,
                                                const QMap<QString, QVariant> &newState,
                                                const QMap<QString, QVariant> &oldState,
                                                bool focusChanged)
{
    Q_UNUSED(clientId);

    // check visualization change
    bool oldVisualization = false;
    bool newVisualization = false;

    QVariant variant = oldState[VisualizationAttribute];

    if (variant.isValid()) {
        oldVisualization = variant.toBool();
    }

    variant = newState[VisualizationAttribute];
    if (variant.isValid()) {
        newVisualization = variant.toBool();
    }

    // update state
    QStringList changedProperties;
    for (QMap<QString, QVariant>::const_iterator iter = newState.constBegin();
         iter != newState.constEnd();
         ++iter)
    {
        if (oldState.value(iter.key()) != iter.value()) {
            changedProperties.append(iter.key());
        }

    }

    variant = newState[FocusStateAttribute];
    const bool widgetFocusState = variant.toBool();

    if (focusChanged) {
        Q_FOREACH (MAbstractInputMethod *target, targets()) {
            target->handleFocusChange(widgetFocusState);
        }
    }

    // call notification methods if needed
    if (oldVisualization != newVisualization) {
        Q_FOREACH (MAbstractInputMethod *target, targets()) {
            target->handleVisualizationPriorityChange(newVisualization);
        }
    }

    const Qt::InputMethodHints lastHints = static_cast<Qt::InputMethodHints>(newState.value(Maliit::Internal::inputMethodHints).toLongLong());
    MImUpdateEvent ev(newState, changedProperties, lastHints);

    // general notification last
    Q_FOREACH (MAbstractInputMethod *target, targets()) {
        if (not changedProperties.isEmpty()) {
            (void) target->imExtensionEvent(&ev);
        }
        target->update();
    }

    // Make sure windows get hidden when no longer focus
    if (not widgetFocusState) {
        hideActivePlugins();
    }
}

void MIMPluginManager::handleMouseClickOnPreedit(const QPoint &pos, const QRect &preeditRect)
{
    Q_FOREACH (MAbstractInputMethod *target, targets()) {
        target->handleMouseClickOnPreedit(pos, preeditRect);
    }
}

void MIMPluginManager::handlePreeditChanged(const QString &text, int cursorPos)
{
    Q_FOREACH (MAbstractInputMethod *target, targets()) {
        target->setPreedit(text, cursorPos);
    }
}

void MIMPluginManager::resetInputMethods()
{
    Q_FOREACH (MAbstractInputMethod *target, targets()) {
        target->reset();
    }
}

void MIMPluginManager::processKeyEvent(QEvent::Type keyType, Qt::Key keyCode,
                     Qt::KeyboardModifiers modifiers, const QString &text, bool autoRepeat, int count,
                     quint32 nativeScanCode, quint32 nativeModifiers, unsigned long time)

{
    Q_FOREACH (MAbstractInputMethod *target, targets()) {
        target->processKeyEvent(keyType, keyCode, modifiers, text, autoRepeat, count,
                                nativeScanCode, nativeModifiers, time);
    }
}

QSet<MAbstractInputMethod *> MIMPluginManager::targets()
{
    Q_D(MIMPluginManager);
    return d->targets;
}

void MIMPluginManager::onGlobalAttributeChanged(const MAttributeExtensionId &id,
                                                const QString &targetItem,
                                                const QString &attribute,
                                                const QVariant &value)
{
    Q_D(MIMPluginManager);

    if (targetItem == InputMethodItem
        && attribute == LoadAll) {

        if (value.toBool()) {
            if (const QSharedPointer<MAttributeExtension> &extension =
                    d->attributeExtensionManager->attributeExtension(id)) {
                // Create an object that is bound to the life time of the
                // attribute extension (through QObject ownership hierarchy).
                // Upon destruction, it will reset the all-subviews-enabled
                // override.
               (void) new MImSubViewOverride(&d->onScreenPlugins, extension.data());
            }
        }

        setAllSubViewsEnabled(value.toBool());
    }
}

void MIMPluginManager::pluginSettingsRequested(int clientId, const QString &descriptionLanguage)
{
    Q_D(MIMPluginManager);

    QList<MImPluginSettingsInfo> settings = d->settings;

    for (int i = 0; i < settings.count(); ++i) {
        QList<MImPluginSettingsEntry> &entries = settings[i].entries;

        // TODO translate descriptions using descriptionLanguage
        settings[i].description_language = descriptionLanguage;

        for (int j = 0; j < entries.count(); ++j) {
            // TODO translate descriptions using descriptionLanguage
            entries[j].value = MImSettings(entries[j].extension_key).value(entries[j].attributes.value(Maliit::SettingEntryAttributes::defaultValue));
        }
    }

    d->mICConnection->pluginSettingsLoaded(clientId, settings);
}

AbstractPluginSetting *MIMPluginManager::registerPluginSetting(const QString &pluginId,
                                                               const QString &pluginDescription,
                                                               const QString &key,
                                                               const QString &description,
                                                               Maliit::SettingEntryType type,
                                                               const QVariantMap &attributes)
{
    Q_D(MIMPluginManager);

    MImPluginSettingsEntry entry;
    entry.description = description;
    entry.type = type;
    entry.extension_key = PluginSettings + "/" + pluginId + "/" + key;
    entry.attributes = attributes;

    MImPluginSettingsInfo info;
    info.plugin_name = pluginId;
    info.plugin_description = pluginDescription;
    info.extension_id = MSharedAttributeExtensionManager::PluginSettings;
    info.entries.append(entry);

    d->registerSettings(info);

    return new PluginSetting(key, entry.extension_key, entry.attributes.value(Maliit::SettingEntryAttributes::defaultValue));
}

PluginSetting::PluginSetting(const QString &shortKey, const QString &fullKey, const QVariant &value) :
    pluginKey(shortKey), setting(fullKey), defaultValue(value)
{
    connect(&setting, SIGNAL(valueChanged()), this, SIGNAL(valueChanged()));
}

QString PluginSetting::key() const
{
    return pluginKey;
}

QVariant PluginSetting::value() const
{
    return setting.value(defaultValue);
}

QVariant PluginSetting::value(const QVariant &def) const
{
    return setting.value(def.isValid() ? def : defaultValue);
}

void PluginSetting::set(const QVariant &val)
{
    setting.set(val);
}

void PluginSetting::unset()
{
    setting.unset();
}

#include "moc_mimpluginmanager.cpp"
