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
#include "duiplainwindow.h"

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
#ifndef DUI_IM_DISABLE_TRANSLUCENCY
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
    //TODO: below _DUI_STATUSBAR_OVERLAY could be removed when libdui does not make statusbar
    //to be separate.
    Atom atomDuiStatusBarOverlay = XInternAtom(dpy, "_DUI_STATUSBAR_OVERLAY", False);
    long propertyData = 1;
    XChangeProperty(dpy, winId(),
                    atomDuiStatusBarOverlay, XA_CARDINAL /* type */,
                    32 /* format, in bits */, PropModeReplace,
                    (unsigned char *) &propertyData, 1 /* number of elements */);
}

DuiPassThruWindow::~DuiPassThruWindow()
{
}

void DuiPassThruWindow::inputPassthrough(const QRegion &region)
{
    Display *dpy = QX11Info::display();

    qDebug() << __PRETTY_FUNCTION__ << region
        << "geometry=" << geometry();
    QVector<QRect> regionRects(region.rects());
    const int size = regionRects.size();

    if (size) {
#ifdef DUI_IM_DISABLE_TRANSLUCENCY
        QPoint newPos(0, 0);

        switch (DuiPlainWindow::instance()->orientationAngle())
        {
        case Dui::Angle0:
            newPos.setY(region.boundingRect().top());
            break;
        case Dui::Angle90:
            newPos.setX(region.boundingRect().width() - width());
            break;
        case Dui::Angle180:
            newPos.setY(region.boundingRect().height() - height());
            break;
        case Dui::Angle270:
            newPos.setX(width() - region.boundingRect().width());
            break;
        default:
            Q_ASSERT(0);
        }
        move(newPos);
        newPos.setX(-newPos.x());
        newPos.setY(-newPos.y());
#endif

        XRectangle * const rects = (XRectangle*)malloc(sizeof(XRectangle)*(size));
        if (!rects) {
            return;
        }

        XRectangle *rect = rects;
        for (int i = 0; i < size; ++i, ++rect) {
#ifdef DUI_IM_DISABLE_TRANSLUCENCY
            regionRects[i].translate(newPos);
#endif
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

