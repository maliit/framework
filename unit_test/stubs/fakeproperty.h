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
#ifndef FAKEPROPERTY_H
#define FAKEPROPERTY_H

#include <QObject>
#include <QString>
#include <QVariant>

/**
 * This file defines FakeGConf class that can be used to store settings
 * in treelike structure, like DuiGConfItem does. All settings are
 * stored in-memory only and are valid during the lifetime of FakeGConf
 * instance.
 */

/*!
 * \brief Simple in-memory version of gconf settings database.
 */
class FakeProperty : public QObject
{
    Q_OBJECT

public:
    FakeProperty(const QString &key, QObject *parent = 0);
    virtual ~FakeProperty();

    QString key() const;

    void setValue(const QVariant &v);
    QVariant value();

Q_SIGNALS:
    void valueChanged();

private:
    QString keyName;
    QVariant keyValue;
};

#endif
