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


#include "mtoolbarlayout.h"
#include "mtoolbarlayout_p.h"

MToolbarLayout::MToolbarLayout(M::Orientation orientation)
    : d_ptr(new MToolbarLayoutPrivate)
{
    d_ptr->orientation = orientation;
}

MToolbarLayout::~MToolbarLayout()
{
    delete d_ptr;
}

bool MToolbarLayout::append(const QSharedPointer<MToolbarItem> &item)
{
    Q_D(MToolbarLayout);

    if (d->items.contains(item)) {
        return false;
    }

    d->items.append(item);
    return true;
}

QList<QSharedPointer<MToolbarItem> > MToolbarLayout::items() const
{
    Q_D(const MToolbarLayout);

    return d->items;
}

M::Orientation MToolbarLayout::orientation() const
{
    Q_D(const MToolbarLayout);

    return d->orientation;
}

