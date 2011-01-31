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

#include "mimapplication.h"

#include <MWindow>
#include <MDebug>

#include "x11wrapper.h"

X11Wrapper::X11Wrapper() 
    : remoteWindowId(0),
      remoteWindowXPixmap(0)
{
}

MIMApplication::MIMApplication(int &argc, char **argv, bool useSelfComposite)
    : MApplication(argc, argv),
      passThruWindow(0),
      x11Wrapper(new X11Wrapper),
      selfComposited(false)
{
    if (useSelfComposite)
        selfComposited = initializeComposite();
}

MIMApplication::~MIMApplication()
{
    delete x11Wrapper;
}

bool MIMApplication::initializeComposite()
{
    int compositeBase, compositeError;
    if (!XCompositeQueryExtension(QX11Info::display(), &compositeBase, &compositeError))
        return false;

    int major = 0, minor = 2; // XComposite 0.2 required for XCompositeNameWindowPixmap
    if (!XCompositeQueryVersion(QX11Info::display(), &major, &minor))
        return false;

    if (major < 0 || (major == 0 && minor < 2))
        return false;

    int damageError;
    if (!XDamageQueryExtension(QX11Info::display(), &damageBase, &damageError))
        return false;

    return true;
}

bool MIMApplication::x11EventFilter(XEvent *ev)
{
    handleMapNotifyEvents(ev);
    handleTransientEvents(ev);
    handleDamageEvents(ev);
    return MApplication::x11EventFilter(ev);
}

void MIMApplication::handleMapNotifyEvents(XEvent *ev)
{
    if (wasPassThruWindowMapped(ev)) {
        mDebug("MIMApplication") << "PassThru window was mapped.";
        emit passThruWindowMapped();
    } else if (wasPassThruWindowUnmapped(ev)) {
        mDebug("MIMApplication") << "PassThru window was unmapped.";
        emit passThruWindowUnmapped();
    }
}

void MIMApplication::handleTransientEvents(XEvent *ev)
{
    if (0 == x11Wrapper->remoteWindowId || not passThruWindow) {
        return;
    }

    if (wasRemoteWindowIconified(ev) || wasRemoteWindowUnmapped(ev)) {
        mDebug("MIMApplication") << "Remote window was destroyed or iconified - hiding.";
        destroyDamage();
        x11Wrapper->remoteWindowId = 0;
        emit remoteWindowGone();
    }
}

void MIMApplication::setTransientHint(int newRemoteWinId)
{
    if (0 == newRemoteWinId || not activeWindow()) {
        return;
    }

    x11Wrapper->remoteWindowId = newRemoteWinId;

    XSetTransientForHint(QX11Info::display(),
                         passThruWindow->effectiveWinId(),
                         x11Wrapper->remoteWindowId);

    // Using PropertyChangeMask is a work-around for NB#172722 (a WONTFIX):
    XSelectInput(QX11Info::display(),
                 x11Wrapper->remoteWindowId,
                 StructureNotifyMask | PropertyChangeMask);
}

void MIMApplication::setPassThruWindow(QWidget *newPassThruWindow)
{
    if (newPassThruWindow != passThruWindow) {
        passThruWindow = newPassThruWindow;
    }
}

MIMApplication *MIMApplication::instance()
{
    return static_cast<MIMApplication *>(QCoreApplication::instance());
}

bool MIMApplication::wasRemoteWindowIconified(XEvent *ev) const
{
    if (PropertyNotify != ev->type) {
        return false;
    }

    static const Atom wmState = XInternAtom(QX11Info::display(), "WM_STATE", false);

    if (ev->xproperty.atom == wmState) {
        Atom type;
        int format;
        unsigned long length;
        unsigned long after;
        unsigned long *state;
        uchar *data = 0;

        int queryResult = XGetWindowProperty(QX11Info::display(), x11Wrapper->remoteWindowId, wmState, 0, 2,
                                             false, AnyPropertyType, &type, &format, &length,
                                             &after, &data);
        state = (unsigned long *) data;

        bool result = (queryResult == Success && data && format == 32 && *state == IconicState);

        if (data) {
            XFree(data);
        }

        return result;
    }

    return false;
}

bool MIMApplication::wasRemoteWindowUnmapped(XEvent *ev) const
{
    return (UnmapNotify == ev->type &&
            ev->xunmap.event == x11Wrapper->remoteWindowId);
}

bool MIMApplication::wasPassThruWindowMapped(XEvent *ev) const
{
    return (passThruWindow &&
            MapNotify == ev->type &&
            static_cast<WId>(ev->xmap.event) == passThruWindow->effectiveWinId());
}

bool MIMApplication::wasPassThruWindowUnmapped(XEvent *ev) const
{
    return (passThruWindow &&
            UnmapNotify == ev->type &&
            static_cast<WId>(ev->xunmap.event) == passThruWindow->effectiveWinId());
}

void MIMApplication::setupDamage()
{
    if (x11Wrapper->remoteWindowId == 0)
        return;

    if (x11Wrapper->pixmapDamage != 0)
        destroyDamage();

    x11Wrapper->pixmapDamage = XDamageCreate(QX11Info::display(), x11Wrapper->remoteWindowId, XDamageReportNonEmpty); 
}

void MIMApplication::destroyDamage()
{
    if (x11Wrapper->pixmapDamage != 0) {
        XDamageDestroy(QX11Info::display(), x11Wrapper->pixmapDamage);
        x11Wrapper->pixmapDamage = 0;
    }
}

void MIMApplication::handleDamageEvents(XEvent *event)
{
    if (x11Wrapper->remoteWindowId == 0 ||
        x11Wrapper->pixmapDamage == 0)
        return;

    if (event->type == damageBase + XDamageNotify) {
        XDamageNotifyEvent *e = reinterpret_cast<XDamageNotifyEvent*>(event);

        if (x11Wrapper->pixmapDamage == e->damage) {
            XserverRegion parts = XFixesCreateRegion(QX11Info::display(), 0, 0);
            XDamageSubtract(QX11Info::display(), e->damage, None, parts);

            QRegion region;

            int nrects;
            XRectangle *rects = XFixesFetchRegion (QX11Info::display(), parts, &nrects);
            if (rects) {
                for (int i = 0; i < nrects; ++i) {
                    region += QRect(rects[i].x, rects[i].y, rects[i].width, rects[i].height);
                }
            }
            free(rects);

            XFixesDestroyRegion(QX11Info::display(), parts);
            emit remoteWindowUpdated(region);
        }
    }
}

void MIMApplication::redirectRemoteWindow()
{
    if (x11Wrapper->remoteWindowId != 0) {
        // TODO: Handle error here
        XCompositeRedirectWindow(QX11Info::display(), 
            x11Wrapper->remoteWindowId, 
            CompositeRedirectManual);

        if (x11Wrapper->remoteWindowXPixmap != 0) {
            XFreePixmap(QX11Info::display(), x11Wrapper->remoteWindowXPixmap);
            x11Wrapper->remoteWindowXPixmap = 0;
        }

        x11Wrapper->remoteWindowXPixmap = XCompositeNameWindowPixmap(QX11Info::display(), x11Wrapper->remoteWindowId);
        XSync(QX11Info::display(), FALSE);
        if (x11Wrapper->remoteWindowXPixmap != 0) {
            x11Wrapper->remoteWindowPixmap = QPixmap::fromX11Pixmap(x11Wrapper->remoteWindowXPixmap, QPixmap::ExplicitlyShared);
        }
        setupDamage();
        emit remoteWindowUpdated(QRegion());
    }
}

void MIMApplication::unredirectRemoteWindow()
{
    if (x11Wrapper->remoteWindowId != 0) {
        XCompositeUnredirectWindow(QX11Info::display(), 
            x11Wrapper->remoteWindowId, 
            CompositeRedirectManual);
        destroyDamage();
    }
}

QPixmap MIMApplication::remoteWindowPixmap() const
{
    return x11Wrapper->remoteWindowPixmap;
}

bool MIMApplication::supportsSelfComposite() const
{
    return selfComposited;
}
