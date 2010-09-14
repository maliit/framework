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



#ifndef MTOOLBARDATA_P_H
#define MTOOLBARDATA_P_H

#include <QSharedPointer>
#include <QList>
#include <QMap>
#include <QStringList>
#include <QString>

#include <mtoolbaritem.h>

class MToolbarLayoutPrivate
{
public:
    //! Rows contained by this layout
    QList<QSharedPointer<MToolbarRow> > rows;

    //! Orientation
    M::Orientation orientation;
};

class MToolbarDataPrivate
{
public:
    //! Construct new instance.
    MToolbarDataPrivate();

    //! Layout for landscape orientation
    QSharedPointer<MToolbarLayout> layoutLandscape;

    //! Layout for portrait orientation
    QSharedPointer<MToolbarLayout> layoutPortrait;

    //! Name of file containing this toolbar
    QString toolbarFileName;

    //! Contains true if toolbar is locked
    bool locked;

    typedef QMap<QString, QSharedPointer<MToolbarItem> > Items;
    Items items;

    bool custom;

    bool visible;

    QStringList refusedNames;
};

#endif

