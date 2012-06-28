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

#include "settingsentry.h"

#include <maliit/settingdata.h>
#include <maliit/attributeextension.h>

namespace Maliit {

struct SettingsEntryPrivate
{
    QSharedPointer<AttributeExtension> extension;
    QString description;
    QString extension_key;
    SettingEntryType type;
    QVariantMap attributes;
};


SettingsEntry::SettingsEntry(QSharedPointer<AttributeExtension> extension, const MImPluginSettingsEntry &info) :
    d_ptr(new SettingsEntryPrivate)
{
    Q_D(SettingsEntry);

    d->extension = extension;
    d->description = info.description;
    d->extension_key = info.extension_key;
    d->type = info.type;
    d->attributes = info.attributes;

    d->extension->updateAttribute(info.extension_key, info.value);

    connect(d->extension.data(), SIGNAL(extendedAttributeChanged(QString,QVariant)),
            this, SLOT(valueChanged(QString)));
}

SettingsEntry::~SettingsEntry()
{
}

void SettingsEntry::valueChanged(const QString &key)
{
    Q_D(SettingsEntry);

    if (key == d->extension_key)
        Q_EMIT valueChanged();
}

QString SettingsEntry::description() const
{
    Q_D(const SettingsEntry);

    return d->description;
}

SettingEntryType SettingsEntry::type() const
{
    Q_D(const SettingsEntry);

    return d->type;
}

QVariantMap SettingsEntry::attributes() const
{
    Q_D(const SettingsEntry);

    return d->attributes;
}

QString SettingsEntry::key() const
{
    Q_D(const SettingsEntry);

    return d->extension_key;
}

QVariant SettingsEntry::value() const
{
    return value(QVariant());
}

QVariant SettingsEntry::value(const QVariant &def) const
{
    Q_D(const SettingsEntry);

    return d->extension->attributes().value(d->extension_key, def);
}

bool SettingsEntry::isValid(const QVariant &value)
{
    Q_D(SettingsEntry);

    return validateSettingValue(d->type, d->attributes, value);
}

void SettingsEntry::set(const QVariant &value)
{
    Q_D(SettingsEntry);

    d->extension->setAttribute(d->extension_key, value);
}

}
