/* * This file is part of dui-im-framework *
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
#ifndef __DUIGCONFITEM_STUB_H__
#define __DUIGCONFITEM_STUB_H__

#include <DuiGConfItem>
#include <QDebug>

#include "fakegconf.h"

/**
 * DuiGConfItem stub class.
 * To fake DuiGConfItem operations, derive from this class
 * and implement the methods you want to fake. Instantiate your
 * derived stub class and assign it to gDuiGConfItemStub
 * global variable.
 */
class DuiGConfItemStub
{
public:
    virtual void duiGConfItemConstructor(const DuiGConfItem *instance, const QString &key, QObject *parent = 0);
    virtual void duiGConfItemDestructor(const DuiGConfItem *instance);
    virtual QString key(const DuiGConfItem *instance);
    virtual QVariant value(const DuiGConfItem *instance);
    virtual QVariant value(const DuiGConfItem *instance, const QVariant &def);
    virtual void set(const DuiGConfItem *instance, const QVariant &val);
    virtual QList<QString> listDirs(const DuiGConfItem *instance);
    virtual QList<QString> listEntries(const DuiGConfItem *instance);

protected:
    QMap<const DuiGConfItem *, QString> instanceKeys; // This map links DuiGConfItem instance to its present key.
    FakeGConf fakeGConf; // This is the in-memory storage for settings.
};

void DuiGConfItemStub::duiGConfItemConstructor(const DuiGConfItem *instance, const QString &key, QObject *)
{
    if (!key.isEmpty()) {
        FakeGConfItem *fakeItem = fakeGConf.initKey(key);
        QObject::connect(fakeItem, SIGNAL(valueChanged()), instance, SIGNAL(valueChanged()));

        instanceKeys[instance] = key;
    }
}

void DuiGConfItemStub::duiGConfItemDestructor(const DuiGConfItem *instance)
{
    instanceKeys.remove(instance);
}

QString DuiGConfItemStub::key(const DuiGConfItem *instance)
{
    return instanceKeys[instance];
}

QVariant DuiGConfItemStub::value(const DuiGConfItem *instance)
{
    return fakeGConf.value(instanceKeys[instance]);
}

QVariant DuiGConfItemStub::value(const DuiGConfItem *instance, const QVariant &def)
{
    QVariant val = fakeGConf.value(instanceKeys[instance]);
    if (val.isNull())
        val = def;
    return val;
}

void DuiGConfItemStub::set(const DuiGConfItem *instance, const QVariant &value)
{
    fakeGConf.setValue(instanceKeys[instance], value);
}

QList<QString> DuiGConfItemStub::listDirs(const DuiGConfItem *instance)
{
    return fakeGConf.listDirs(instanceKeys[instance]);
}

QList<QString> DuiGConfItemStub::listEntries(const DuiGConfItem *instance)
{
    return fakeGConf.listEntries(instanceKeys[instance]);
}

DuiGConfItemStub gDefaultDuiGConfItemStub;

/**
 * This is the stub class instance used by the system. If you want to alter behaviour,
 * derive your stub class from DuiGConfItemStub, implement the methods you want to
 * fake, create an instance of your stub class and assign the instance into this global variable.
 */
DuiGConfItemStub *gDuiGConfItemStub = &gDefaultDuiGConfItemStub;

/**
 * These are the proxy method implementations of DuiGConfItem. They will
 * call the stub object methods of the gDuiGConfItemStub.
 */

DuiGConfItem::DuiGConfItem(const QString &key, QObject *parent)
{
    gDuiGConfItemStub->duiGConfItemConstructor(this, key, parent);
}

DuiGConfItem::~DuiGConfItem()
{
    gDuiGConfItemStub->duiGConfItemDestructor(this);
}

QString DuiGConfItem::key() const
{
    return gDuiGConfItemStub->key(this);
}

QVariant DuiGConfItem::value() const
{
    return gDuiGConfItemStub->value(this);
}

QVariant DuiGConfItem::value(const QVariant &def) const
{
    return gDuiGConfItemStub->value(this, def);
}

void DuiGConfItem::set(const QVariant &val)
{
    gDuiGConfItemStub->set(this, val);
}

QList<QString> DuiGConfItem::listDirs() const
{
    return gDuiGConfItemStub->listDirs(this);
}

QList<QString> DuiGConfItem::listEntries() const
{
    return gDuiGConfItemStub->listEntries(this);
}

#endif //__DUIGCONFITEM_STUB_H__
