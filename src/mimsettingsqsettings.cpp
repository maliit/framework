/* * This file is part of Maliit framework *
 *
 * Copyright (C) 2012 Mattia Barbon <mattia@develer.com>
 * Copyright (C) 2012 Openismus GmbH
 *
 * Contact: maliit-discuss@lists.maliit.org
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 */

#include "mimsettingsqsettings.h"

#include <QSettings>
#include <QPointer>


typedef QList<MImSettingsQSettingsBackend *> Items;
typedef QHash<QString, Items> ItemMap;

namespace
{
    const QString Organization = "maliit.org";
    const QString Application = "server";

    QList<QString> makeAbsolute(const QString &prefix, const QList<QString> &entries)
    {
        QList<QString> absolute;

        Q_FOREACH (const QString &path, entries) {
            absolute.append(prefix + "/" + path);
        }

        return absolute;
    }
}


struct MImSettingsQSettingsBackendPrivate {
    QString key;
    static ItemMap registry;
    QSettings *settingsInstance;

    void registerInstance(MImSettingsQSettingsBackend *instance)
    {
        registry[key].append(instance);
    }

    void unregisterInstance(MImSettingsQSettingsBackend *instance)
    {
        ItemMap::iterator items = registry.find(key);

        items->removeOne(instance);
        if (items->isEmpty())
            registry.erase(items);
    }

    void notify()
    {
        // use QPointer to avoid referencing a deleted object in case
        // one slot deletes another MImSettings instance for this key
        QList<QPointer<MImSettingsQSettingsBackend> > items;
        ItemMap::iterator it = registry.find(key);

        Q_FOREACH (MImSettingsQSettingsBackend *item, *it) {
            items.append(item);
        }

        Q_FOREACH (MImSettingsQSettingsBackend *item, items)
        {
            if (item)
                Q_EMIT item->valueChanged();
        }
    }
};

ItemMap MImSettingsQSettingsBackendPrivate::registry;


QString MImSettingsQSettingsBackend::key() const
{
    Q_D(const MImSettingsQSettingsBackend);

    return d->key;
}

QVariant MImSettingsQSettingsBackend::value(const QVariant &def) const
{
    Q_D(const MImSettingsQSettingsBackend);

    if (!d->settingsInstance->contains(d->key))
        return MImSettings::defaults().value(d->key, def);

    return d->settingsInstance->value(d->key, def);
}

void MImSettingsQSettingsBackend::set(const QVariant &val)
{
    Q_D(MImSettingsQSettingsBackend);

    if (val == d->settingsInstance->value(d->key))
        return;

    d->settingsInstance->setValue(d->key, val);
    d->notify();
}

void MImSettingsQSettingsBackend::unset()
{
    Q_D(MImSettingsQSettingsBackend);

    if (!d->settingsInstance->contains(d->key))
        return;

    d->settingsInstance->remove(d->key);
    d->notify();
}

QList<QString> MImSettingsQSettingsBackend::listDirs() const
{
    Q_D(const MImSettingsQSettingsBackend);

    d->settingsInstance->beginGroup(d->key);
    QList<QString> result = makeAbsolute(d->key, d->settingsInstance->childGroups());
    d->settingsInstance->endGroup();

    return result;
}

QList<QString> MImSettingsQSettingsBackend::listEntries() const
{
    Q_D(const MImSettingsQSettingsBackend);

    d->settingsInstance->beginGroup(d->key);
    QList<QString> result = makeAbsolute(d->key, d->settingsInstance->childKeys());
    d->settingsInstance->endGroup();

    return result;
}

MImSettingsQSettingsBackend::MImSettingsQSettingsBackend(QSettings *settingsInstance, const QString &key, QObject *parent) :
    MImSettingsBackend(parent),
    d_ptr(new MImSettingsQSettingsBackendPrivate)
{
    Q_D(MImSettingsQSettingsBackend);

    d->key = key;
    d->settingsInstance = settingsInstance;
    d->registerInstance(this);
}

MImSettingsQSettingsBackend::~MImSettingsQSettingsBackend()
{
    Q_D(MImSettingsQSettingsBackend);

    d->unregisterInstance(this);
}

/* QSettings backend backed by the native settings store for the Maliit Server org. and app. */
MImSettingsQSettingsBackendFactory::MImSettingsQSettingsBackendFactory()
    : mSettings(Organization, Application)
{}

MImSettingsQSettingsBackendFactory::MImSettingsQSettingsBackendFactory(const QString &organization,
                                                                       const QString &application)
    : mSettings(organization, application)
{}

MImSettingsQSettingsBackendFactory::~MImSettingsQSettingsBackendFactory()
{
}

MImSettingsBackend *MImSettingsQSettingsBackendFactory::create(const QString &key, QObject *parent)
{
    return new MImSettingsQSettingsBackend(&mSettings, key, parent);
}

/* QSettings backend backed by a temporary file */
MImSettingsQSettingsTemporaryBackendFactory::MImSettingsQSettingsTemporaryBackendFactory()
    : mTempFile()
{
    // Force backing file to be created, otherwise fileName() returns empty
    mTempFile.open();
    mTempFile.close();

    mSettings.reset(new QSettings(mTempFile.fileName(), QSettings::IniFormat));
}

MImSettingsQSettingsTemporaryBackendFactory::~MImSettingsQSettingsTemporaryBackendFactory()
{
}

MImSettingsBackend *MImSettingsQSettingsTemporaryBackendFactory::create(const QString &key, QObject *parent)
{

    return new MImSettingsQSettingsBackend(mSettings.data(), key, parent);
}
