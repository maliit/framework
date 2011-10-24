/* * This file is part of meego-im-framework *
 *
 * Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#include "mimgraphicsview_p.h"
#include "mimgraphicsview.h"

#if defined(Q_WS_X11)
#include "mimxapplication.h"
#endif

#include <QDebug>

MImGraphicsViewPrivate::MImGraphicsViewPrivate()
    : q_ptr(0)
{}

MImGraphicsViewPrivate::~MImGraphicsViewPrivate()
{}

MImGraphicsView::MImGraphicsView(QWidget *parent)
    : QGraphicsView(parent)
    , d_ptr(new MImGraphicsViewPrivate)
{
    init();
}

MImGraphicsView::MImGraphicsView(QGraphicsScene *scene,
                                 QWidget *parent)
    : QGraphicsView(scene, parent)
    , d_ptr(new MImGraphicsViewPrivate)
{
    init();
}

MImGraphicsView::MImGraphicsView(MImGraphicsViewPrivate *dd,
                                 QGraphicsScene *scene,
                                 QWidget *parent)
    : QGraphicsView(scene, parent)
    , d_ptr(dd)
{
    init();
}

MImGraphicsView::~MImGraphicsView()
{
    delete d_ptr;
}

void MImGraphicsView::init()
{
    Q_D(MImGraphicsView);
    d->q_ptr = this;
    setAttribute(Qt::WA_OpaquePaintEvent);
    setAttribute(Qt::WA_NoSystemBackground);

    if (viewport()) {
        viewport()->setAttribute(Qt::WA_OpaquePaintEvent);
        viewport()->setAttribute(Qt::WA_NoSystemBackground);
    } else {
        qWarning() << __PRETTY_FUNCTION__
                   << "Could not find viewport - unable to set window attributes!";
    }
}

void MImGraphicsView::drawBackground(QPainter *painter,
                                     const QRectF &rect)
{
    if (rect.isEmpty()) {
        return;
    }

#if defined(Q_WS_X11)
    const QPixmap &bg(MImXApplication::instance()->remoteWindowPixmap());
    if (not bg.isNull()) {
        painter->drawPixmap(rect.toRect(), bg, rect.toRect());
    }
#else
    return;
#endif
}
