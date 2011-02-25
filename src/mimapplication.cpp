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

MIMApplication::MIMApplication(int &argc, char **argv)
    : MApplication(argc, argv),
      mPassThruWindow(0),
      mRemoteWindow(0),
      mCompositeExtension(),
      mDamageExtension(),
      mSelfComposited(false),
      mManualRedirection(false),
      mBypassWMHint(false)
{
    parseArguments(argc, argv);
}

MIMApplication::~MIMApplication()
{
}

void MIMApplication::parseArguments(int &argc, char** argv)
{
    for (int i = 1; i < argc; i++) {
        QLatin1String arg(argv[i]);

        if (arg == "-manual-redirection") {
            mManualRedirection = true;
        } else if (arg == "-bypass-wm-hint") {
            mBypassWMHint = true;
        } else if (arg == "-use-self-composition") {
            mSelfComposited = mCompositeExtension.supported(0, 2) && mDamageExtension.supported();
        }
    }
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
    if (0 == mRemoteWindow || not mPassThruWindow) {
        return;
    }

    if (mRemoteWindow->wasIconified(ev) || mRemoteWindow->wasUnmapped(ev)) {
        mDebug("MIMApplication") << "Remote window was destroyed or iconified - hiding.";
        emit remoteWindowGone();
        delete mRemoteWindow;
        mRemoteWindow = 0;
    }
}

void MIMApplication::setTransientHint(WId newRemoteWinId)
{
    if (0 == newRemoteWinId || not activeWindow()) {
        return;
    }

    if (mRemoteWindow && mRemoteWindow->id() == newRemoteWinId) {
        return;
    }

    MImRemoteWindow *oldWindow = mRemoteWindow;

    mRemoteWindow = new MImRemoteWindow(newRemoteWinId, this);
    mRemoteWindow->setIMWidget(mPassThruWindow->window());
    emit remoteWindowChanged(mRemoteWindow);

    delete oldWindow;
}

void MIMApplication::setPassThruWindow(QWidget *newPassThruWindow)
{
    if (newPassThruWindow != mPassThruWindow) {
        mPassThruWindow = newPassThruWindow;
    }
}

MIMApplication *MIMApplication::instance()
{
    return static_cast<MIMApplication *>(QCoreApplication::instance());
}

bool MIMApplication::wasPassThruWindowMapped(XEvent *ev) const
{
    return (mPassThruWindow &&
            MapNotify == ev->type &&
            static_cast<WId>(ev->xmap.event) == mPassThruWindow->effectiveWinId());
}

bool MIMApplication::wasPassThruWindowUnmapped(XEvent *ev) const
{
    return (mPassThruWindow &&
            UnmapNotify == ev->type &&
            static_cast<WId>(ev->xunmap.event) == mPassThruWindow->effectiveWinId());
}

void MIMApplication::handleDamageEvents(XEvent *event)
{
    if (mRemoteWindow == 0)
        return;

    mRemoteWindow->handleDamageEvent(event);
}

bool MIMApplication::selfComposited() const
{
    return mSelfComposited;
}

bool MIMApplication::manualRedirection() const
{
    return mManualRedirection;
}

bool MIMApplication::bypassWMHint() const
{
    return mBypassWMHint;
}
