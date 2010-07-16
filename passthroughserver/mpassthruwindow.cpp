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
#include "mpassthruwindow.h"
#include "mplainwindow.h"
#include "mimapplication.h"

#include <QX11Info>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>

#ifndef M_IM_DISABLE_TRANSLUCENCY
#include <X11/extensions/Xfixes.h>
#include <X11/extensions/shape.h>
#endif

namespace {
    const int ShowRetryLimit = 100;
    const int WaitForMapNotifyTimeout = 100; // in ms
}

MPassThruWindow::MPassThruWindow(bool bypassWMHint, QWidget *p)
    : QWidget(p)
    , showRetryCount(0)
{
    waitForMapNotify.setSingleShot(false);
    waitForMapNotify.setInterval(WaitForMapNotifyTimeout);

    // We'll keep sending show requests until we got a map notify event from X.
    // See NB#173434
    connect(&waitForMapNotify, SIGNAL(timeout()),
            this,              SLOT(showRequest()));

    connect(MIMApplication::instance(), SIGNAL(passThruWindowMapped()),
            this,                       SLOT(cancelShowRequest()));

    setWindowTitle("MInputMethod");
#ifndef M_IM_DISABLE_TRANSLUCENCY
    setAttribute(Qt::WA_TranslucentBackground);
#endif

    Display *dpy =  QX11Info::display();

    Qt::WindowFlags windowFlags = Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint;

    if (bypassWMHint) {
        windowFlags |= Qt::X11BypassWindowManagerHint;
    }

    setWindowFlags(windowFlags);

    // Hint to the window manager that we don't want input focus
    // for the inputmethod window
    XWMHints wmhints;
    wmhints.flags = InputHint;
    wmhints.input = False;
    XSetWMHints(dpy, winId(), &wmhints);

    Atom input = XInternAtom(dpy, "_NET_WM_WINDOW_TYPE_INPUT", False);
    XChangeProperty(dpy, winId(), XInternAtom(dpy, "_NET_WM_WINDOW_TYPE", False), XA_ATOM, 32,
                    PropModeReplace, (unsigned char *) &input, 1);
}

MPassThruWindow::~MPassThruWindow()
{
}

void MPassThruWindow::showRequest()
{
    waitForMapNotify.start();
    qDebug() << __PRETTY_FUNCTION__
             << "trying to show window, count = " << showRetryCount;

    if (showRetryCount == 0) {
        show();
    } else if (showRetryCount < ShowRetryLimit) {
        // Qt refuses to show us if we are already shown, hence we do
        // it ourself:
        XMapWindow(QX11Info::display(), effectiveWinId());
        XFlush(QX11Info::display());
    } else {
        // give up ...
        cancelShowRequest();
    }

    ++showRetryCount;
}

void MPassThruWindow::cancelShowRequest()
{
    qDebug() << __PRETTY_FUNCTION__
             << "window got finally mapped, count = " << showRetryCount;
    showRetryCount = 0;
    waitForMapNotify.stop();
}

void MPassThruWindow::inputPassthrough(const QRegion &region)
{
#ifndef M_IM_DISABLE_TRANSLUCENCY
    Display *dpy = QX11Info::display();
#endif

    qDebug() << __PRETTY_FUNCTION__ << region
        << "geometry=" << geometry();
    QVector<QRect> regionRects(region.rects());
    const int size = regionRects.size();

    if (size) {
#ifdef M_IM_DISABLE_TRANSLUCENCY
#ifdef M_IM_USE_SHAPE_WINDOW
        setMask(region);
#else
        QPoint newPos(0, 0);

        switch (MPlainWindow::instance()->orientationAngle())
        {
        case M::Angle0:
            newPos.setY(region.boundingRect().top());
            break;
        case M::Angle90:
            newPos.setX(region.boundingRect().width() - width());
            break;
        case M::Angle180:
            newPos.setY(region.boundingRect().height() - height());
            break;
        case M::Angle270:
            newPos.setX(width() - region.boundingRect().width());
            break;
        default:
            Q_ASSERT(0);
        }
        move(newPos);
        newPos.setX(-newPos.x());
        newPos.setY(-newPos.y());
#endif
#else

        XRectangle * const rects = (XRectangle*)malloc(sizeof(XRectangle)*(size));
        if (!rects) {
            return;
        }

        XRectangle *rect = rects;
        for (int i = 0; i < size; ++i, ++rect) {
            rect->x = regionRects.at(i).x();
            rect->y = regionRects.at(i).y();
            rect->width = regionRects.at(i).width();
            rect->height = regionRects.at(i).height();
        }

        const XserverRegion shapeRegion = XFixesCreateRegion(dpy, rects, size);
        XFixesSetWindowShapeRegion(dpy, winId(), ShapeBounding, 0, 0, 0);
        XFixesSetWindowShapeRegion(dpy, winId(), ShapeInput, 0, 0, shapeRegion);

        XFixesDestroyRegion(dpy, shapeRegion);

        free(rects);
        XSync(dpy, False);
#endif
    }

    // selective compositing
    if (isVisible() && region.isEmpty()) {
        // TODO: fix hiding of VKB - it is the same problem as with show()
        hide();
    } else if (!isVisible() && !region.isEmpty()) {
        showRequest();
    }
}

