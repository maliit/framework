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
#ifndef __MGCONFITEM_STUB_H__
#define __MGCONFITEM_STUB_H__

#include <MGConfItem>
#include <QDebug>

#include "fakegconf.h"

/**
 * MGConfItem stub class.
 * To fake MGConfItem operations, derive from this class
 * and implement the methods you want to fake. Instantiate your
 * derived stub class and assign it to gMGConfItemStub
 * global variable.
 */
class MGConfItemStub
{
public:
    virtual void mGConfItemConstructor(const MGConfItem *instance, const QString &key, QObject *parent = 0);
    virtual void mGConfItemDestructor(const MGConfItem *instance);
    virtual QString key(const MGConfItem *instance);
    virtual QVariant value(const MGConfItem *instance);
    virtual QVariant value(const MGConfItem *instance, const QVariant &def);
    virtual void set(const MGConfItem *instance, const QVariant &val);
    virtual QList<QString> listDirs(const MGConfItem *instance);
    virtual QList<QString> listEntries(const MGConfItem *instance);

protected:
    QMap<const MGConfItem *, QString> instanceKeys; // This map links MGConfItem instance to its present key.
    FakeGConf fakeGConf; // This is the in-memory storage for settings.
};

void MGConfItemStub::mGConfItemConstructor(const MGConfItem *instance, const QString &key, QObject *)
{
    if (!key.isEmpty()) {
        FakeGConfItem *fakeItem = fakeGConf.initKey(key);
        QObject::connect(fakeItem, SIGNAL(valueChanged()), instance, SIGNAL(valueChanged()));

        instanceKeys[instance] = key;
    }
}

void MGConfItemStub::mGConfItemDestructor(const MGConfItem *instance)
{
    instanceKeys.remove(instance);
}

QString MGConfItemStub::key(const MGConfItem *instance)
{
    return instanceKeys[instance];
}

QVariant MGConfItemStub::value(const MGConfItem *instance)
{
    return fakeGConf.value(instanceKeys[instance]);
}

QVariant MGConfItemStub::value(const MGConfItem *instance, const QVariant &def)
{
    QVariant val = fakeGConf.value(instanceKeys[instance]);
    if (val.isNull())
        val = def;
    return val;
}

void MGConfItemStub::set(const MGConfItem *instance, const QVariant &value)
{
    fakeGConf.setValue(instanceKeys[instance], value);
}

QList<QString> MGConfItemStub::listDirs(const MGConfItem *instance)
{
    return fakeGConf.listDirs(instanceKeys[instance]);
}

QList<QString> MGConfItemStub::listEntries(const MGConfItem *instance)
{
    return fakeGConf.listEntries(instanceKeys[instance]);
}

/**
 * This is the stub class instance used by the system. If you want to alter behaviour,
 * derive your stub class from MGConfItemStub, implement the methods you want to
 * fake, create an instance of your stub class and assign the instance into this global variable.
 */
// this dynamic alloc for gMGConfItemStub will cause memory leak. But it is accaptable for
// // unit test, because it is just a small memory leak. And this can avoid core dump if there are
// // some static MGConfItem object declared by application.
MGConfItemStub *gMGConfItemStub = new MGConfItemStub;

/**
 * These are the proxy method implementations of MGConfItem. They will
 * call the stub object methods of the gMGConfItemStub.
 */

MGConfItem::MGConfItem(const QString &key, QObject *parent)
{
    gMGConfItemStub->mGConfItemConstructor(this, key, parent);
}

MGConfItem::~MGConfItem()
{
    gMGConfItemStub->mGConfItemDestructor(this);
}

QString MGConfItem::key() const
{
    return gMGConfItemStub->key(this);
}

QVariant MGConfItem::value() const
{
    return gMGConfItemStub->value(this);
}

QVariant MGConfItem::value(const QVariant &def) const
{
    return gMGConfItemStub->value(this, def);
}

void MGConfItem::set(const QVariant &val)
{
    gMGConfItemStub->set(this, val);
}

QList<QString> MGConfItem::listDirs() const
{
    return gMGConfItemStub->listDirs(this);
}

QList<QString> MGConfItem::listEntries() const
{
    return gMGConfItemStub->listEntries(this);
}

#endif //__MGCONFITEM_STUB_H__
