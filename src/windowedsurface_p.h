/*
 * This file is part of Maliit framework *
 *
 * Copyright (C) 2012 Intel Corporation and/or its subsidiary(-ies).
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

#ifndef MALIIT_SERVER_WINDOWED_SURFACE_P_H
#define MALIIT_SERVER_WINDOWED_SURFACE_P_H

#include <QObject>
#include <QPoint>
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QWindow>
#else
#include <QWidget> // For WId
#endif

#include <vector>

#ifdef HAVE_WAYLAND
#include <wayland-client.h>
#include "wayland-desktop-shell-client-protocol.h"
#endif

#include "maliit/plugins/abstractsurface.h"

namespace Maliit {
namespace Server {

class WindowedSurfaceFactory;

class WindowedSurface : public QObject,
                        public virtual Maliit::Plugins::AbstractSurface
{
    Q_OBJECT

public:
    WindowedSurface(WindowedSurfaceFactory *factory,
                    AbstractSurface::Options options,
                    const QSharedPointer<WindowedSurface> &parent,
                    QWidget *toplevel);

    ~WindowedSurface();

    void show();
    void hide();
    QSize size() const;
    void setSize(const QSize &size);
    QPoint relativePosition() const;
    void setRelativePosition(const QPoint &position);
    QSharedPointer<AbstractSurface> parent() const;
    QPoint translateEventPosition(const QPoint &eventPosition,
                                  const QSharedPointer<AbstractSurface> &eventSurface = QSharedPointer<AbstractSurface>()) const;
    void setActive(bool active);
    void applicationFocusChanged(WId winId);
    QRegion inputMethodArea();
    virtual QWidget *widget() const = 0;

private:
    void updateVisibility();

protected:
    bool isWindow() const;
    QPoint mapToGlobal(const QPoint &pos) const;

    bool eventFilter(QObject *object, QEvent *event);

    WindowedSurfaceFactory *mFactory;
    Options mOptions;
    QSharedPointer<WindowedSurface> mParent;
    QScopedPointer<QWidget> mToplevel;
    bool mActive;
    bool mVisible;
    QPoint mRelativePosition;

#ifdef HAVE_WAYLAND
    struct input_panel_surface *mSurface;
    struct wl_subsurface *mSubsurface;
#endif
};

} // namespace Server
} // namespace Maliit

#endif // MALIIT_SERVER_WINDOWED_SURFACE_P_H
