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

#include "duiinputcontextconnection.h"

#include "duiinputmethodbase.h"
#include <QSet>

class DuiInputContextConnectionPrivate
{
public:
    DuiInputContextConnectionPrivate();
    ~DuiInputContextConnectionPrivate();

    QSet<DuiInputMethodBase *> targets; // not owned by us
};


DuiInputContextConnectionPrivate::DuiInputContextConnectionPrivate()
{
    // nothing
}


DuiInputContextConnectionPrivate::~DuiInputContextConnectionPrivate()
{
    // nothing
}


////////////////////////
// actual class

DuiInputContextConnection::DuiInputContextConnection()
    : d(new DuiInputContextConnectionPrivate)
{
    // nothing
}


DuiInputContextConnection::~DuiInputContextConnection()
{
    delete d;
}

void DuiInputContextConnection::addTarget(DuiInputMethodBase *target)
{
    d->targets.insert(target);
}

void DuiInputContextConnection::removeTarget(DuiInputMethodBase *target)
{
    d->targets.remove(target);
}

void DuiInputContextConnection::updateInputMethodArea(const QRegion &region)
{
    Q_UNUSED(region);
}

QSet<DuiInputMethodBase *> DuiInputContextConnection::targets()
{
    return d->targets;
}

