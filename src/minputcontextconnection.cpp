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

#include "minputcontextconnection.h"

#include "minputmethodbase.h"
#include <QSet>

class MInputContextConnectionPrivate
{
public:
    MInputContextConnectionPrivate();
    ~MInputContextConnectionPrivate();

    QSet<MInputMethodBase *> targets; // not owned by us
};


MInputContextConnectionPrivate::MInputContextConnectionPrivate()
{
    // nothing
}


MInputContextConnectionPrivate::~MInputContextConnectionPrivate()
{
    // nothing
}


////////////////////////
// actual class

MInputContextConnection::MInputContextConnection()
    : d(new MInputContextConnectionPrivate)
{
    // nothing
}


MInputContextConnection::~MInputContextConnection()
{
    delete d;
}

void MInputContextConnection::addTarget(MInputMethodBase *target)
{
    d->targets.insert(target);
}

void MInputContextConnection::removeTarget(MInputMethodBase *target)
{
    d->targets.remove(target);
}

void MInputContextConnection::updateInputMethodArea(const QRegion &region)
{
    Q_UNUSED(region);
}

QSet<MInputMethodBase *> MInputContextConnection::targets()
{
    return d->targets;
}

