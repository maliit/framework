/* * This file is part of dui-im-framework *
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
#include "duipassthruwindow.h"

#include <QX11Info>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xfixes.h>
#include <X11/extensions/shape.h>

DuiPassThruWindow::DuiPassThruWindow(bool bypassWMHint, QWidget *p)
    : QWidget(p)
{
    setWindowTitle("DuiInputMethod");
    setAttribute(Qt::WA_TranslucentBackground);

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

DuiPassThruWindow::~DuiPassThruWindow()
{
}

void DuiPassThruWindow::inputPassthrough(const QRegion &region)
{
    Display *dpy = QX11Info::display();

    qDebug() << __PRETTY_FUNCTION__ << region;
    const QVector<QRect> regionRects(region.rects());
    const int size = regionRects.size();

    if (size) {
        //we should receive correct pointer even if region is empty
        XRectangle *rects = (XRectangle *) malloc(sizeof(XRectangle) * (size + 1));
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
    }

    // selective compositing
    if (isVisible() && region.isEmpty()) {
        hide();
    } else if (!isVisible() && !region.isEmpty()) {
        show();
    }
}
