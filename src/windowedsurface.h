/* * This file is part of meego-im-framework *
 *
 * Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
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

class WindowedSurface;

class WindowedSurfaceFactory : public Maliit::Plugins::AbstractSurfaceFactory
{
    Q_OBJECT

public:
    WindowedSurfaceFactory();
    virtual ~WindowedSurfaceFactory();

    QSize screenSize() const;

    bool supported(Maliit::Plugins::AbstractSurface::Options options) const;
    QSharedPointer<Maliit::Plugins::AbstractSurface> create(Maliit::Plugins::AbstractSurface::Options options, const QSharedPointer<Maliit::Plugins::AbstractSurface> &parent);

    void activate();
    void deactivate();

    void applicationFocusChanged(WId winId);

    void updateInputMethodArea();

Q_SIGNALS:
    void inputMethodAreaChanged(const QRegion &region);
    void surfaceWidgetCreated(QWidget *widget, int options);

private:
    Q_SLOT void screenResized(int screen);

    std::vector<QWeakPointer<WindowedSurface> > surfaces;
    bool mActive;
};

} // namespace Server
} // namespace Maliit

#endif // MALIIT_SERVER_DEFAULTSURFACE_H
