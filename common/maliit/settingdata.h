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

#ifndef MALIIT_SETTINGDATA_H
#define MALIIT_SETTINGDATA_H

#include <maliit/namespace.h>

#include <QString>
#include <QVariant>
#include <QList>


/*!
 * \brief Single configuration entry for Maliit input method plugin
 *
 * \sa Maliit::SettingsEntry
 */
struct MImPluginSettingsEntry
{
    //! Human-readable description
    QString description;
    //! The attribute extension key attribute for this entry
    QString extension_key;
    //! Entry value type
    Maliit::SettingEntryType type;
    //! Current value for the configuration entry
    QVariant value;
    //! Attributes, including domain values and descriptions
    QVariantMap attributes;
};


/*!
 * \brief Settings for a Maliit input method plugin
 */
struct MImPluginSettingsInfo
{
    //! The language used for human-readable descriptions
    QString description_language;
    //! Internal plugin name; "server" for global configuration entries
    QString plugin_name;
    //! Human-readable plugin description
    QString plugin_description;
    //! Attribute extension id; multiple MImPluginSettingsInfo instances might share the same extension id
    int extension_id;
    //! List of configuration entries for this plugin
    QList<MImPluginSettingsEntry> entries;
};


/*!
 * \brief Validate the value for a plugin setting entry
 */
bool validateSettingValue(Maliit::SettingEntryType type, const QVariantMap attributes, const QVariant &value);

Q_DECLARE_METATYPE(MImPluginSettingsEntry)
Q_DECLARE_METATYPE(MImPluginSettingsInfo)
Q_DECLARE_METATYPE(QList<MImPluginSettingsInfo>)

#endif
