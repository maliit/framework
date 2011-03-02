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
#include "mimremotewindow.h"

#include <QDebug>
#include <X11/Xlib.h> // must be last include

MIMApplication::MIMApplication(int &argc, char **argv)
    : QApplication(argc, argv),
      mCompositeExtension(),
      mDamageExtension(),
      mSelfComposited(false),
      mManualRedirection(false),
      mBypassWMHint(false)
{
    parseArguments(argc, argv);
    mPassThruWindow.reset(new MPassThruWindow),

    connect(this, SIGNAL(aboutToQuit()),
            this, SLOT(finalize()),
            Qt::UniqueConnection);
}

void MIMApplication::finalize()
{
    // Cannot destroy QWidgets or QPixmaps during MIMApplication destruction.
    // Hence the finalize handler that runs before the d'tor.
    mPassThruWindow.reset();
    mRemoteWindow.reset();
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
    return QApplication::x11EventFilter(ev);
}

void MIMApplication::updatePassThruWindow(const QRegion &region)
{
    if (region.isEmpty() || MIMApplication::remoteWindowPixmap().isNull()) {
        qDebug() << "Skipping update request for passthru window.";
        return;
    }

    mPassThruWindow->updateFromRemoteWindow(region);
}

void MIMApplication::handleMapNotifyEvents(XEvent *ev)
{
    if (wasPassThruWindowMapped(ev)) {
        qDebug() << "MIMApplication" << __PRETTY_FUNCTION__
                 << "PassThru window was mapped.";
        emit passThruWindowMapped();
    } else if (wasPassThruWindowUnmapped(ev)) {
        qDebug() << "MIMApplication" << __PRETTY_FUNCTION__
                 << "PassThru window was unmapped.";
        emit passThruWindowUnmapped();
    }
}

void MIMApplication::handleTransientEvents(XEvent *ev)
{
    if (not mRemoteWindow.get()) {
        return;
    }

    if (mRemoteWindow->wasIconified(ev) || mRemoteWindow->wasUnmapped(ev)) {
        qDebug() << "MIMApplication" << __PRETTY_FUNCTION__
                 << "Remote window was destroyed or iconified - hiding.";

        emit remoteWindowGone();
        mRemoteWindow.reset();
    }
}

void MIMApplication::setTransientHint(WId newRemoteWinId)
{
    if (0 == newRemoteWinId) {
        return;
    }

    if (mRemoteWindow.get() && mRemoteWindow->id() == newRemoteWinId) {
        return;
    }

    mRemoteWindow.reset(new MImRemoteWindow(newRemoteWinId));
    mRemoteWindow->setIMWidget(mPassThruWindow->window());

    connect(mRemoteWindow.get(), SIGNAL(contentUpdated(QRegion)),
            this,                SLOT(updatePassThruWindow(QRegion)));

    emit remoteWindowChanged(mRemoteWindow.get());
}

QWidget *MIMApplication::passThruWindow() const
{
    return mPassThruWindow.get();
}

MIMApplication *MIMApplication::instance()
{
    return static_cast<MIMApplication *>(QCoreApplication::instance());
}

bool MIMApplication::wasPassThruWindowMapped(XEvent *ev) const
{
    return (MapNotify == ev->type &&
            static_cast<WId>(ev->xmap.event) == mPassThruWindow->effectiveWinId());
}

bool MIMApplication::wasPassThruWindowUnmapped(XEvent *ev) const
{
    return (UnmapNotify == ev->type &&
            static_cast<WId>(ev->xunmap.event) == mPassThruWindow->effectiveWinId());
}

void MIMApplication::handleDamageEvents(XEvent *event)
{
    if (not mRemoteWindow.get()) {
        return;
    }

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

#ifdef UNIT_TEST
MImRemoteWindow *MIMApplication::remoteWindow() const
{
    return mRemoteWindow.get();
}
#endif

const QPixmap &MIMApplication::remoteWindowPixmap()
{
    if (not mApp || not mApp->mRemoteWindow.get()) {
        static const QPixmap empty;
        return empty;
    }

    return mApp->mRemoteWindow->windowPixmap();
}
