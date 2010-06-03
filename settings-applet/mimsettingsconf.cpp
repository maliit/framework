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

#include <MGConfItem>
#include <QDir>
#include <QPluginLoader>
#include <QDebug>
#include "minputmethodbase.h"
#include "minputmethodplugin.h"
#include "mimsettingsconf.h"

namespace
{
    const QString DefaultPluginLocation("/usr/lib/meego-im-plugins/");

    const QString ConfigRoot = "/meegotouch/inputmethods/";
    const QString MImPluginPaths = ConfigRoot + "paths";
    const QString MImPluginDisabled = ConfigRoot + "disabledpluginfiles";
}

MImSettingsConf *MImSettingsConf::imSettingsConfInstance = 0;

MImSettingsConf::MImSettingsConf()
    : paths(MGConfItem(MImPluginPaths).value(QStringList(DefaultPluginLocation)).toStringList()),
      blacklist(MGConfItem(MImPluginDisabled).value().toStringList())
{

    loadPlugins();
    loadSettings();
}

MImSettingsConf::~MImSettingsConf()
{
}

void MImSettingsConf::createInstance()
{
    Q_ASSERT(!imSettingsConfInstance);
    if (!imSettingsConfInstance) {
        imSettingsConfInstance = new MImSettingsConf;
    }
}

void MImSettingsConf::destroyInstance()
{
    Q_ASSERT(imSettingsConfInstance);
    delete imSettingsConfInstance;
    imSettingsConfInstance = 0;
}

void MImSettingsConf::loadPlugins()
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

bool MImSettingsConf::loadPlugin(const QString &fileName)
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
                imPlugins.insert(plugin, load.fileName());
                val = true;
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

void MImSettingsConf::loadSettings()
{
    foreach (MInputMethodPlugin *plugin, plugins()) {
        MInputMethodSettingsBase *settings = plugin->createInputMethodSettings();
        if (settings) {
            settingList.append(settings);
        }
    }
}

QList<MInputMethodPlugin *> MImSettingsConf::plugins() const
{
    QList<MInputMethodPlugin *> pluginList;
    foreach (MInputMethodPlugin *plugin, imPlugins.keys()) {
        if (blacklist.contains(imPlugins.value(plugin)))
            continue;
        if (plugin->supportedStates().contains(OnScreen))
            pluginList << plugin;
    }
    return pluginList;
}

QList<MInputMethodSettingsBase *> MImSettingsConf::settings() const
{
    return settingList;
}
