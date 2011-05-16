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

#include "minputmethodplugin.h"
#include "mimsettingsconf.h"
#include "mimsettings.h"

#include <QDir>
#include <QPluginLoader>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDebug>

namespace
{
    const char * const DefaultPluginLocation(M_IM_PLUGINS_DIR);

    const QString ConfigRoot = "/meegotouch/inputmethods/";
    const QString MImPluginPaths = ConfigRoot + "paths";
    const QString MImPluginDisabled = ConfigRoot + "disabledpluginfiles";

    const char * const DBusMIMPluginManagerServiceName = "com.meego.inputmethodpluginmanager1";
    const char * const DBusMIMPluginManagerPath = "/com/meego/inputmethodpluginmanager1";
    const char * const DBusMIMPluginManagerInterface = "com.meego.inputmethodpluginmanager1";
}

MImSettingsConf *MImSettingsConf::imSettingsConfInstance = 0;

MImSettingsConf::MImSettingsConf()
    : paths(MImSettings(MImPluginPaths).value(QStringList(DefaultPluginLocation)).toStringList()),
      blacklist(MImSettings(MImPluginDisabled).value().toStringList())
{
    connectToIMPluginManagerDBus();

    loadPlugins();
    loadSettings();
}

MImSettingsConf::~MImSettingsConf()
{
    delete impluginMgrIface;
    impluginMgrIface = 0;
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
        MAbstractInputMethodSettings *settings = plugin->createInputMethodSettings();
        if (settings) {
            settingList.insert(plugin->name(), settings);
        }
    }
}

QList<MInputMethodPlugin *> MImSettingsConf::plugins() const
{
    QList<MInputMethodPlugin *> pluginList;
    foreach (MInputMethodPlugin *plugin, imPlugins.keys()) {
        if (blacklist.contains(imPlugins.value(plugin)))
            continue;
        if (plugin->supportedStates().contains(MInputMethod::OnScreen))
            pluginList << plugin;
    }
    return pluginList;
}

QMap<QString, MAbstractInputMethodSettings *> MImSettingsConf::settings() const
{
    return settingList;
}

QList<MImSettingsConf::MImSubView> MImSettingsConf::subViews() const
{
    QList<MImSubView> views;
    if (impluginMgrIface) {
        QDBusReply<QStringList> reply
            = impluginMgrIface->call("queryAvailablePlugins", MInputMethod::OnScreen);
        if (reply.isValid()) {
            foreach (const QString &plugin, reply.value()) {
                QDBusReply< QMap<QString, QVariant> > replySubViews
                    = impluginMgrIface->call("queryAvailableSubViews",
                                             plugin, MInputMethod::OnScreen);
                if (replySubViews.isValid()) {
                    QMap<QString, QVariant> sv = replySubViews.value();
                    QMap<QString, QVariant>::const_iterator iterator = sv.constBegin();
                    while (iterator != sv.constEnd()) {
                        MImSubView subView = { plugin, iterator.key(), iterator.value().toString()};
                        views << subView;
                        iterator++;
                    }
                }
            }
        }
    }
    return views;
}

void MImSettingsConf::connectToIMPluginManagerDBus()
{
    qDebug() << __PRETTY_FUNCTION__;
    QDBusConnection connection = QDBusConnection::sessionBus();

    if (!connection.isConnected()) {
        qWarning() << "Cannot connect to the DBus session bus";
        return;
    }
    impluginMgrIface = new QDBusInterface(DBusMIMPluginManagerServiceName, DBusMIMPluginManagerPath,
                                          DBusMIMPluginManagerInterface, connection);

    if (!impluginMgrIface->isValid()) {
        qWarning() << "MImSettingsConf was unable to connect to indicator server: "
                   << connection.lastError().message();
        delete impluginMgrIface;
        impluginMgrIface = 0;
    }
}
