/* * This file is part of meego-im-framework *
 *
 * Copyright (C) 2012 Mattia Barbon <mattia@develer.com>
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

#include "dbuscustomarguments.h"

#include  <maliit/settingdata.h>

#include <QDBusArgument>


QDBusArgument &operator<<(QDBusArgument &argument, const MImPluginSettingsEntry &entry)
{
    argument.beginStructure();
    argument << entry.description;
    argument << entry.extension_key;
    argument << static_cast<int>(entry.type);
    // DBus does not have an "invalid variant" value, so we encode the validity as
    // a separate boolean field
    argument << entry.value.isValid();
    if (entry.value.isValid())
        argument << QDBusVariant(entry.value);
    else
        argument << QDBusVariant(QVariant(0));
    argument << entry.attributes;
    argument.endStructure();

    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, MImPluginSettingsEntry &entry)
{
    bool valid_value;
    int type;

    argument.beginStructure();
    argument >> entry.description;
    argument >> entry.extension_key;
    argument >> type;
    // see comment in operator<<
    argument >> valid_value;
    argument >> entry.value;
    if (!valid_value)
        entry.value = QVariant();

    argument >> entry.attributes;
    argument.endStructure();

    entry.type = static_cast<Maliit::SettingEntryType>(type);

    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const MImPluginSettingsInfo &info)
{
    argument.beginStructure();
    argument << info.description_language;
    argument << info.plugin_name;
    argument << info.plugin_description;
    argument << info.extension_id;
    argument << info.entries;
    argument.endStructure();

    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, MImPluginSettingsInfo &info)
{
    argument.beginStructure();
    argument >> info.description_language;
    argument >> info.plugin_name;
    argument >> info.plugin_description;
    argument >> info.extension_id;
    argument >> info.entries;
    argument.endStructure();

    return argument;
}
