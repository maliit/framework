/* * This file is part of Maliit framework *
 *
 * Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 *
 * Contact: maliit-discuss@lists.maliit.org
 *
 * Copyright (C) 2012 One Laptop per Child Association
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 */

#ifndef MALIIT_PLUGINS_WINDOWEDSURFACE_H
#define MALIIT_PLUGINS_WINDOWEDSURFACE_H

#include <QWeakPointer>

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QWindow>
#else
#include <QWidget> // For WId
#endif

#include <vector>

#include <maliit/plugins/abstractsurfacefactory.h>

namespace Maliit {
namespace Server {

class WindowedSurfaceFactoryPrivate;
class WindowedSurface;

class WindowedSurfaceFactory : public Maliit::Plugins::AbstractSurfaceFactory
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(WindowedSurfaceFactory)

public:
    WindowedSurfaceFactory();
    virtual ~WindowedSurfaceFactory();

    QSize screenSize() const;

    bool supported(Maliit::Plugins::AbstractSurface::Options options) const;
    QSharedPointer<Maliit::Plugins::AbstractSurface> create(Maliit::Plugins::AbstractSurface::Options options,
                                                            const QSharedPointer<Maliit::Plugins::AbstractSurface> &parent);

    void activate();
    void deactivate();

    void applicationFocusChanged(WId winId);

    void updateInputMethodArea();

#ifdef HAVE_WAYLAND
    void *getInputPanelSurface(QWindow *window);
#endif

Q_SIGNALS:
    void inputMethodAreaChanged(const QRegion &region);
    void surfaceWidgetCreated(QWidget *widget, int options);

private:
    QScopedPointer<WindowedSurfaceFactoryPrivate> d_ptr;
};

} // namespace Server
} // namespace Maliit

#endif // MALIIT_SERVER_DEFAULTSURFACE_H
