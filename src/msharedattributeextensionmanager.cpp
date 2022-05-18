/* * This file is part of Maliit framework *
 *
 * Copyright (C) 2012 Mattia Barbon <mattia@develer.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 */

#include "msharedattributeextensionmanager.h"
#include "mimsettings.h"

#include <maliit/namespace.h>

struct MSharedAttributeExtensionManagerPluginSetting
{
    MSharedAttributeExtensionManagerPluginSetting(const QString &key, QVariantMap attributes) :
        setting(key),
        attributes(attributes)
    {
    }

    MImSettings setting;
    QVariantMap attributes;
};


MSharedAttributeExtensionManager::MSharedAttributeExtensionManager()
{
}

MSharedAttributeExtensionManager::~MSharedAttributeExtensionManager()
{
}

void MSharedAttributeExtensionManager::handleClientDisconnect(unsigned int clientId)
{
    clientIds.removeOne(clientId);
}

void MSharedAttributeExtensionManager::handleAttributeExtensionRegistered(unsigned int clientId, int id,
                                                                          const QString &attributeExtension)
{
    Q_UNUSED(attributeExtension);

    if (id != PluginSettings)
        return;
    if (clientIds.contains(clientId))
        return;

    clientIds.append(clientId);
}

void MSharedAttributeExtensionManager::handleAttributeExtensionUnregistered(unsigned int clientId, int id)
{
    if (id != PluginSettings)
        return;

    clientIds.removeOne(clientId);
}

void MSharedAttributeExtensionManager::handleExtendedAttributeUpdate(unsigned int clientId, int id,
                                   const QString &target, const QString &targetName,
                                   const QString &attribute, const QVariant &value)
{
    Q_UNUSED(clientId);

    if (id != PluginSettings)
        return;

    QString key = QString::fromLatin1("%1/%2/%3").arg(target, targetName, attribute);
    SharedAttributeExtensionContainer::iterator it = sharedAttributeExtensions.find(key);

    if (it == sharedAttributeExtensions.end())
        return;

    it->data()->setting.set(value);
}

void MSharedAttributeExtensionManager::attributeValueChanged()
{
    MImSettings *value = qobject_cast<MImSettings *>(sender());

    if (!value)
        return;
    if (sharedAttributeExtensions.find(value->key()) == sharedAttributeExtensions.end())
        return;

    const QString fullName = value->key();
    const QString &target = QString::fromLatin1("/") + fullName.section('/', 1, 1);
    const QString &targetItem = fullName.section('/', 2, -2);
    const QString &attribute = fullName.section('/', -1, -1);

    Q_EMIT notifyExtensionAttributeChanged(clientIds, PluginSettings, target, targetItem, attribute, value->value());
}
