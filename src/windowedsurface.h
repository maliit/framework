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

#include <vector>

#include <maliit/plugins/abstractsurfacefactory.h>

namespace Maliit {
namespace Server {

class WindowedSurface;

class WindowedSurfaceFactory : public Maliit::Plugins::AbstractSurfaceFactory
{
public:
    WindowedSurfaceFactory();
    virtual ~WindowedSurfaceFactory();

    bool supported(Maliit::Plugins::AbstractSurface::Options options) const;
    QSharedPointer<Maliit::Plugins::AbstractSurface> create(Maliit::Plugins::AbstractSurface::Options options, const QSharedPointer<Maliit::Plugins::AbstractSurface> &parent);

    void activate();
    void deactivate();

private:
    std::vector<QWeakPointer<WindowedSurface> > surfaces;
};


} // namespace Server
} // namespace Maliit

#endif // MALIIT_SERVER_DEFAULTSURFACE_H
