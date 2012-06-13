/* * This file is part of meego-im-framework *
 *
 * Copyright (C) 2012 Mattia Barbon <mattia@develer.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 */

#include "settingsmanager.h"

#include <maliit/settingdata.h>
#include <maliit/attributeextension.h>
#include "mimserverconnection.h"
#include "connectionfactory.h"
#include "pluginsettings.h"


namespace Maliit {

struct SettingsManagerPrivate
{
    QSharedPointer<AttributeExtension> settings_list_changed;
    QSharedPointer<MImServerConnection> connection;
};


QString SettingsManager::preferred_description_locale = "en";

void SettingsManager::setPreferredDescriptionLocale(const QString &localeName)
{
    preferred_description_locale = localeName;
}

QString SettingsManager::preferredDescriptionLocale()
{
    return preferred_description_locale;
}

SettingsManager *SettingsManager::create(QObject *parent)
{
    return new SettingsManager(createServerConnection(qgetenv("QT_IM_MODULE")), parent);
}

SettingsManager::SettingsManager(QSharedPointer<MImServerConnection> connection,
                                 QObject *parent) :
    QObject(parent),
    d_ptr(new SettingsManagerPrivate)
{
    Q_D(SettingsManager);

    d->connection = connection;

    if (d->connection) {
        connect(d->connection.data(), SIGNAL(pluginSettingsReceived(QList<MImPluginSettingsInfo>)),
                this, SLOT(onPluginSettingsReceived(QList<MImPluginSettingsInfo>)));
        connect(d->connection.data(), SIGNAL(connected()), this, SIGNAL(connected()));
        connect(d->connection.data(), SIGNAL(disconnected()), this, SIGNAL(disconnected()));
    } else {
        qCritical() << __PRETTY_FUNCTION__ << "No connection established";
    }
}

SettingsManager::~SettingsManager()
{
}

void SettingsManager::loadPluginSettings() const
{
    Q_D(const SettingsManager);

    if (d->connection) {
        d->connection->loadPluginSettings(preferredDescriptionLocale());
    } else {
        qCritical() << "Could not load plugin settings - no connection to server";
    }
}

void SettingsManager::onPluginSettingsReceived(const QList<MImPluginSettingsInfo> &settings)
{
    Q_D(SettingsManager);

    QList<QSharedPointer<PluginSettings> > result;
    QHash<int, QSharedPointer<AttributeExtension> > extensions;

    Q_FOREACH (const MImPluginSettingsInfo &plugin_info, settings)
    {
        QSharedPointer<AttributeExtension> extension;

        // special case for the attribute extension used to monitor changes in the settings list
        if (plugin_info.plugin_name == "@settings") {
            d->settings_list_changed = QSharedPointer<AttributeExtension>(AttributeExtension::create(plugin_info.extension_id));

            connect(d->settings_list_changed.data(), SIGNAL(extendedAttributeChanged(QString,QVariant)),
                    this, SLOT(loadPluginSettings()));

            continue;
        }

        if (extensions.contains(plugin_info.extension_id))
            extension = extensions[plugin_info.extension_id];
        else
            extension = extensions[plugin_info.extension_id] =
                AttributeExtension::create(plugin_info.extension_id);

        result.append(QSharedPointer<PluginSettings>(new PluginSettings(plugin_info, extension)));
    }

    Q_EMIT pluginSettingsReceived(result);
}

}
