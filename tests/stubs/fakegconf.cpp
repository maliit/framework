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

#include "fakegconf.h"
#include <QDebug>

FakeGConf::FakeGConf() :
    root(NULL, "/")
{
}

FakeGConf::~FakeGConf()
{
}

FakeGConfItem *FakeGConf::initKey(const QString &key)
{
    QStringList path = key.split("/", QString::SkipEmptyParts);
    FakeGConfItem *item = &root;
    Q_FOREACH(QString keyName, path) {
        FakeGConfItem *child = item->findChild(keyName);
        if (child == NULL) {
            child = new FakeGConfItem(item, keyName);
        }
        item = child;
    }
    return item;
}

void FakeGConf::setValue(const QString &key, const QVariant &value)
{
    initKey(key)->setValue(value);
}

QVariant FakeGConf::value(const QString &key)
{
    return initKey(key)->value();
}

QStringList FakeGConf::listDirs(const QString &key)
{
    QList<FakeGConfItem *> children = initKey(key)->children();
    QStringList dirs;

    // Get every child that has children, i.e. it is called "directory"
    Q_FOREACH(FakeGConfItem * child, children) {
        if (child->hasChildren())
            dirs << child->name();
    }
    return dirs;
}

QStringList FakeGConf::listEntries(const QString &key)
{
    QList<FakeGConfItem *> children = initKey(key)->children();
    QStringList entries;

    // Get every child that has value
    Q_FOREACH(FakeGConfItem * child, children) {
        if (child->hasValueSet()) {
            QString fullPath = key + "/" + child->name();
            entries << fullPath;
        }
    }
    return entries;
}

FakeGConfItem::FakeGConfItem(FakeGConfItem *parent, const QString &name) :
    parent(parent),
    keyName(name),
    keyValue(QVariant())
{
    if (parent)
        parent->childList.append(this);
}

FakeGConfItem::~FakeGConfItem()
{
    qDeleteAll(childList);
    childList.clear();
}

FakeGConfItem *FakeGConfItem::findChild(const QString &childName)
{
    FakeGConfItem *result = 0;
    Q_FOREACH(FakeGConfItem * child, childList) {
        if (child->keyName == childName) {
            result = child;
            break;
        }
    }
    return result;
}

QList<FakeGConfItem *> FakeGConfItem::children()
{
    return childList;
}

bool FakeGConfItem::hasChildren()
{
    return (childList.count() > 0);
}

bool FakeGConfItem::hasValueSet()
{
    return !keyValue.isNull();
}

QString FakeGConfItem::name()
{
    return keyName;
}

QVariant FakeGConfItem::value()
{
    return keyValue;
}

void FakeGConfItem::setValue(const QVariant &value)
{
    bool changed = false;

    if (keyValue != value)
        changed = true;

    keyValue = value;

    if (changed)
        Q_EMIT valueChanged();
}
