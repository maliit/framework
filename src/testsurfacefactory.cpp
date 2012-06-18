/* * This file is part of Maliit framework *
 *
 * Copyright (C) 2012 Openismus GmbH
 *
 * Contact: maliit-discuss@lists.maliit.org
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 */

#include "testsurfacefactory.h"
#include "windowedsurface.h"

namespace Maliit {
namespace Server {

//! \ingroup maliitserver
//! \brief Allows to create a surface without a running maliit-server instance. Useful for tests.
//!
//! \note Plugins must not use this, but should instead use the Maliit::Plugins::AbstractSurfaceFactory
//! returned by MAbstractInputMethodHost::surfaceFactory().
//!
//! \param options the options the surface should have
//! \param parent the parent of the new surface
//! \returns a new shared Maliit::Plugins::AbstractSurface
QSharedPointer<Maliit::Plugins::AbstractSurface> createTestSurface(Maliit::Plugins::AbstractSurface::Options options, const QSharedPointer<Maliit::Plugins::AbstractSurface> &parent)
{
    static Maliit::Server::WindowedSurfaceFactory factory;

    return factory.create(options, parent);
}

//! \ingroup maliitserver
//! \brief Allows to create a graphics view surface without a running maliit-server instance. Useful for tests.
//!
//! \note Plugins must not use this, but should instead use the Maliit::Plugins::AbstractSurfaceFactory
//! returned by MAbstractInputMethodHost::surfaceFactory().
//!
//! \param parent the parent of the new surface
//! \returns a new shared Maliit::Plugins::AbstractGraphicsViewSurface
QSharedPointer<Maliit::Plugins::AbstractGraphicsViewSurface> createTestGraphicsViewSurface(const QSharedPointer<Maliit::Plugins::AbstractSurface> &parent)
{
    QSharedPointer<Maliit::Plugins::AbstractSurface> surface;

    if (parent) {
        surface = createTestSurface(Maliit::Plugins::AbstractSurface::PositionOverlay | Maliit::Plugins::AbstractSurface::TypeGraphicsView, parent);
    } else {
        surface = createTestSurface(Maliit::Plugins::AbstractSurface::PositionCenterBottom | Maliit::Plugins::AbstractSurface::TypeGraphicsView, parent);
    }

    return qSharedPointerDynamicCast<Maliit::Plugins::AbstractGraphicsViewSurface>(surface);
}

//! \ingroup maliitserver
//! \brief Allows to create a widget surface without a running maliit-server instance. Useful for tests.
//!
//! \note Plugins must not use this, but should instead use the Maliit::Plugins::AbstractSurfaceFactory
//! returned by MAbstractInputMethodHost::surfaceFactory().
//!
//! \param parent the parent of the new surface
//! \returns a new shared Maliit::Plugins::AbstractWidgetSurface
QSharedPointer<Maliit::Plugins::AbstractWidgetSurface> createTestWidgetSurface(const QSharedPointer<Maliit::Plugins::AbstractSurface> &parent)
{
    QSharedPointer<Maliit::Plugins::AbstractSurface> surface;

    if (parent) {
        surface = createTestSurface(Maliit::Plugins::AbstractSurface::PositionOverlay | Maliit::Plugins::AbstractSurface::TypeWidget, parent);
    } else {
        surface = createTestSurface(Maliit::Plugins::AbstractSurface::PositionCenterBottom | Maliit::Plugins::AbstractSurface::TypeWidget, parent);
    }

    return qSharedPointerDynamicCast<Maliit::Plugins::AbstractWidgetSurface>(surface);
}

} // namespace Plugins
} // namespace Maliit
