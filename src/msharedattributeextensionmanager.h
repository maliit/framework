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


#ifndef MSHAREDATTRIBUTEEXTENSIONMANAGER_H
#define MSHAREDATTRIBUTEEXTENSIONMANAGER_H

#include <maliit/settingdata.h>

#include <QObject>
#include <QHash>
#include <QSharedPointer>

class MSharedAttributeExtensionManagerPluginSetting;

class MSharedAttributeExtensionManager : public QObject
{
    Q_OBJECT

public:
    enum ReservedExtensionIds
    {
        PluginSettings = -3,
        PluginSettingsList = -4
    };

    MSharedAttributeExtensionManager();
    virtual ~MSharedAttributeExtensionManager();

    void registerPluginSetting(const QString &fullName, Maliit::SettingEntryType type, QVariantMap attributes);

public Q_SLOTS:
    void handleClientDisconnect(unsigned int clientId);
    void handleAttributeExtensionRegistered(unsigned int clientId, int id, const QString &attributeExtension);
    void handleAttributeExtensionUnregistered(unsigned int clientId, int id);
    void handleExtendedAttributeUpdate(unsigned int clientId, int id,
                                       const QString &target, const QString &targetName,
                                       const QString &attribute, const QVariant &value);

Q_SIGNALS:
    void notifyExtensionAttributeChanged(const QList<int> &clientIds,
                                         int id,
                                         const QString &target,
                                         const QString &targetItem,
                                         const QString &attribute,
                                         const QVariant &value);

private:
    Q_SLOT void attributeValueChanged();

    typedef QHash<QString, QSharedPointer<MSharedAttributeExtensionManagerPluginSetting> > SharedAttributeExtensionContainer;
    //! all registered attribute extensions
    SharedAttributeExtensionContainer sharedAttributeExtensions;
    QList<int> clientIds;
};

#endif // MSHAREDATTRIBUTEEXTENSIONMANAGER_H
