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
#include "mimapplication.h"
#include "mimremotewindow.h"

#include <QDebug>
#include <QX11Info>

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>

#ifndef M_IM_DISABLE_TRANSLUCENCY
#include <X11/extensions/Xfixes.h>
#include <X11/extensions/shape.h>
#endif

MPassThruWindow::MPassThruWindow(bool bypassWMHint, bool selfComposited, QWidget *p)
    : QWidget(p),
      raiseOnShow(bypassWMHint), // if bypassing window hint, also do raise to ensure visibility
      selfComposited(selfComposited),
      remoteWindow(0)
{
    setWindowTitle("MInputMethod");

    if (!selfComposited) {
        setAttribute(Qt::WA_TranslucentBackground);
    }

    Qt::WindowFlags windowFlags = Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint;

    if (bypassWMHint) {
        windowFlags |= Qt::X11BypassWindowManagerHint;
    }

    setWindowFlags(windowFlags);

    // We do not want input focus for that window.
    setAttribute(Qt::WA_X11DoNotAcceptFocus);

    QObject::connect(mApp, SIGNAL(remoteWindowChanged(MImRemoteWindow *)),
                     this, SLOT(setRemoteWindow(MImRemoteWindow *)));
    QObject::connect(mApp, SIGNAL(remoteWindowGone()),
                     this, SLOT(setRemoteWindow()));
}

MPassThruWindow::~MPassThruWindow()
{
}

void MPassThruWindow::inputPassthrough(const QRegion &region)
{
#ifndef M_IM_DISABLE_TRANSLUCENCY
    Display *dpy = QX11Info::display();
#endif

    qDebug() << __PRETTY_FUNCTION__ << "QWidget::effectiveWinId(): " << effectiveWinId();

    // Set _NET_WM_WINDOW_TYPE to _NET_WM_WINDOW_TYPE_INPUT
    static Atom input = XInternAtom(QX11Info::display(), "_NET_WM_WINDOW_TYPE_INPUT", False);
    static Atom window_type = XInternAtom(QX11Info::display(), "_NET_WM_WINDOW_TYPE", False);
    XChangeProperty(QX11Info::display(), effectiveWinId(), window_type, XA_ATOM, 32,
                    PropModeReplace, (unsigned char *) &input, 1);

    qDebug() << __PRETTY_FUNCTION__ << region << "geometry=" << geometry();
    QVector<QRect> regionRects(region.rects());
    const int size = regionRects.size();

    if (size) {
        XRectangle * const rects = (XRectangle*)malloc(sizeof(XRectangle)*(size));
        if (!rects) {
            return;
        }

        quint32 customRegion[size * 4]; // custom region is pack of x, y, w, h
        XRectangle *rect = rects;
        for (int i = 0; i < size; ++i, ++rect) {
            rect->x = regionRects.at(i).x();
            rect->y = regionRects.at(i).y();
            rect->width = regionRects.at(i).width();
            rect->height = regionRects.at(i).height();
            customRegion[i * 4 + 0] = rect->x;
            customRegion[i * 4 + 1] = rect->y;
            customRegion[i * 4 + 2] = rect->width;
            customRegion[i * 4 + 3] = rect->height;
        }

        const XserverRegion shapeRegion = XFixesCreateRegion(dpy, rects, size);
        XFixesSetWindowShapeRegion(dpy, effectiveWinId(), ShapeBounding, 0, 0, 0);
        XFixesSetWindowShapeRegion(dpy, effectiveWinId(), ShapeInput, 0, 0, shapeRegion);

        XFixesDestroyRegion(dpy, shapeRegion);

        XChangeProperty(dpy, effectiveWinId(), XInternAtom(dpy, "_MEEGOTOUCH_CUSTOM_REGION", False), XA_CARDINAL, 32,
                        PropModeReplace, (unsigned char *) customRegion, size * 4);

        free(rects);
        XSync(dpy, False);
    }

    // selective compositing
    if (isVisible() && region.isEmpty()) {
        if (selfComposited && remoteWindow) {
            remoteWindow->unredirect();
        }

        hide();
    } else if (!isVisible() && !region.isEmpty()) {
        if (selfComposited && remoteWindow) {
            remoteWindow->redirect();
        }

        showFullScreen();

        if (raiseOnShow) {
            raise();
        }
    }
}

void MPassThruWindow::setRemoteWindow(MImRemoteWindow *newWindow)
{
    remoteWindow = newWindow;

    if (!newWindow)
        inputPassthrough();
}
