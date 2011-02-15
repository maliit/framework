/* * This file is part of meego-im-framework *
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
#ifndef __MIMSETTINGS_STUB_H__
#define __MIMSETTINGS_STUB_H__

#include "mimsettings.h"
#include "fakegconf.h"

#include <QDebug>


/**
 * MImSettings stub class.
 * To fake MImSettings operations, derive from this class
 * and implement the methods you want to fake. Instantiate your
 * derived stub class and assign it to gMImSettingsStub
 * global variable.
 */
class MImSettingsStub
{
public:
    virtual void mGConfItemConstructor(const MImSettings *instance, const QString &key, QObject *parent = 0);
    virtual void mGConfItemDestructor(const MImSettings *instance);
    virtual QString key(const MImSettings *instance);
    virtual QVariant value(const MImSettings *instance);
    virtual QVariant value(const MImSettings *instance, const QVariant &def);
    virtual void set(const MImSettings *instance, const QVariant &val);
    virtual QList<QString> listDirs(const MImSettings *instance);
    virtual QList<QString> listEntries(const MImSettings *instance);

protected:
    QMap<const MImSettings *, QString> instanceKeys; // This map links MImSettings instance to its present key.
    FakeGConf fakeGConf; // This is the in-memory storage for settings.
};

void MImSettingsStub::mGConfItemConstructor(const MImSettings *instance, const QString &key, QObject *)
{
    if (!key.isEmpty()) {
        FakeGConfItem *fakeItem = fakeGConf.initKey(key);
        QObject::connect(fakeItem, SIGNAL(valueChanged()), instance, SIGNAL(valueChanged()));

        instanceKeys[instance] = key;
    }
}

void MImSettingsStub::mGConfItemDestructor(const MImSettings *instance)
{
    instanceKeys.remove(instance);
}

QString MImSettingsStub::key(const MImSettings *instance)
{
    return instanceKeys[instance];
}

QVariant MImSettingsStub::value(const MImSettings *instance)
{
    return fakeGConf.value(instanceKeys[instance]);
}

QVariant MImSettingsStub::value(const MImSettings *instance, const QVariant &def)
{
    QVariant val = fakeGConf.value(instanceKeys[instance]);
    if (val.isNull())
        val = def;
    return val;
}

void MImSettingsStub::set(const MImSettings *instance, const QVariant &value)
{
    fakeGConf.setValue(instanceKeys[instance], value);
}

QList<QString> MImSettingsStub::listDirs(const MImSettings *instance)
{
    return fakeGConf.listDirs(instanceKeys[instance]);
}

QList<QString> MImSettingsStub::listEntries(const MImSettings *instance)
{
    return fakeGConf.listEntries(instanceKeys[instance]);
}

/**
 * This is the stub class instance used by the system. If you want to alter behaviour,
 * derive your stub class from MImSettingsStub, implement the methods you want to
 * fake, create an instance of your stub class and assign the instance into this global variable.
 */
// this dynamic alloc for gMImSettingsStub will cause memory leak. But it is accaptable for
// // unit test, because it is just a small memory leak. And this can avoid core dump if there are
// // some static MImSettings object declared by application.
MImSettingsStub *gMImSettingsStub = new MImSettingsStub;

/**
 * These are the proxy method implementations of MImSettings. They will
 * call the stub object methods of the gMImSettingsStub.
 */

MImSettings::MImSettings(const QString &key, QObject *parent)
{
    gMImSettingsStub->mGConfItemConstructor(this, key, parent);
}

MImSettings::~MImSettings()
{
    gMImSettingsStub->mGConfItemDestructor(this);
}

QString MImSettings::key() const
{
    return gMImSettingsStub->key(this);
}

QVariant MImSettings::value() const
{
    return gMImSettingsStub->value(this);
}

QVariant MImSettings::value(const QVariant &def) const
{
    return gMImSettingsStub->value(this, def);
}

void MImSettings::set(const QVariant &val)
{
    gMImSettingsStub->set(this, val);
}

QList<QString> MImSettings::listDirs() const
{
    return gMImSettingsStub->listDirs(this);
}

QList<QString> MImSettings::listEntries() const
{
    return gMImSettingsStub->listEntries(this);
}

#endif //__MIMSETTINGS_STUB_H__
