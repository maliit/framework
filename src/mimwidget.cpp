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

#include "mimwidget_p.h"
#include "mimwidget.h"

#if defined(Q_WS_X11)
#include "mimxapplication.h"
#endif

#include <QPainter>
#include <QDebug>

MImWidgetPrivate::MImWidgetPrivate()
    : q_ptr(0)
{}

MImWidgetPrivate::~MImWidgetPrivate()
{}

MImWidget::MImWidget(QWidget *parent,
                     Qt::WindowFlags f)
    : QWidget(parent, f)
    , d_ptr(new MImWidgetPrivate)
{
    init();
}

MImWidget::MImWidget(MImWidgetPrivate *dd,
                     QWidget *parent,
                     Qt::WindowFlags f)

    : QWidget(parent, f)
    , d_ptr(dd)
{
    init();
}

MImWidget::~MImWidget()
{
    delete d_ptr;
}

void MImWidget::init()
{
    Q_D(MImWidget);
    d->q_ptr = this;

    setAttribute(Qt::WA_OpaquePaintEvent);
    setAttribute(Qt::WA_NoSystemBackground);
}

void MImWidget::paintEvent(QPaintEvent *ev)
{
    if (ev->rect().isEmpty()) {
        return;
    }

#if defined(Q_WS_X11)
    const QPixmap &bg(MImXApplication::instance()->serverLogic()->remoteWindowPixmap());
    if (not bg.isNull()) {
        QPainter p(this);
        p.drawPixmap(ev->rect(), bg, ev->rect());
    }
#else
    return;
#endif
}
