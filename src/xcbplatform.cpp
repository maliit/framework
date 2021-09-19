/* * This file is part of Maliit framework *
 *
 * Copyright (C) 2013 Openismus GmbH
 *
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 */

#include <xcb/xcb.h>
#include <xcb/xfixes.h>

#include <QDebug>
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
    Q_UNUSED(position);

    if (not window) {
        return;
    }

    // set window type as input, supported by at least mcompositor
    QPlatformNativeInterface *xcbiface = QGuiApplication::platformNativeInterface();
    xcb_connection_t *xcbConnection
            = static_cast<xcb_connection_t *>(xcbiface->nativeResourceForWindow("connection", window));
    if (!xcbConnection) {
        qWarning("Unable to get Xcb connection");
        return;
    }

    const char * windowType = "_NET_WM_WINDOW_TYPE";
    const char * windowTypeInput = "_NET_WM_WINDOW_TYPE_INPUT";

    xcb_intern_atom_cookie_t windowTypeCookie =
            xcb_intern_atom(xcbConnection, false, strlen(windowType), windowType);
    xcb_intern_atom_cookie_t typeInputCookie =
            xcb_intern_atom(xcbConnection, false, strlen(windowTypeInput), windowTypeInput);

    xcb_atom_t windowTypeAtom;
    xcb_atom_t windowTypeInputAtom;

    xcb_intern_atom_reply_t *reply = xcb_intern_atom_reply(xcbConnection, windowTypeCookie, 0);
    if (reply) {
        windowTypeAtom = reply->atom;
        free(reply);
    } else {
        qWarning("Unable to fetch window type atom");
        return;
    }

    reply = xcb_intern_atom_reply(xcbConnection, typeInputCookie, 0);
    if (reply) {
        windowTypeInputAtom = reply->atom;
        free(reply);
    } else {
        qWarning("Unable to fetch window type input atom");
        return;
    }

    xcb_change_property(xcbConnection, XCB_PROP_MODE_REPLACE, window->winId(), windowTypeAtom, XCB_ATOM_ATOM,
                        32, 1, &windowTypeInputAtom);
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

void XCBPlatform::setApplicationWindow(QWindow *window, WId appWindowId)
{
    qDebug() << "Xcb platform setting transient target" << QString("0x%1").arg(QString::number(appWindowId, 16))
             << "for" << QString("0x%1").arg(QString::number(window->winId(), 16));

    QPlatformNativeInterface *xcbiface = QGuiApplication::platformNativeInterface();
    xcb_connection_t *xcbConnection
            = static_cast<xcb_connection_t *>(xcbiface->nativeResourceForWindow("connection", window));

    xcb_change_property(xcbConnection, XCB_PROP_MODE_REPLACE, window->winId(),
                        XCB_ATOM_WM_TRANSIENT_FOR, XCB_ATOM_WINDOW, 32, 1, &appWindowId);
}

} // namespace Maliit
