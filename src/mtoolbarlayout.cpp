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

void MToolbarLayout::append(QSharedPointer<MToolbarRow> row)
{
    Q_D(MToolbarLayout);

    d->rows.append(row);
}

QList<QSharedPointer<const MToolbarRow> > MToolbarLayout::rows() const
{
    Q_D(const MToolbarLayout);
    QList<QSharedPointer<const MToolbarRow> > result;

    foreach (const QSharedPointer<const MToolbarRow> row, d->rows) {
        result.append(row);
    }

    return result;
}

QList<QSharedPointer<MToolbarRow> > MToolbarLayout::rows()
{
    Q_D(MToolbarLayout);

    return d->rows;
}

M::Orientation MToolbarLayout::orientation() const
{
    Q_D(const MToolbarLayout);

    return d->orientation;
}

