/***************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (directui@nokia.com)
**
** This file is part of libmeegotouch.
**
** If you have questions regarding the use of this file, please contact
** Nokia at directui@nokia.com.
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation
** and appearing in the file LICENSE.LGPL included in the packaging
** of this file.
**
****************************************************************************/

#include "mimsettings.h"
#if !defined(M_IM_DISABLE_GCONF)
#include "mimsettingsgconf.h"
#endif
#include "mimsettingsqsettings.h"
#include "config.h"

#include <QString>
#include <QStringList>
#include <QByteArray>
#include <QVariant>
#include <QDebug>


QScopedPointer<MImSettingsBackendFactory> MImSettings::factory;


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
    backend->set(val);
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
    if (factory) {
        backend.reset(factory->create(key, this));
    } else {
#if defined(M_IM_DISABLE_GCONF)
        backend.reset(new MImSettingsQSettingsBackend(key, this));
#else
        backend.reset(new MImSettingsGConfBackend(key, this));
#endif
    }

    connect(backend.data(), SIGNAL(valueChanged()), this, SIGNAL(valueChanged()));
}

MImSettings::~MImSettings()
{
}

void MImSettings::setImplementationFactory(MImSettingsBackendFactory *newFactory)
{
    factory.reset(newFactory);
}

QHash<QString, QVariant> MImSettings::defaults()
{
    QHash<QString, QVariant> defaults;

    defaults[MALIIT_CONFIG_ROOT"plugins/hardware"] =
        M_IM_DEFAULT_HW_PLUGIN;
    defaults[MALIIT_CONFIG_ROOT"accessoryenabled"] = false;
    defaults[MALIIT_CONFIG_ROOT"multitouch/enabled"] = M_IM_ENABLE_MULTITOUCH;
    defaults[MALIIT_CONFIG_ROOT"onscreen/enabled"] =
        QStringList() << M_IM_DEFAULT_PLUGIN << M_IM_DEFAULT_SUBVIEW;
    defaults[MALIIT_CONFIG_ROOT"onscreen/active"] =
        QStringList() << M_IM_DEFAULT_PLUGIN << M_IM_DEFAULT_SUBVIEW;

    return defaults;
}
