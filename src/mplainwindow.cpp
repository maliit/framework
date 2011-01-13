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

#include "mplainwindow.h"
#include "mimscene.h"

#include <MSceneManager>
#include <MGConfItem>
#include <MTimestamp>
#include <QDebug>

namespace
{
    // This GConf item defines whether multitouch is enabled or disabled
    const char * const MultitouchSettings = "/meegotouch/inputmethods/multitouch/enabled";
}

MPlainWindow *MPlainWindow::m_instance = 0;

MPlainWindow *MPlainWindow::instance()
{
    return m_instance;
}

MPlainWindow::MPlainWindow(QWidget *parent) :
    MWindow(new MSceneManager(new MImScene), parent)
{
    if (m_instance)
        qFatal("There can be only one instance of MPlainWindow");

    m_instance = this;

    // This *does not* prevent plugins from activating multitouch through
    // QGraphicsItem::setAcceptTouchEvents, as the first (enabling) call to
    // that method *will* set the WA_AcceptTouchEvents attribute on all
    // attached viewports (this was probably done in Qt to add some
    // convenience for sloppy programmers).
    //
    // Setting this attribute explicitely here is supposed to guard against
    // changes in above mentioned (undocumented!) "convenience", as this is
    // what the documentation suggests [1].
    //
    // [1] http://doc.trolltech.com/4.6/qtouchevent.html#enabling-touch-events
    if (MGConfItem(MultitouchSettings).value().toBool()) {
        setAttribute(Qt::WA_AcceptTouchEvents);
    }

    ungrabGesture(Qt::TapAndHoldGesture);
    ungrabGesture(Qt::PinchGesture);
    ungrabGesture(Qt::PanGesture);
    ungrabGesture(Qt::SwipeGesture);
    ungrabGesture(Qt::TapGesture);
}

MPlainWindow::~MPlainWindow()
{
    m_instance = 0;
    delete scene();
    delete sceneManager();
}

#if defined(M_IM_DISABLE_TRANSLUCENCY) && !defined(M_IM_USE_SHAPE_WINDOW)
void MPlainWindow::updatePosition(const QRegion &region)
{
    //update view's parameters to compensate movement of MPasstroughWindow
    //this update allows plugin to use screen coordinates
    const QRect rect = region.boundingRect();
    const QSize sceneSize = visibleSceneSize();
    const int size = region.rects().size();

    if (!size) {
        setSceneRect(0, 0, sceneSize.width(), sceneSize.height());
        resize(sceneSize);
        move(0, 0);
        return;
    }

    switch (orientationAngle())
    {
    case M::Angle0:
        setSceneRect(0, sceneSize.height() - rect.height(),
                     sceneSize.width(), rect.height());
        resize(rect.width(), rect.height());
        move(0, 0);
        break;
    case M::Angle90:
        setSceneRect(0, 0, rect.width(), rect.height());
        resize(rect.width(), rect.height());
        move(sceneSize.height() - rect.width(), 0);
        break;
    case M::Angle180:
        setSceneRect(0, 0, rect.width(), sceneSize.height());
        resize(rect.width(), rect.height());
        move(0, sceneSize.height() - rect.height());
        break;
    case M::Angle270:
        setSceneRect(sceneSize.height() - rect.width(), rect.top(),
                     rect.width(), sceneSize.width());
        resize(rect.width(), rect.height());
        move(0, 0);
        break;
    }
}
#endif

bool MPlainWindow::viewportEvent(QEvent *event)
{
#ifdef M_TIMESTAMP
    QString start;
    QString end;
    start = QString("%1_start").arg(event->type());
    end = QString("%1_end").arg(event->type());

    mTimestamp("MPlainWindow", start);
#endif

    bool result = MWindow::viewportEvent(event);

#ifdef M_TIMESTAMP
    mTimestamp("MPlainWindow", end);
#endif
    return result;
}

