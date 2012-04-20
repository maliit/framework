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
#include "mimdirectserverconnection.h"
#ifndef M_IM_DISABLE_DBUS
#include "connectionfactory.h"
#endif
#include "pluginsettings.h"


namespace Maliit {

struct SettingsManagerPrivate
{
    QSharedPointer<AttributeExtension> settings_list_changed;
    MImServerConnection *connection;
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

SettingsManager *SettingsManager::create()
{
    QByteArray im_module = qgetenv("QT_IM_MODULE");
    MImServerConnection *serverConnection;

    if (im_module == MALIIT_INPUTCONTEXT_NAME"Direct") {
        serverConnection = MImDirectServerConnection::instance();
    } else if (im_module == MALIIT_INPUTCONTEXT_NAME) {
#ifndef M_IM_DISABLE_DBUS
        const QByteArray overriddenAddress = qgetenv("MALIIT_SERVER_ADDRESS");

        if (overriddenAddress.isEmpty()) {
            serverConnection = Maliit::DBus::createServerConnectionWithDynamicAddress();
        } else {
            serverConnection = Maliit::DBus::createServerConnectionWithFixedAddress(overriddenAddress);
        }
#else
        qCritical("This QT_IM_MODULE is not available since DBus support is disabled");
#endif
    } else {
        qCritical("Invalid value for QT_IM_MODULE, unable to create connection for settings");

        return 0;
    }

    return new SettingsManager(serverConnection);
}

SettingsManager::SettingsManager(MImServerConnection *connection) :
    d_ptr(new SettingsManagerPrivate)
{
    Q_D(SettingsManager);

    d->connection = connection;

    connect(d->connection, SIGNAL(pluginSettingsReceived(QList<MImPluginSettingsInfo>)),
            this, SLOT(onPluginSettingsReceived(QList<MImPluginSettingsInfo>)));
    connect(d->connection, SIGNAL(connected()), this, SIGNAL(connected()));
    connect(d->connection, SIGNAL(disconnected()), this, SIGNAL(disconnected()));
}

SettingsManager::~SettingsManager()
{
}

void SettingsManager::loadPluginSettings() const
{
    Q_D(const SettingsManager);

    d->connection->loadPluginSettings(preferredDescriptionLocale());
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
