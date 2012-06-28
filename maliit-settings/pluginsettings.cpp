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

#include "pluginsettings.h"

#include <maliit/settingdata.h>
#include <maliit/attributeextension.h>
#include "settingsentry.h"

namespace Maliit {

struct PluginSettingsPrivate
{
    QString description_language;
    QString plugin_name;
    QString plugin_description;
    QList<QSharedPointer<SettingsEntry> > entries;
};


PluginSettings::PluginSettings(const MImPluginSettingsInfo &info, const QSharedPointer<AttributeExtension> &extension) :
    d_ptr(new PluginSettingsPrivate)
{
    Q_D(PluginSettings);

    d->description_language = info.description_language;
    d->plugin_name = info.plugin_name;
    d->plugin_description = info.plugin_description;

    Q_FOREACH (const MImPluginSettingsEntry &entry, info.entries)
    {
        d->entries.append(QSharedPointer<SettingsEntry>(new SettingsEntry(extension, entry)));
    }
}

PluginSettings::~PluginSettings()
{
}

QString PluginSettings::descriptionLanguage() const
{
    Q_D(const PluginSettings);

    return d->description_language;
}

QString PluginSettings::pluginName() const
{
    Q_D(const PluginSettings);

    return d->plugin_name;
}

QString PluginSettings::pluginDescription() const
{
    Q_D(const PluginSettings);

    return d->plugin_description;
}


QList<QSharedPointer<SettingsEntry> > PluginSettings::configurationEntries() const
{
    Q_D(const PluginSettings);

    return d->entries;
}

}
