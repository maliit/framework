/* * This file is part of Maliit framework *
 *
 * Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
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

#include "abstractsurface.h"

namespace Maliit {
namespace Plugins {

//! \ingroup pluginapi
//! \class AbstractSurface
//! This class provides a window abstraction for rendering the plugin's
//! content into a surface. It's a pure interface; derived classes have to
//! provide the implementation.

//! \fn AbstractSurface::show
//! Shows the surface on the display.

//! \fn AbstractSurface::hide
//! Hides the surface from the display.

//! \fn AbstractSurface::size
//! \returns The real size of the surface.

//! \fn AbstractSurface::setSize
//! Sets the size of the surface.
//! \param size The new surface size.

//! \fn AbstractSurface::relativePosition
//! \returns The position of the surface relative to its parent.

//! \fn AbstractSurface::setRelativePosition
//! Sets the surface's position relative to its parent. Only works on
//! PositionOverlay surfaces.
//! \param position The new relative position.

//! \fn parent
//! \returns The parent of a surface.

//! \fn translateEventPosition
//! Translates the position of a mouse or touch event from the event
//! coordinate system into the local coordinate system of the surface.
//! \param event_position The position coordinates of the event.
//!                       Usually given in display coordinates.
//! \param event_surface The surface where the event occured (by default this).

AbstractSurface::~AbstractSurface()
{}

}} // namespace Plugins, Maliit
