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

#include "mimremotewindow.h"

#include <X11/Xlib.h>

MIMApplication::MIMApplication(int &argc, char **argv, bool useSelfComposite)
    : MApplication(argc, argv),
      passThruWindow(0),
      remoteWindow(0),
      composite_extension(),
      damage_extension(),
      self_composited(useSelfComposite && composite_extension.supported(0, 2) && damage_extension.supported())
{
}

MIMApplication::~MIMApplication()
{
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
    if (0 == remoteWindow || not passThruWindow) {
        return;
    }

    if (remoteWindow->wasIconified(ev) || remoteWindow->wasUnmapped(ev)) {
        mDebug("MIMApplication") << "Remote window was destroyed or iconified - hiding.";
        emit remoteWindowGone();
        delete remoteWindow;
        remoteWindow = 0;
    }
}

void MIMApplication::setTransientHint(WId newRemoteWinId)
{
    if (0 == newRemoteWinId || not activeWindow()) {
        return;
    }

    if (remoteWindow && remoteWindow->id() == newRemoteWinId) {
        return;
    }

    MImRemoteWindow *oldWindow = remoteWindow;

    remoteWindow = new MImRemoteWindow(newRemoteWinId, this);
    remoteWindow->setIMWidget(passThruWindow->window());
    emit remoteWindowChanged(remoteWindow);

    delete oldWindow;
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

void MIMApplication::handleDamageEvents(XEvent *event)
{
    if (remoteWindow == 0)
        return;

    remoteWindow->handleDamageEvent(event);
}

bool MIMApplication::selfComposited() const
{
    return self_composited;
}
