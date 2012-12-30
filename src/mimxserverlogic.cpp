/* * This file is part of Maliit framework *
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 *
 * Contact: maliit-discuss@lists.maliit.org
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 */

#include "mimxserverlogic.h"

#include "mpassthruwindow.h"
#include "mimremotewindow.h"
#include "mimxextension.h"
#include "mimrotationanimation.h"

#include "mimxapplication.h"
#include "mimpluginsproxywidget.h"
#include "mimrotationanimation.h"
#include "mimserveroptions.h"

#include "windowedsurfacegroup.h"

#include <QDebug>

#include <X11/Xlib.h>

namespace
{
    class ConfigureForCompositing {

    public:
        ConfigureForCompositing(const MImServerXOptions &newOptions)
            : options(newOptions)
        {
        }

        bool operator()(QWidget *w)
        {
            if (not w) {
                return false;
            }

            w->setAttribute(Qt::WA_OpaquePaintEvent);
            w->setAttribute(Qt::WA_NoSystemBackground);
            w->setAutoFillBackground(false);
            // Be aware that one cannot verify whether the background role *is*
            // QPalette::NoRole - see QTBUG-17924.
            w->setBackgroundRole(QPalette::NoRole);

            if (!options.selfComposited) {
                // Careful: This flag can trigger a call to
                // qt_x11_recreateNativeWidgetsRecursive
                // - which will crash when it tries to get the effective WId
                // (as none of widgets have been mapped yet).
                w->setAttribute(Qt::WA_TranslucentBackground);
            }

            return true;
        }

    private:
        const MImServerXOptions &options;

    };


}

MImXServerLogic::MImXServerLogic(const MImServerXOptions &options, QObject *parent) :
    MImAbstractServerLogic(parent),
    xOptions(options),
    mBackgroundSuppressed(false),
    mCompositeExtension(),
    mDamageExtension(),
    mPassThruWindow(),
    mRemoteWindow(),
    mSurfaceGroupFactory(new Maliit::Server::WindowedSurfaceGroupFactory)
{
    mPassThruWindow.reset(new MPassThruWindow(this, xOptions));
    mPluginsProxyWidget.reset(new MImPluginsProxyWidget(mPassThruWindow.data()));
    mRotationAnimation = new MImRotationAnimation(mPluginsProxyWidget.data(), passThruWindow(),
                                                  this, xOptions);

    configureWidgetsForCompositing();
}

MImXServerLogic::~MImXServerLogic()
{
}

void MImXServerLogic::finalize()
{
    // Cannot destroy QWidgets or QPixmaps during MIMApplication destruction.
    // Hence the finalize handler that runs before the d'tor.
    mPluginsProxyWidget.reset();
    mPassThruWindow.reset();
    mRemoteWindow.reset();
}

bool MImXServerLogic::selfCompositionSupported()
{
    return mCompositeExtension.supported(0, 2) && mDamageExtension.supported();
}

void MImXServerLogic::x11EventFilter(XEvent *ev)
{
    handleTransientEvents(ev);
    handleRemoteWindowEvents(ev);
    handlePassThruMapEvent(ev);
}

void MImXServerLogic::handleTransientEvents(XEvent *ev)
{
    if (not mRemoteWindow.data()) {
        return;
    }

    if (mRemoteWindow->wasIconified(ev) || mRemoteWindow->wasUnmapped(ev)) {
        qDebug() << "MImX11ServerLogic" << __PRETTY_FUNCTION__
                 << "Remote window was destroyed or iconified - hiding.";

        Q_EMIT remoteWindowChanged(0);
        Q_EMIT applicationWindowGone();
        mRemoteWindow.reset();
    }
}

void MImXServerLogic::handleRemoteWindowEvents(XEvent *event)
{
    if (not mRemoteWindow.data()) {
        return;
    }

    mRemoteWindow->handleEvent(event);
}

void MImXServerLogic::handlePassThruMapEvent(XEvent *ev)
{
    if (ev->type != MapNotify)
        return;

    if (ev->xmap.window != mPassThruWindow->effectiveWinId())
        return;

    if (not mRemoteWindow.data()) {
        qWarning() << __PRETTY_FUNCTION__
                   << "No remote window found, but passthru window was mapped.";
        return;
    }

    mRemoteWindow->resetPixmap();
}

void MImXServerLogic::applicationFocusChanged(WId newRemoteWinId)
{
    if (0 == newRemoteWinId) {
        return;
    }

    if (mRemoteWindow.data() && mRemoteWindow->id() == newRemoteWinId) {
        return;
    }

    const bool wasRedirected(mRemoteWindow.data() && mRemoteWindow->isRedirected());

    mRemoteWindow.reset(new MImRemoteWindow(newRemoteWinId, this, xOptions));

    QSharedPointer<Maliit::Server::WindowedSurfaceGroupFactory> windowedSurfaceGroupFactory
            = qSharedPointerDynamicCast<Maliit::Server::WindowedSurfaceGroupFactory>(mSurfaceGroupFactory);
    if (windowedSurfaceGroupFactory)
        windowedSurfaceGroupFactory->applicationFocusChanged(newRemoteWinId);

    connect(mRemoteWindow.data(), SIGNAL(contentUpdated(QRegion)),
            this,                SLOT(updatePassThruWindow(QRegion)));

    if (wasRedirected) {
        mRemoteWindow->redirect();
    }

    Q_EMIT remoteWindowChanged(mRemoteWindow.data());
}

void MImXServerLogic::setSuppressBackground(bool suppress)
{
    mBackgroundSuppressed = suppress;
}

QWidget *MImXServerLogic::passThruWindow() const
{
    return mPassThruWindow.data();
}

QSharedPointer<Maliit::Server::AbstractSurfaceGroupFactory> MImXServerLogic::surfaceGroupFactory() const
{
    return mSurfaceGroupFactory;
}

const QPixmap &MImXServerLogic::remoteWindowPixmap()
{
    if (not mRemoteWindow.data()
            || mBackgroundSuppressed
            || not xOptions.selfComposited) {
        static const QPixmap empty;
        return empty;
    }

    return mRemoteWindow->windowPixmap();
}

void MImXServerLogic::updatePassThruWindow(const QRegion &region)
{
    if (region.isEmpty() || remoteWindowPixmap().isNull()) {
        qDebug() << "Skipping update request for passthru window.";
        return;
    }

    mPassThruWindow->updateFromRemoteWindow(region);
}

void MImXServerLogic::pluginLoaded()
{
    configureWidgetsForCompositing();
}

void MImXServerLogic::configureWidgetsForCompositing()
{
    ConfigureForCompositing visitor(xOptions);
    visitWidgetHierarchy(visitor, mPassThruWindow.data());
}

void MImXServerLogic::appOrientationAboutToChange(int toAngle) {
    mRotationAnimation->appOrientationAboutToChange(toAngle);
}

void MImXServerLogic::appOrientationChangeFinished(int toAngle) {
    mRotationAnimation->appOrientationChangeFinished(toAngle);
}

MImRemoteWindow *MImXServerLogic::remoteWindow() const
{
    return mRemoteWindow.data();
}

void MImXServerLogic::inputPassthrough(const QRegion &region)
{
    mPassThruWindow->inputPassthrough(region);
}

const MImXCompositeExtension& MImXServerLogic::compositeExtension()
{
    return mCompositeExtension;
}

const MImXDamageExtension& MImXServerLogic::damageExtension()
{
    return mDamageExtension;
}
