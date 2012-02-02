/* * This file is part of meego-im-framework *
 *
 * Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * Contact: Nokia Corporation (directui@nokia.com)
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

#include "mimsettingsqsettings.h"

#include <QSettings>
#include <QPointer>


typedef QList<MImSettingsQSettingsBackend *> Items;
typedef QHash<QString, Items> ItemMap;

namespace
{
    const QString ORGANIZATION = "maliit.org";
    const QString APPLICATION = "server";

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
    QSettings settings(ORGANIZATION, APPLICATION);

    if (!settings.contains(d->key))
        return MImSettings::defaults().value(d->key, def);

    return settings.value(d->key, def);
}

void MImSettingsQSettingsBackend::set(const QVariant &val)
{
    Q_D(MImSettingsQSettingsBackend);
    QSettings settings(ORGANIZATION, APPLICATION);

    if (val == settings.value(d->key))
        return;

    settings.setValue(d->key, val);
    d->notify();
}

void MImSettingsQSettingsBackend::unset()
{
    Q_D(MImSettingsQSettingsBackend);
    QSettings settings(ORGANIZATION, APPLICATION);

    if (!settings.contains(d->key))
        return;

    settings.remove(d->key);
    d->notify();
}

QList<QString> MImSettingsQSettingsBackend::listDirs() const
{
    Q_D(const MImSettingsQSettingsBackend);
    QSettings settings(ORGANIZATION, APPLICATION);

    settings.beginGroup(d->key);

    return makeAbsolute(d->key, settings.childGroups());
}

QList<QString> MImSettingsQSettingsBackend::listEntries() const
{
    Q_D(const MImSettingsQSettingsBackend);
    QSettings settings(ORGANIZATION, APPLICATION);

    settings.beginGroup(d->key);

    return makeAbsolute(d->key, settings.childKeys());
}

MImSettingsQSettingsBackend::MImSettingsQSettingsBackend(const QString &key, QObject *parent) :
    MImSettingsBackend(parent),
    d_ptr(new MImSettingsQSettingsBackendPrivate)
{
    Q_D(MImSettingsQSettingsBackend);

    d->key = key;
    d->registerInstance(this);
}

MImSettingsQSettingsBackend::~MImSettingsQSettingsBackend()
{
    Q_D(MImSettingsQSettingsBackend);

    d->unregisterInstance(this);
}
