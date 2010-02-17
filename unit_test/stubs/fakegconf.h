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
#ifndef __FAKEGCONF_H__
#define __FAKEGCONF_H__

#include <QObject>
#include <QStringList>
#include <QVariant>

/**
 * This file defines FakeGConf class that can be used to store settings
 * in treelike structure, like DuiGConfItem does. All settings are
 * stored in-memory only and are valid during the lifetime of FakeGConf
 * instance.
 */

/*!
 * \class FakeGConfItem
 * \brief Stores one settings item that has name, and optionally
 *        value and children.
 *
 * This class is for the internal use of FakeGConf class.
 */
class FakeGConfItem : public QObject
{
    Q_OBJECT
public:
    FakeGConfItem(FakeGConfItem *parent, const QString &name);
    ~FakeGConfItem();

    FakeGConfItem *findChild(const QString &childName);
    QList<FakeGConfItem *> children();

    bool hasChildren();
    bool hasValueSet();

    QString name();
    QVariant value();
    void setValue(const QVariant &value);

signals:
    void valueChanged();

private:
    FakeGConfItem *parent;
    QList<FakeGConfItem *> childList;

    QString keyName;
    QVariant keyValue;
};

/*!
 * \class FakeGConf
 * \brief Simple in-memory version of gconf settings database.
 */
class FakeGConf
{
public:
    FakeGConf();
    ~FakeGConf();

    FakeGConfItem *initKey(const QString &key);

    void setValue(const QString &key, const QVariant &value);
    QVariant value(const QString &key);

    QStringList listDirs(const QString &key);
    QStringList listEntries(const QString &key);

private:
    FakeGConfItem root;
};


#endif // __FAKEGCONF_H__
