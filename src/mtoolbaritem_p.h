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



#ifndef MTOOLBARITEM_P_H
#define MTOOLBARITEM_P_H

#include <QSharedPointer>

class MToolbarItemAction;

class MToolbarItemActionPrivate {
public:
    MToolbarItemActionPrivate();

    MInputMethod::ActionType type;
    QString keys;
    QString text;
    QString command;
    QString group;
};

class MToolbarItemPrivate {
public:
    MToolbarItemPrivate();

    void assign(const MToolbarItemPrivate &other);


    //! Item type, e. g. button
    MInputMethod::ItemType type;
    //! The NAME attribute should be unique and it is used as a reference in the toolbar system.
    QString name;
    //! The group name which the button belongs to
    QString group;
    int priority;
    MInputMethod::VisibleType showOn;
    MInputMethod::VisibleType hideOn;
    Qt::Alignment alignment;
    QString text;
    QString textId;
    bool visible;

    // below attributes are only valid for Button
    bool toggle;
    bool pressed;
    QString icon;
    int size;
    QString iconId;

    //! actions when clicking the widget
    QList<QSharedPointer<MToolbarItemAction> > actions;
};

#endif

