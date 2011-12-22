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

#include "mimrotationanimation.h"
#include "qdebug.h"
#include "mimremotewindow.h"
#include "mimxapplication.h"
#include "mpassthruwindow.h"

#include <QBitmap>
#include <QDesktopWidget>
#include <QGraphicsScene>
#include <QObject>
#include <QPropertyAnimation>
#include <QResizeEvent>
#include <QX11Info>

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>

SnapshotPixmapItem::SnapshotPixmapItem(QGraphicsItem *parent) :
        QObject(),
        QGraphicsPixmapItem(parent)
{
}

SnapshotPixmapItem::SnapshotPixmapItem(QPixmap pixmap, QGraphicsItem *parent) :
        QObject(),
        QGraphicsPixmapItem(pixmap, parent)
{
}

SnapshotPixmapItem::~SnapshotPixmapItem() {}


MImDamageMonitor::MImDamageMonitor(MImRemoteWindow* remoteWin,
                                   QObject* parent) :
    QObject(parent),
    timeoutTimer(),
    damageDetected(false)
{
    timeoutTimer.setInterval(1000);
    timeoutTimer.setSingleShot(true);
    connect(&timeoutTimer,SIGNAL(timeout()),
            this,SLOT(timeoutExpired()));

    remoteWindowChanged(remoteWin);
}

void
MImDamageMonitor::remoteWindowChanged(MImRemoteWindow *newRemoteWindow)
{
    remoteWindow = newRemoteWindow;
}

void
MImDamageMonitor::activate()
{
    damageDetected = false;
    // reset triggers
    cancel();
    connect(remoteWindow, SIGNAL(contentUpdated(QRegion)),
            this, SLOT(contentUpdated(QRegion)));
}

void
MImDamageMonitor::waitForDamage()
{
    timeoutTimer.start();
    if (damageDetected) {
        qDebug() << __PRETTY_FUNCTION__ << " - damage already received, emitting signal.";
        cancel();
        Q_EMIT damageReceivedOrTimeout();
    }
}

void
MImDamageMonitor::contentUpdated(QRegion)
{
    damageDetected = true;
    // Emit signal only after client called waitForDamage().
    if (timeoutTimer.isActive()) {
        qDebug() << __PRETTY_FUNCTION__ << " - damage received, emitting signal.";
        cancel();
        Q_EMIT damageReceivedOrTimeout();
    }
}

void
MImDamageMonitor::cancel()
{
    timeoutTimer.stop();
    if (remoteWindow) {
        disconnect(remoteWindow, SIGNAL(contentUpdated(QRegion)),
                   this, SLOT(contentUpdated(QRegion)));
    }
}

void
MImDamageMonitor::timeoutExpired()
{
    qDebug() << __PRETTY_FUNCTION__;
    cancel();
    Q_EMIT damageReceivedOrTimeout();
}

namespace {
    qreal rotateAngle(int startAngle, int endAngle) {
        // Special cases ensuring shortest rotation angle according to original code from
        // MCrossFadedOrientationAnimationPrivate::setRootElementRotationAnimationValues.
        if (startAngle == 270 && endAngle == 90) {
            return 180.0;
        } else if (startAngle == 180 && endAngle == 0) {
            return 180.0;
        } else if (startAngle == 270 && endAngle == 0) {
            return 90.0;
        } else if (startAngle == 0 && endAngle == 270) {
            return -90.0;
        } else {
            return endAngle - startAngle;
        }
    }
}

MImRotationAnimation::MImRotationAnimation(QWidget* snapshotWidget, QWidget* parent, MImXApplication *application) :
        QGraphicsView(new QGraphicsScene(), parent),
        snapshotWidget(snapshotWidget),
        remoteWindow(0),
        animationStartPixmapItem(0),
        animationEndPixmapItem(0),
        startOrientationAngle(0),
        currentOrientationAngle(0),
        aboutToChangeReceived(false),
        damageMonitor(0),
        mApplication(application),
        compositeWindowPixmapUpdated(false)
{
    // Animation plays on top of a black backround,
    // covering up the underlying application.
    setBackgroundBrush(QBrush(Qt::black));

    // Get rid of Meegotouch decorations.
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

    // Avoid drawing transparent pixel borders.
    setFrameShape(QFrame::NoFrame);

    // We do not want input focus for that window.
    setAttribute(Qt::WA_X11DoNotAcceptFocus);

    // Avoid a white flicker when popping up our
    // extra window for the animation.
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_OpaquePaintEvent);
    viewport()->setAttribute(Qt::WA_NoSystemBackground);
    viewport()->setAttribute(Qt::WA_OpaquePaintEvent);

    // Remove all animation elements from scene and hide.
    connect(&rotationAnimationGroup, SIGNAL(finished()),
            this, SLOT(clearScene()));

    connect(mApplication, SIGNAL(remoteWindowChanged(MImRemoteWindow*)),
            this, SLOT(remoteWindowChanged(MImRemoteWindow*)), Qt::UniqueConnection);

    damageMonitor = new MImDamageMonitor(remoteWindow, this);
    connect(damageMonitor, SIGNAL(damageReceivedOrTimeout()),
            this, SLOT(startAnimation()));

    hide();

    if (QApplication::desktop()) {
        resize(QApplication::desktop()->screenGeometry().size());
    }
}

void
MImRotationAnimation::cancelAnimation()
{
    // disableAnimation() slot is connected to passThruWindowUnmapped()
    // So, in order to be on the safe side. We clean the scene.
    if (rotationAnimationGroup.state() != QAbstractAnimation::Stopped) {
        rotationAnimationGroup.stop();
    }

    damageMonitor->cancel();
    clearScene();

    aboutToChangeReceived = false;
}


void
MImRotationAnimation::resizeEvent(QResizeEvent *event)
{
    setSceneRect(QRect(QPoint(0,0),event->size()));
}

void
MImRotationAnimation::setupScene()
{
    animationStartPixmapItem = new SnapshotPixmapItem(compositeWindowStart);
    // See crossfadedorientationanimationstyle.css for the origin of these values.
    animationStartPixmapItem->setTransformOriginPoint(240,240);
    animationStartPixmapItem->setPos(0,0);
    if (scene()) {
        scene()->addItem(animationStartPixmapItem);
    }
}

void
MImRotationAnimation::showInitial()
{
    show();
    // We need to be on the screen really fast, otherwise you would
    // see the meegotouch orientation change in the window under us.
    // Going through the event loop will take about 800ms, which is too long.
    // Thus, requesting a synchronous update here.
    // TODO: Not a perfect solution, in rare cases and high load situations
    // application is updating the screen faster than meego-im-uiserver.
    repaint();
}

void
MImRotationAnimation::showEvent(QShowEvent*) {
    // Setting _NET_WM_WINDOW_TYPE to _NET_WM_WINDOW_TYPE_INPUT
    // prevents the underlying app from losing focus and
    // avoid window decorations such as the slide-in animation.
    static Atom input = XInternAtom(QX11Info::display(), "_NET_WM_WINDOW_TYPE_INPUT", False);
    static Atom window_type = XInternAtom(QX11Info::display(), "_NET_WM_WINDOW_TYPE", False);
    XChangeProperty(QX11Info::display(), effectiveWinId(), window_type, XA_ATOM, 32,
                    PropModeReplace, (unsigned char *) &input, 1);
}

void
MImRotationAnimation::setupAnimation(int fromAngle, int toAngle) {

    qreal hiddenOpacity = 0;
    qreal visibleOpacity = 1.0;

    // See crossfadedorientationanimationstyle.css for the origin of the animation parameters.
    int duration = 500;

    QPropertyAnimation* startAnimations[2];
    QPropertyAnimation* endAnimations[4];

    startAnimations[0] = new QPropertyAnimation();
    startAnimations[1] = new QPropertyAnimation();
    endAnimations[0] = new QPropertyAnimation();
    endAnimations[1] = new QPropertyAnimation();
    endAnimations[2] = new QPropertyAnimation();
    endAnimations[3] = new QPropertyAnimation();

    int initialRotationOfTarget = fromAngle - toAngle;
    int rotateBy = rotateAngle(fromAngle, toAngle);

    QPixmap remoteWindowPixmap;
    if (remoteWindow) {
        // This is the live pixmap of the application coming from X.
        // It stays live if it's used in read-only mode.
        remoteWindowPixmap = remoteWindow->windowPixmap();
    }
    animationEndPixmapItem = new SnapshotPixmapItem(remoteWindowPixmap);
    animationEndPixmapItem->setPos(0,0);
    animationEndPixmapItem->setTransformOriginPoint(240,240);
    animationEndPixmapItem->setRotation(initialRotationOfTarget);
    animationEndPixmapItem->setOpacity(hiddenOpacity);

    animationEndVkbOverlayItem = new SnapshotPixmapItem(grabVkbOnly());
    animationEndVkbOverlayItem->setPos(0,0);
    animationEndVkbOverlayItem->setTransformOriginPoint(240,240);
    animationEndVkbOverlayItem->setRotation(initialRotationOfTarget);
    animationEndVkbOverlayItem->setOpacity(hiddenOpacity);

    if (scene()) {
        scene()->addItem(animationEndPixmapItem);
        scene()->addItem(animationEndVkbOverlayItem);
    }

    startAnimations[0]->setPropertyName("rotation");
    startAnimations[0]->setStartValue(0);
    startAnimations[0]->setEndValue(rotateBy);
    startAnimations[0]->setEasingCurve(QEasingCurve::InOutExpo);
    startAnimations[0]->setDuration(duration);

    startAnimations[1]->setPropertyName("opacity");
    startAnimations[1]->setStartValue(visibleOpacity);
    startAnimations[1]->setEndValue(hiddenOpacity);
    startAnimations[1]->setEasingCurve(QEasingCurve::InOutExpo);
    startAnimations[1]->setDuration(duration);

    endAnimations[0]->setPropertyName("rotation");
    endAnimations[0]->setStartValue(initialRotationOfTarget);
    endAnimations[0]->setEndValue(initialRotationOfTarget+rotateBy);
    endAnimations[0]->setEasingCurve(QEasingCurve::InOutExpo);
    endAnimations[0]->setDuration(duration);

    endAnimations[1]->setPropertyName("opacity");
    endAnimations[1]->setStartValue(hiddenOpacity);
    endAnimations[1]->setEndValue(visibleOpacity);
    endAnimations[1]->setEasingCurve(QEasingCurve::InOutExpo);
    endAnimations[1]->setDuration(duration);

    // same for vkb overlay
    endAnimations[2]->setPropertyName("rotation");
    endAnimations[2]->setStartValue(initialRotationOfTarget);
    endAnimations[2]->setEndValue(initialRotationOfTarget+rotateBy);
    endAnimations[2]->setEasingCurve(QEasingCurve::InOutExpo);
    endAnimations[2]->setDuration(duration);

    endAnimations[3]->setPropertyName("opacity");
    endAnimations[3]->setStartValue(hiddenOpacity);
    endAnimations[3]->setEndValue(visibleOpacity);
    endAnimations[3]->setEasingCurve(QEasingCurve::InOutExpo);
    endAnimations[3]->setDuration(duration);

    startAnimations[0]->setTargetObject(animationStartPixmapItem);
    startAnimations[1]->setTargetObject(animationStartPixmapItem);
    endAnimations[0]->setTargetObject(animationEndPixmapItem);
    endAnimations[1]->setTargetObject(animationEndPixmapItem);
    endAnimations[2]->setTargetObject(animationEndVkbOverlayItem);
    endAnimations[3]->setTargetObject(animationEndVkbOverlayItem);


    rotationAnimationGroup.clear();
    rotationAnimationGroup.addAnimation(startAnimations[0]);
    rotationAnimationGroup.addAnimation(startAnimations[1]);
    rotationAnimationGroup.addAnimation(endAnimations[0]);
    rotationAnimationGroup.addAnimation(endAnimations[1]);
    rotationAnimationGroup.addAnimation(endAnimations[2]);
    rotationAnimationGroup.addAnimation(endAnimations[3]);
}

MImRotationAnimation::~MImRotationAnimation() {
    QGraphicsScene *myScene = scene();
    setScene(0);
    delete myScene;

    delete damageMonitor;
    damageMonitor = 0;

    rotationAnimationGroup.clear();
}


void
MImRotationAnimation::grabComposited()
{
    qDebug() << __PRETTY_FUNCTION__;
    if (!remoteWindow || remoteWindow->windowPixmap().isNull()) {
        compositeWindowStart = QPixmap();
    } else {
        QImage grabImage;

        //check if composite window is already updated, i.e. updateCompositeWindowPixmap(),
        //grab it only if necessary

        if (!compositeWindowPixmapUpdated) {
            // Explicitly copying here since we want to paint the keyboard on top.
            // Qt is unable to render text into QPixmap, therefore we use QImage here.
            grabImage = QImage(remoteWindow->windowPixmap().toImage());
        } else {
            grabImage = QImage(compositeWindowStart.toImage());
        }

        //proper background is provided already in grabImage
        mApplication->setSuppressBackground(true);

        // Overlay keyboard, transparency not required
        QPainter painter(&grabImage);
        snapshotWidget->render(&painter,QPoint(0,0),QRect(0,0,width(),height()));
        painter.end();

        mApplication->setSuppressBackground(false);

        compositeWindowStart = QPixmap::fromImage(grabImage);
    }
}

QPixmap
MImRotationAnimation::grabVkbOnly()
{
    mApplication->setSuppressBackground(true);
    // We need to work with a QImage here, otherwise we lose the
    // transparency of the see-through part of the keyboard image.
    QImage grabImage(size(),QImage::Format_ARGB32);
    grabImage.fill(Qt::transparent);

    // Fill empty QImage with keyboard snapshot.
    QPainter painter(&grabImage);
    snapshotWidget->render(&painter,QPoint(0,0),QRect(0,0,width(),height()));
    // QPainter needs to be closed before returning
    // new QPixmap generated from QImage.
    painter.end();

    mApplication->setSuppressBackground(false);

    return QPixmap::fromImage(grabImage);
}

void
MImRotationAnimation::remoteWindowChanged(MImRemoteWindow* newRemoteWindow) {
    remoteWindow = newRemoteWindow;
    damageMonitor->remoteWindowChanged(newRemoteWindow);

    // Stop playing animations when underlying window is unmapped.
    if (!remoteWindow) {
        qDebug() << __PRETTY_FUNCTION__ << " - remote window gone, cancelling animation.";
        cancelAnimation();
    }
}

#if defined(Q_WS_X11)
void
MImRotationAnimation::updateCompositeWindowPixmap(Qt::HANDLE pixmapHandle) {
    qDebug() << __PRETTY_FUNCTION__ << " - pixmapHandle:" << pixmapHandle;
    compositeWindowStart = QPixmap::fromX11Pixmap(pixmapHandle).copy();

    compositeWindowPixmapUpdated = !compositeWindowStart.isNull();
}
#endif

void
MImRotationAnimation::appOrientationAboutToChange(int toAngle) {
    qDebug() << __PRETTY_FUNCTION__ << " - toAngle: " << toAngle;

    if (!mApplication->passThruWindow()->isVisible()
        || toAngle == currentOrientationAngle
        || aboutToChangeReceived) {
        return;
    }
    startOrientationAngle = currentOrientationAngle;

    // Assuming that in self-composited case we don't need
    // extra redirection, we're already redirected.
    if (not mApplication->selfComposited() && remoteWindow) {
        remoteWindow->redirect();
    }

    // Capturing initial snapshot that's used for the beginning of the rotation.
    grabComposited();

    // Do not do anything if no initial snapshot is available
    if (compositeWindowStart.isNull())
        return;

    // Clean up if we were still in the middle of a previous rotation animation.
    rotationAnimationGroup.stop();
    if (scene()) {
        scene()->clear();
    }

    setupScene();
    showInitial();

    damageMonitor->activate();

    // Unfortunately, we need to shield against appOrientationChangeFinishedEvents
    // that come in without a previous "AboutToChange" event.
    aboutToChangeReceived = true;
}

void
MImRotationAnimation::appOrientationChangeFinished(int toAngle) {
    qDebug() << __PRETTY_FUNCTION__ << " - toAngle: " << toAngle
            << " startOrientationAngle: " << startOrientationAngle;

    currentOrientationAngle = toAngle;

    if (!mApplication->passThruWindow()->isVisible()
        || toAngle == startOrientationAngle
        || !aboutToChangeReceived) {
        clearScene();
        aboutToChangeReceived = false;
        return;
    }

    setupAnimation(startOrientationAngle, toAngle);

    damageMonitor->waitForDamage();
}


void
MImRotationAnimation::startAnimation()
{
    qDebug() << __PRETTY_FUNCTION__;

    rotationAnimationGroup.start();
    aboutToChangeReceived = false;

    compositeWindowPixmapUpdated = false;
}

void MImRotationAnimation::clearScene() {
    // When self-compositing is off, we don't need to maintain
    // the redirection.
    if (not mApplication->selfComposited() && remoteWindow) {
        remoteWindow->unredirect();
    }

    hide();

    if (scene()) {
        scene()->clear();
    }

    compositeWindowPixmapUpdated = false;
    compositeWindowStart = QPixmap();
}
