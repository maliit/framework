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

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h> // defines IconicState

MIMApplication::MIMApplication(int &argc, char **argv)
    : MApplication(argc, argv),
      remoteWinId(0),
      passThruWindow(0)
{}

MIMApplication::~MIMApplication()
{}

bool MIMApplication::x11EventFilter(XEvent *ev)
{
    handleMapNotifyEvents(ev);
    handleTransientEvents(ev);
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
    if (0 == remoteWinId || not passThruWindow) {
        return;
    }

    if (wasRemoteWindowIconified(ev) || wasRemoteWindowUnmapped(ev)) {
        mDebug("MIMApplication") << "Remote window was destroyed or iconified - hiding.";
        remoteWinId = 0;
        emit remoteWindowGone();
    }
}

void MIMApplication::setTransientHint(int newRemoteWinId)
{
    if (0 == newRemoteWinId || not activeWindow()) {
        return;
    }

    remoteWinId = newRemoteWinId;

    // The transient hint is strictly speaken not necessary yet - we still
    // handle window stacking order manually by making the VKB plugin raise
    // over the top-most application window.
    // TODO: Get rid of window stacking code in plugin side, depends on fix
    // for NB#172937.
    XSetTransientForHint(QX11Info::display(), activeWindow()->winId(), remoteWinId);

    // Using PropertyChangeMask is a work-around for NB#172722.
    XSelectInput(QX11Info::display(), newRemoteWinId, StructureNotifyMask | PropertyChangeMask);
}

void MIMApplication::setPassThruWindow(QWidget *newPassThruWindow)
{
    // The MPassThruWindow is different from the activeWindow, but we have to
    // follow the X11 events from the activeWindow while hiding its viewport (an
    // MPassThruWindow instance, usually), in order to not mess with the
    // application logic.
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

        int queryResult = XGetWindowProperty(QX11Info::display(), remoteWinId, wmState, 0, 2,
                                             false, AnyPropertyType, &type, &format, &length,
                                             &after, &data);
        state = (unsigned long *) data;

        if (queryResult == Success && data && format == 32 && *state == IconicState) {
            XFree((char *)data);
            return true;
        }
    }

    return false;
}

bool MIMApplication::wasRemoteWindowUnmapped(XEvent *ev) const
{
    return (UnmapNotify == ev->type &&
            static_cast<int>(ev->xunmap.event) == remoteWinId);
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
