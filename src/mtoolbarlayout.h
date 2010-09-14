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



#ifndef MTOOLBARLAYOUT_H
#define MTOOLBARLAYOUT_H

//#include <QObject>
#include <QList>
#include <QSharedPointer>

#include "mtoolbarrow.h"

class MToolbarLayoutPrivate;

/*!
 * \brief MToolbarLayout defines toolbar layout for specified orientation
 */
class MToolbarLayout {
public:
    //! Creates empty layout object for given \a orientation
    explicit MToolbarLayout(M::Orientation orientation = M::Landscape);

    //! Destructor.
    virtual ~MToolbarLayout();

    //! Append row to layout
    void append(QSharedPointer<MToolbarRow> row);

    //! Return all items contained by this row
    QList<QSharedPointer<const MToolbarRow> > rows() const;

    //! Return all items contained by this row
    QList<QSharedPointer<MToolbarRow> > rows();

    //! Return layout's orientation
    M::Orientation orientation() const;

private:
    Q_DISABLE_COPY(MToolbarLayout)
    Q_DECLARE_PRIVATE(MToolbarLayout)

    MToolbarLayoutPrivate *const d_ptr;
};

#endif

