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


class SettingsManager : public QObject
{
    Q_OBJECT

public:
    static SettingsManager *create();

    virtual ~SettingsManager();

    Q_SLOT void loadPluginSettings() const;

    static void setPreferredDescriptionLocale(const QString &localeName);
    static QString preferredDescriptionLocale();

Q_SIGNALS:
    void pluginSettingsReceived(const QList<QSharedPointer<Maliit::PluginSettings> > &settings);

    void connected();
    void disconnected();

private:
    SettingsManager(MImServerConnection *connection);

    Q_SLOT void onPluginSettingsReceived(const QList<MImPluginSettingsInfo> &settings);

    static QString preferred_description_locale;

    QScopedPointer<SettingsManagerPrivate> d_ptr;

    Q_DISABLE_COPY(SettingsManager)
    Q_DECLARE_PRIVATE(SettingsManager)
};

}

#endif // MALIIT_SETTINGSMANAGER_H
