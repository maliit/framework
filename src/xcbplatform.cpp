/* * This file is part of Maliit framework *
 *
 * Copyright (C) 2013 Openismus GmbH
 *
 * Contact: maliit-discuss@lists.maliit.org
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 */

#include <xcb/xcb.h>
#include <xcb/xfixes.h>

#include <QGuiApplication>
#include <QRegion>
#include <QVector>
#include <QWindow>
#include <qpa/qplatformnativeinterface.h>

#include "xcbplatform.h"

namespace Maliit
{

void XCBPlatform::setupInputPanel(QWindow* window,
                                  Maliit::Position position)
{
    Q_UNUSED(window);
    Q_UNUSED(position);
    // nothing to do.
}

void XCBPlatform::setInputRegion(QWindow* window,
                                 const QRegion& region)
{
    if (not window) {
        return;
    }

    QVector<xcb_rectangle_t> xcbrects;
    const QVector<QRect> rects(region.rects());

    xcbrects.reserve(rects.size());
    Q_FOREACH (const QRect &rect, rects) {
        xcb_rectangle_t xcbrect;

        xcbrect.x = rect.x();
        xcbrect.y = rect.y();
        xcbrect.width = rect.width();
        xcbrect.height = rect.height();
        xcbrects.append (xcbrect);
    }

    QPlatformNativeInterface *xcbiface = QGuiApplication::platformNativeInterface();
    xcb_connection_t *xcbconnection = static_cast<xcb_connection_t *>(xcbiface->nativeResourceForWindow("connection", window));
    xcb_xfixes_region_t xcbregion = xcb_generate_id(xcbconnection);
    xcb_xfixes_create_region(xcbconnection, xcbregion,
                             xcbrects.size(), xcbrects.constData());

    xcb_window_t xcbwindow  = window->winId();
    xcb_xfixes_set_window_shape_region(xcbconnection, xcbwindow,
                                       XCB_SHAPE_SK_BOUNDING, 0, 0, 0);
    xcb_xfixes_set_window_shape_region(xcbconnection, xcbwindow,
                                       XCB_SHAPE_SK_INPUT, 0, 0, xcbregion);

    xcb_xfixes_destroy_region(xcbconnection, xcbregion);
}

} // namespace Maliit
