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
#include "mimpluginsproxywidget.h"

#include <deque>

#ifdef HAVE_MEEGOGRAPHICSSYSTEM
#include <QtMeeGoGraphicsSystemHelper/QMeeGoGraphicsSystemHelper>
#endif

#include <QPainter>

#include <QDebug>
#include <X11/Xlib.h> // must be last include

namespace
{
    const QEasingCurve::Type MaskEasingCurve = QEasingCurve::Linear;
    const int MaskFadeOutDuration = 500;

    bool configureForCompositing(QWidget *w)
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

        if (mApp && not mApp->selfComposited()) {
            // Careful: This flag can trigger a call to
            // qt_x11_recreateNativeWidgetsRecursive
            // - which will crash when it tries to get the effective WId
            // (as none of widgets have been mapped yet).
            w->setAttribute(Qt::WA_TranslucentBackground);
        }

        return true;
    }
}

MIMApplication::MIMApplication(int &argc, char **argv)
    : QApplication(argc, argv),
      mCompositeExtension(),
      mDamageExtension(),
      mSelfComposited(false),
      mManualRedirection(false),
      mBypassWMHint(false),
      mBackgroundSuppressed(false),
      mMasked(false),
      mMaskOpacity(1.0f),
      mMaskAnimation(this, "maskOpacity")

{
    parseArguments(argc, argv);

#ifdef HAVE_MEEGOGRAPHICSSYSTEM
    QMeeGoGraphicsSystemHelper::setSwitchPolicy(QMeeGoGraphicsSystemHelper::NoSwitch);
#endif

    mPassThruWindow.reset(new MPassThruWindow);
    mPluginsProxyWidget.reset(new MImPluginsProxyWidget(mPassThruWindow.get()));
    configureForCompositing(mPassThruWindow.get());

    connect(this, SIGNAL(aboutToQuit()),
            this, SLOT(finalize()),
            Qt::UniqueConnection);

    mMaskAnimation.setEasingCurve(MaskEasingCurve);
    mMaskAnimation.setDuration(MaskFadeOutDuration);
    mMaskAnimation.setStartValue(qreal(1.0f));
    mMaskAnimation.setEndValue(qreal(0.0f));
}

void MIMApplication::finalize()
{
    // Cannot destroy QWidgets or QPixmaps during MIMApplication destruction.
    // Hence the finalize handler that runs before the d'tor.
    mPluginsProxyWidget.reset();
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
    handleTransientEvents(ev);
    handleRemoteWindowEvents(ev);
    handlePassThruMapEvent(ev);
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

void MIMApplication::handleTransientEvents(XEvent *ev)
{
    if (not mRemoteWindow.get()) {
        return;
    }

    if (mRemoteWindow->wasIconified(ev) || mRemoteWindow->wasUnmapped(ev)) {
        qDebug() << "MIMApplication" << __PRETTY_FUNCTION__
                 << "Remote window was destroyed or iconified - hiding.";

        emit remoteWindowChanged(0);
        mRemoteWindow.reset();
    }
}

void MIMApplication::handlePassThruMapEvent(XEvent *ev)
{
    if (ev->type != MapNotify)
        return;

    if (ev->xmap.window != mPassThruWindow->effectiveWinId())
        return;

    if (not mRemoteWindow.get()) {
        qWarning() << __PRETTY_FUNCTION__
                   << "No remote window found, but passthru window was mapped.";
        return;
    }

    mRemoteWindow->resetPixmap();
}

void MIMApplication::setTransientHint(WId newRemoteWinId)
{
    if (0 == newRemoteWinId) {
        return;
    }

    if (mRemoteWindow.get() && mRemoteWindow->id() == newRemoteWinId) {
        return;
    }

    const bool wasRedirected(mRemoteWindow.get() && mRemoteWindow->isRedirected());

    mRemoteWindow.reset(new MImRemoteWindow(newRemoteWinId));
    mRemoteWindow->setIMWidget(mPassThruWindow->window());

    connect(mRemoteWindow.get(), SIGNAL(contentUpdated(QRegion)),
            this,                SLOT(updatePassThruWindow(QRegion)));

    if (wasRedirected) {
        mRemoteWindow->redirect();
    }

    emit remoteWindowChanged(mRemoteWindow.get());
}

QWidget *MIMApplication::passThruWindow() const
{
    return mPassThruWindow.get();
}

QWidget* MIMApplication::pluginsProxyWidget() const
{
    return mPluginsProxyWidget.get();
}

MIMApplication *MIMApplication::instance()
{
    return static_cast<MIMApplication *>(QCoreApplication::instance());
}

void MIMApplication::handleRemoteWindowEvents(XEvent *event)
{
    if (not mRemoteWindow.get()) {
        return;
    }

    mRemoteWindow->handleEvent(event);
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

void MIMApplication::setSuppressBackground(bool suppress)
{
    mBackgroundSuppressed = suppress;
}

#ifdef UNIT_TEST
MImRemoteWindow *MIMApplication::remoteWindow() const
{
    return mRemoteWindow.get();
}
#endif

const QPixmap &MIMApplication::remoteWindowPixmap()
{
    if (not mApp || not mApp->mRemoteWindow.get()
            || mApp->mBackgroundSuppressed
            || (not mApp->mMasked && not mApp->mSelfComposited)) {
        static const QPixmap empty;
        return empty;
    }

    if (mApp->mMasked) {
        static QPixmap composited;
        if (mApp->mSelfComposited) {
            composited = mApp->mRemoteWindow->windowPixmap();
        } else {
            composited = QPixmap(mApp->mPassThruWindow->size());
            composited.fill(Qt::transparent);
        }
        QPainter p(&composited);
        p.setOpacity(mApp->mMaskOpacity);
        for (int i = 0; i < mApp->mPassThruWindow.get()->region().rects().size(); ++i) {
            p.fillRect(mApp->mPassThruWindow.get()->region().rects().at(i), QBrush(Qt::black));
        }
        return composited;
    }

    return mApp->mRemoteWindow->windowPixmap();
}

void MIMApplication::visitWidgetHierarchy(WidgetVisitor visitor,
                                          QWidget *widget)
{
    if (not mApp) {
        return;
    }

    std::deque<QWidget *> unvisited;
    unvisited.push_back(widget ? widget : mApp->passThruWindow());

    // Breadth-first traversal of widget hierarchy, until no more
    // unvisited widgets remain. Will find viewports of QGraphicsViews,
    // as QAbstractScrollArea reparents the viewport to itself.
    while (not unvisited.empty()) {
        QWidget *current = unvisited.front();
        unvisited.pop_front();

        // If true, then continue walking the hiearchy of current widget.
        if (visitor(current)) {
            // Mark children of current widget as unvisited:
            foreach (QObject *obj, current->children()) {
                if (QWidget *w = qobject_cast<QWidget *>(obj)) {
                    unvisited.push_back(w);
                }
            }
        }
    }
}

void MIMApplication::configureWidgetsForCompositing(QWidget *widget)
{
    MIMApplication::visitWidgetHierarchy(configureForCompositing, widget);
}

void MIMApplication::enableBackgroundMask()
{
    if (mMasked || mMaskAnimation.state() == QAbstractAnimation::Running) {
        return;
    }
    mMasked = true;
    setMaskOpacity(mMaskAnimation.startValue().toReal());
    mMaskAnimation.stop();

    if (mRemoteWindow.get() && mPassThruWindow.get()) {
        mRemoteWindow->update(mPassThruWindow->region());
    }
}

void MIMApplication::disableBackgroundMask(bool instantly)
{
    if (not mMasked) {
        return;
    }

    if (instantly) {
        mMasked = false;
        setMaskOpacity(mMaskAnimation.endValue().toReal());
        mMaskAnimation.stop();
    } else if (mMaskAnimation.state() != QAbstractAnimation::Running){
        mMaskAnimation.start();
    }

    if (mRemoteWindow.get() && mPassThruWindow.get()) {
        mRemoteWindow->update(mPassThruWindow->region());
    }
}

qreal MIMApplication::maskOpacity() const
{
    return mMaskOpacity;
}

void MIMApplication::setMaskOpacity(qreal opacity)
{
    mMaskOpacity = opacity;

    if (!mMaskOpacity) {
        mMasked = false;
    }

    if (mRemoteWindow.get() && mPassThruWindow.get()) {
        mRemoteWindow->update(mPassThruWindow->region());
    }
}

