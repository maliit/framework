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

#include "mimremotewindow.h"

#include <QDebug>
#include <QX11Info>

#include "mimapplication.h"
#include "mimxerrortrap.h"

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xcomposite.h>
#include <X11/extensions/Xdamage.h>

MImRemoteWindow::MImRemoteWindow(WId window, QObject *parent) :
    QObject(parent),
    wid(window),
    xpixmap(0),
    damage(0),
    pixmap(),
    redirected(false)
{
}

MImRemoteWindow::~MImRemoteWindow()
{
    XSelectInput(QX11Info::display(), wid, 0);
    unredirect();
}

void MImRemoteWindow::setIMWidget(const QWidget *widget)
{
    XSetTransientForHint(QX11Info::display(),
                         widget->effectiveWinId(),
                         wid);

    // Using PropertyChangeMask is a work-around for NB#172722 (a WONTFIX):
    XSelectInput(QX11Info::display(),
                 wid,
                 StructureNotifyMask | PropertyChangeMask);
}

bool MImRemoteWindow::wasIconified(XEvent *ev) const
{
    if (PropertyNotify != ev->type) {
        return false;
    }

    static const Atom wmState = XInternAtom(QX11Info::display(), "WM_STATE", false);

    if (ev->xproperty.atom == wmState) {
        return isIconified();
    }

    return false;
}

bool MImRemoteWindow::isIconified() const
{
    static const Atom wmState = XInternAtom(QX11Info::display(), "WM_STATE", false);

    Atom type;
    int format;
    unsigned long length;
    unsigned long after;
    unsigned long *state;
    uchar *data = 0;

    int queryResult = XGetWindowProperty(QX11Info::display(), wid, wmState, 0, 2,
                                         false, AnyPropertyType, &type, &format, &length,
                                         &after, &data);
    state = reinterpret_cast<unsigned long *>(data);

    bool result = (queryResult == Success && data && format == 32 && *state == IconicState);

    if (data) {
        XFree(data);
    }

    return result;
}


bool MImRemoteWindow::wasUnmapped(XEvent *ev) const
{
    return (ev->type == UnmapNotify &&
            ev->xunmap.event == wid);
}

void MImRemoteWindow::setupDamage()
{
    destroyDamage();

    damage = XDamageCreate(QX11Info::display(), wid, XDamageReportNonEmpty);
}

void MImRemoteWindow::destroyDamage()
{
    if (damage == 0)
        return;

    XDamageDestroy(QX11Info::display(), damage);
    damage = 0;
}

void MImRemoteWindow::redirect()
{
    if (redirected)
        return;

    if (MIMApplication::instance()->manualRedirection()) {
        MImXErrorTrap xerror(MIMApplication::instance()->compositeExtension(), X_CompositeRedirectWindow);
        XCompositeRedirectWindow(QX11Info::display(),
                                 wid,
                                 CompositeRedirectManual);
        if (xerror.untrap() == BadAccess)
            qDebug() << "Window " << wid << " was already redirected";
    }

    redirected = true;

    setupPixmap();
    setupDamage();

    Q_EMIT contentUpdated(QRegion(QRect(QPoint(), pixmap.size())));
}

void MImRemoteWindow::unredirect()
{
    if (!redirected)
        return;

    redirected = false;

    destroyDamage();
    destroyPixmap();

    if (MIMApplication::instance()->manualRedirection()) {
        MImXErrorTrap xerror(MIMApplication::instance()->compositeExtension(), X_CompositeUnredirectWindow);
        XCompositeUnredirectWindow(QX11Info::display(),
                                   wid,
                                   CompositeRedirectManual);
        if (xerror.untrap() == BadAccess)
            qDebug() << "Window " << wid << " was not redirected";
    }
}

void MImRemoteWindow::handleEvent(XEvent *event)
{
    handleConfigureNotifyEvent(event);
    handleDamageEvent(event);
}

void MImRemoteWindow::handleConfigureNotifyEvent(XEvent *event)
{
    if (event->type != ConfigureNotify)
        return;

    XConfigureEvent *e = &event->xconfigure;

    if (e->window != wid)
        return;

    resetPixmap();
}

void MImRemoteWindow::resetPixmap()
{
    if (redirected)
        setupPixmap();
}

void MImRemoteWindow::update(const QRegion &region)
{
    Q_EMIT contentUpdated(region);
}

void MImRemoteWindow::handleDamageEvent(XEvent *event)
{
    if (event->type != MIMApplication::instance()->damageExtension().eventBase() + XDamageNotify)
        return;

    XDamageNotifyEvent *e = reinterpret_cast<XDamageNotifyEvent*>(event);

    if (damage != e->damage)
        return;

    XserverRegion parts = XFixesCreateRegion(QX11Info::display(), 0, 0);
    XDamageSubtract(QX11Info::display(), e->damage, None, parts);

    QRegion region;

    int nrects;
    XRectangle *rects = XFixesFetchRegion(QX11Info::display(), parts, &nrects);
    if (rects) {
        for (int i = 0; i < nrects; ++i) {
            region += QRect(rects[i].x, rects[i].y, rects[i].width, rects[i].height);
        }
    }
    free(rects);

    XFixesDestroyRegion(QX11Info::display(), parts);

    // setup remote pixmap when it failed before
    if (pixmap.isNull())
        setupPixmap();

    Q_EMIT contentUpdated(region);
}

void MImRemoteWindow::setupPixmap()
{
    destroyPixmap();

    MImXErrorTrap error(MIMApplication::instance()->compositeExtension(), X_CompositeNameWindowPixmap);
    xpixmap = XCompositeNameWindowPixmap(QX11Info::display(), wid);
    if (error.untrap() == BadMatch) {
        qDebug() << "Cannot get offscreen reference for Window " << wid;
        xpixmap = 0;
        return;
    }

    if (xpixmap != 0) {
        pixmap = QPixmap::fromX11Pixmap(xpixmap, QPixmap::ExplicitlyShared);
    }
}

void MImRemoteWindow::destroyPixmap()
{
    if (mApp)
        pixmap = QPixmap();

    if (xpixmap != 0) {
        XFreePixmap(QX11Info::display(), xpixmap);
        xpixmap = 0;
    }
}

const QPixmap &MImRemoteWindow::windowPixmap() const
{
    // setup remote pixmap when it failed before
    if (redirected && pixmap.isNull()) {
        const_cast<MImRemoteWindow*>(this)->setupPixmap();
    }

    return pixmap;
}
