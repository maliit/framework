/* * This file is part of Maliit framework *
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 *
 * Contact: maliit-discuss@lists.maliit.org
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 */

#include "mimsettings.h"
#include "mimsettingsqsettings.h"
#include "config.h"

#include <QString>
#include <QStringList>
#include <QByteArray>
#include <QVariant>
#include <QDebug>

typedef MImSettingsQSettingsBackendFactory MImSettingsDefaultPersistentBackendFactory;

QScopedPointer<MImSettingsBackendFactory> MImSettings::factory;
MImSettings::SettingsType MImSettings::preferredSettingsType = MImSettings::InvalidSettings;

MImSettingsBackend::MImSettingsBackend(QObject *parent) :
    QObject(parent)
{
}

MImSettingsBackend::~MImSettingsBackend()
{
}


QString MImSettings::key() const
{
    return backend->key();
}

QVariant MImSettings::value() const
{
    return backend->value(QVariant());
}

QVariant MImSettings::value(const QVariant &def) const
{
    return backend->value(def);
}

void MImSettings::set(const QVariant &val)
{
    if (val.isValid()) {
        backend->set(val);
    } else {
        backend->unset();
    }
}

void MImSettings::unset()
{
    backend->unset();
}

QList<QString> MImSettings::listDirs() const
{
    return backend->listDirs();
}

QList<QString> MImSettings::listEntries() const
{
    return backend->listEntries();
}

MImSettings::MImSettings(const QString &key, QObject *parent)
    : QObject(parent)
{
    if (!factory) {
        MImSettingsBackendFactory *newFactory = 0;
        switch (preferredSettingsType) {

        case TemporarySettings:
            // Might be possible with other backends as well,
            // but it does not matter as the settings will be temporary
            // and not visible to others anyway
            newFactory = new MImSettingsQSettingsTemporaryBackendFactory;
            break;

        case PersistentSettings:
            newFactory = new MImSettingsDefaultPersistentBackendFactory;
            break;

        case InvalidSettings:
            qFatal("No settings type specified. "
                   "Call MImSettings::setPreferredSettingsType() before making use of MImSettings.");
            break;

        default:
            qCritical() << __PRETTY_FUNCTION__ <<
                           "Invalid value for preferredSettingType." << preferredSettingsType;
        }
        MImSettings::setImplementationFactory(newFactory);
    }

    backend.reset(factory->create(key, this));

    connect(backend.data(), SIGNAL(valueChanged()), this, SIGNAL(valueChanged()));
}

MImSettings::~MImSettings()
{
}

void MImSettings::setPreferredSettingsType(SettingsType setting)
{
    preferredSettingsType = setting;
    factory.reset();
}

void MImSettings::setImplementationFactory(MImSettingsBackendFactory *newFactory)
{
    factory.reset(newFactory);
}

QHash<QString, QVariant> MImSettings::defaults()
{
    QHash<QString, QVariant> defaults;

    defaults[MALIIT_CONFIG_ROOT"plugins/hardware"] =
        MALIIT_DEFAULT_HW_PLUGIN;
    defaults[MALIIT_CONFIG_ROOT"accessoryenabled"] = false;
    defaults[MALIIT_CONFIG_ROOT"multitouch/enabled"] = MALIIT_ENABLE_MULTITOUCH;
    defaults[MALIIT_CONFIG_ROOT"onscreen/enabled"] =
        QStringList() << MALIIT_DEFAULT_PLUGIN ":" MALIIT_DEFAULT_SUBVIEW;
    defaults[MALIIT_CONFIG_ROOT"onscreen/active"] =
        MALIIT_DEFAULT_PLUGIN ":" MALIIT_DEFAULT_SUBVIEW;

    return defaults;
}
