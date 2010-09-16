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


#include "mtoolbarrow.h"
#include "mtoolbarrow_p.h"

namespace
{
    bool lessThanItem(const QSharedPointer<const MToolbarItem> &left,
                      const QSharedPointer<const MToolbarItem> &right)
    {
        if (left->alignment() != right->alignment()) {
            return left->alignment() < right->alignment();
        }

        return left->priority() < right->priority();
    }
}

MToolbarRow::MToolbarRow()
    : d_ptr(new MToolbarRowPrivate)
{
}

MToolbarRow::~MToolbarRow()
{
    delete d_ptr;
}

QList<QSharedPointer<MToolbarItem> > MToolbarRow::items() const
{
    Q_D(const MToolbarRow);

    return d->items;
}

void MToolbarRow::sort()
{
    Q_D(MToolbarRow);

    qSort(d->items.begin(), d->items.end(), &lessThanItem);
}

void MToolbarRow::append(const QSharedPointer<MToolbarItem> &item)
{
    Q_D(MToolbarRow);

    d->items.append(item);
}

