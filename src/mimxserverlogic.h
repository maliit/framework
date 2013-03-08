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

#ifndef MIMX11SERVERLOGIC_H
#define MIMX11SERVERLOGIC_H

#include "mimabstractserverlogic.h"

#include "mimxextension.h"

typedef union _XEvent XEvent;

struct MImServerXOptions;
class MImRemoteWindow;
class MPassThruWindow;
class MImRotationAnimation;

// Server logic for standalone X11 server
//
// For X11 the toplevel window is a MPassThruWindow (managed by MImXServerLogic).
// It contains a MImPluginsProxyWidget as container for all plugin widgets and a
// MImRotationAnimation, which is used to display the rotation animation. MImXServerLogic
// also manages MImRemoteWindows representing the current application window.
class MImXServerLogic : public MImAbstractServerLogic
{
    Q_OBJECT
public:
    explicit MImXServerLogic(const MImServerXOptions &options, QObject *parent = 0);
    virtual ~MImXServerLogic();

    void x11EventFilter(XEvent *ev);
    bool selfCompositionSupported();

    const MImXCompositeExtension& compositeExtension();
    const MImXDamageExtension& damageExtension();

    //! Flag that is used to sync between MAbstractInputMethodHost and
    //! rotation animation in order to capture the VKB without
    //! self-composited background drawn.
    void setSuppressBackground(bool suppress);

    QWidget *passThruWindow() const;

    const QPixmap &remoteWindowPixmap();

    //! For unittesting purposes
    MImRemoteWindow *remoteWindow() const;

public Q_SLOTS:
    //! reimpl
    virtual void inputPassthrough(const QRegion &region);
    virtual void appOrientationAboutToChange(int toAngle);
    virtual void appOrientationChangeFinished(int toAngle);
    virtual void applicationFocusChanged(WId remoteWinId);
    virtual void pluginLoaded();

Q_SIGNALS:
    //! This signal is emitted when remote window is changed.
    //! Parameter can be 0 if window is unmapped.
    void remoteWindowChanged(MImRemoteWindow *newWindow);

private Q_SLOTS:
    void finalize();
    void updatePassThruWindow(const QRegion &region);

private:
    const MImServerXOptions &xOptions;

    void configureWidgetsForCompositing();
    void handleTransientEvents(XEvent *ev);
    void handleRemoteWindowEvents(XEvent *ev);
    void handlePassThruMapEvent(XEvent *ev);

    bool mBackgroundSuppressed;

    MImXCompositeExtension mCompositeExtension;
    MImXDamageExtension mDamageExtension;

    QScopedPointer<MPassThruWindow> mPassThruWindow;
    QScopedPointer<QWidget> mPluginsProxyWidget;
    QScopedPointer<MImRemoteWindow> mRemoteWindow;
    MImRotationAnimation *mRotationAnimation;

    friend class Ut_PassthroughServer;
};

#endif // MIMX11SERVERLOGIC_H
