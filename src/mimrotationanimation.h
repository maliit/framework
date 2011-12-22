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

#ifndef MIMROTATIONANIMATION_H
#define MIMROTATIONANIMATION_H

#include <QPixmap>
#include <QObject>
#include <QParallelAnimationGroup>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QTimer>

class MImXApplication;

/*! \internal
 * \ingroup maliitserver
 * \brief Custom QObject'ified PixmapItem that's placed on the scene, one
 * instance for the origin snapshot, one for the target of the rotation
 * (crossfading).
 *
 * Inherits from QObject so that it can have animatable properties.
 */
class SnapshotPixmapItem : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT
    Q_PROPERTY(QPointF pos READ pos WRITE setPos)
    Q_PROPERTY(double opacity READ opacity WRITE setOpacity)
    Q_PROPERTY(double rotation READ rotation WRITE setRotation)

public:
    SnapshotPixmapItem(QPixmap pixmap, QGraphicsItem* parent = 0);
    SnapshotPixmapItem(QGraphicsItem *parent = 0);

    virtual ~SnapshotPixmapItem();

private:
    Q_DISABLE_COPY(SnapshotPixmapItem)
};


class MImRemoteWindow;

class MImDamageMonitor : public QObject {
    Q_OBJECT

public:
    explicit MImDamageMonitor(MImRemoteWindow* remoteWin, QObject* parent = 0);
    void activate();
    void waitForDamage();
    void cancel();
    void remoteWindowChanged(MImRemoteWindow* newRemoteWindow);

Q_SIGNALS:
    void damageReceivedOrTimeout();

private Q_SLOTS:
    void contentUpdated(QRegion region);
    void timeoutExpired();

private:
    Q_DISABLE_COPY(MImDamageMonitor);

    MImRemoteWindow* remoteWindow;
    QTimer timeoutTimer;
    bool damageDetected;
};


/*! \internal
 * \brief Top-level graphics view to superimpose the rotation animation
 * on top of application and keyboard widgets.
 *
 * This class derives from QGraphicsView for showing the rotation
 * animation. The widget that is used for capturing the start snapshots
 * is specified in the constructor.
 *
 * First, when appOrientationAboutToChange is
 * triggered, a snapshot of the application is recorded by having the
 * snapshot widget draw its contents into a pixmap. This snapshot is
 * displayed on screen immediately. When the applicatin has rotated to the
 * new orientation, the rotation animation is started.
 * Then during the rotation two additional items become visible.
 * The live X11 pixmap from the redirected application window
 * as well as a see-through pixmap containing the keyboard
 * in the final orientation overlaid on top.
 *
 */
class MImRotationAnimation : public QGraphicsView {
    Q_OBJECT

public:
    MImRotationAnimation(QWidget* snapshotWidget, QWidget* parent, MImXApplication *application);
    virtual ~MImRotationAnimation();

public Q_SLOTS:
    /*! Slot for receiving information about the underlying
     * application's orientation status. This triggers preparations
     * for starting the rotation animation, e.g. capturing the starting
     * snapshot.
     */
    void appOrientationAboutToChange(int toAngle);

    /*! Slot for receiving information about the underlying
     * application's orientation status. This triggers playback of the
     * animation after the end snapshot has been taken.
     */
    void appOrientationChangeFinished(int toAngle);

    /*! Used for receiving an update when the underlying application
     * window has changed. MIMOrientationRotator needs to be aware
     * of the change in order to take snapshots of the correct window.
     */
    void remoteWindowChanged(MImRemoteWindow* newRemoteWindow);

#if defined(Q_WS_X11)
    void updateCompositeWindowPixmap(Qt::HANDLE);
#endif

protected:
    virtual void resizeEvent(QResizeEvent *event);
    virtual void showEvent(QShowEvent *event);

private Q_SLOTS:
    void clearScene();
    void startAnimation();

private:
    void setupAnimation(int fromAngle, int toAngle);

    //! \brief Grab snapshot of application and plugin into one fullscreen pixmap
    void grabComposited();

    //! \brief Grab snapshot of plugin only
    QPixmap grabVkbOnly();

    void setupScene();
    void showInitial();

private:
    Q_DISABLE_COPY(MImRotationAnimation)

    void cancelAnimation();

    QWidget* snapshotWidget;
    MImRemoteWindow* remoteWindow;
    QPixmap compositeWindowStart;
    QParallelAnimationGroup rotationAnimationGroup;

    SnapshotPixmapItem *animationStartPixmapItem;
    SnapshotPixmapItem *animationEndPixmapItem;
    SnapshotPixmapItem *animationEndVkbOverlayItem;

    int startOrientationAngle;
    int currentOrientationAngle;
    bool aboutToChangeReceived;

    MImDamageMonitor* damageMonitor;

    MImXApplication* mApplication;

    bool compositeWindowPixmapUpdated;
};

#endif // MIMROTATIONANIMATION_H
