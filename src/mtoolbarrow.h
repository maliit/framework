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



#ifndef MTOOLBARROW_H
#define MTOOLBARROW_H

//#include <QObject>

//#include "minputmethodnamespace.h"
#include "mtoolbaritem.h"

class MToolbarRowPrivate;

/*!
 * \brief MToolbarRow defines content of one row in toolbar
 */
class MToolbarRow {
public:
    //! Constructor.
    MToolbarRow();

    //! Destructor.
    virtual ~MToolbarRow();

    //! Return all items contained by this row
    QList<QSharedPointer<MToolbarItem> > items() const;

    //! Sort items according to their priorities and alignments
    void sort();

private:
    Q_DISABLE_COPY(MToolbarRow)
    Q_DECLARE_PRIVATE(MToolbarRow)

    //! Append item to row
    void append(const QSharedPointer<MToolbarItem> &item);

    MToolbarRowPrivate *const d_ptr;

    friend class MToolbarData;
};

#endif

