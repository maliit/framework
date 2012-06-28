/* * This file is part of maliit-framework *
 *
 * Copyright (C) 2012 Mattia Barbon <mattia@develer.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 */

#ifndef MALIIT_SETTINGSMANAGER_H
#define MALIIT_SETTINGSMANAGER_H

#include <QScopedPointer>
#include <QSharedPointer>
#include <QVariant>

class MImServerConnection;
class MImPluginSettingsInfo;

namespace Maliit {

class SettingsManagerPrivate;
class PluginSettings;
class AttributeExtension;


/*!
 * \brief Entry point to the settings API
 *
 * Retrieves the list of server/plugin settings from maliit-server
 */
class SettingsManager : public QObject
{
    Q_OBJECT

public:
    /*!
     * \brief Creates a new settings instance,
     *
     * Uses QT_IM_MODULE to determine the connection type (direct, DBus, ...) to maliit-server
     */
    static SettingsManager *create(QObject *parent = 0);

    virtual ~SettingsManager();

    //! Request the list of settings from maliit-server
    Q_SLOT void loadPluginSettings() const;

    //! Sets the preferred locale for human-readable description
    static void setPreferredDescriptionLocale(const QString &localeName);

    //! Gets the preferred locale for human-readable description
    static QString preferredDescriptionLocale();

Q_SIGNALS:
    //! Emitted after \a loadPluginSettings() and when the plugin list changes on the server.
    void pluginSettingsReceived(const QList<QSharedPointer<Maliit::PluginSettings> > &settings);

    void connected();
    void disconnected();

private:
    SettingsManager(QSharedPointer<MImServerConnection> connection, QObject *parent);

    Q_SLOT void onPluginSettingsReceived(const QList<MImPluginSettingsInfo> &settings);

    static QString preferred_description_locale;

    QScopedPointer<SettingsManagerPrivate> d_ptr;

    Q_DISABLE_COPY(SettingsManager)
    Q_DECLARE_PRIVATE(SettingsManager)
};

}

#endif // MALIIT_SETTINGSMANAGER_H
