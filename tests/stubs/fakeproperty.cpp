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

#include "fakeproperty.h"
#include <QDebug>

FakeProperty::FakeProperty(const QString &key, QObject *parent)
    : keyName(key),
      keyValue(QVariant())
{
    Q_UNUSED(parent);
}

FakeProperty::~FakeProperty()
{
}

void FakeProperty::setValue(const QVariant &value)
{
    bool changed = false;

    if (keyValue != value)
        changed = true;

    keyValue = value;

    if (changed)
        Q_EMIT valueChanged();
}

QVariant FakeProperty::value()
{
    return keyValue;
}

