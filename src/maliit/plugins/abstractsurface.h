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

#ifndef MALIIT_PLUGINS_ABSTRACTSURFACE_H
#define MALIIT_PLUGINS_ABSTRACTSURFACE_H

#include <QSharedPointer>

class QPoint;
class QSize;

namespace Maliit {
namespace Plugins {

/*! \ingroup pluginapi
 * \brief The AbstractSurface class provides a window abstraction for rendering the plugin.
 *
 */
class AbstractSurface
{
public:
    /*!
     * \brief The Option enum describes the postioning and type of a surface
     */
    enum Option {
        None = 0x0,

        PositionOverlay = 0x1,
        PositionCenterBottom = 0x2,
        PositionLeftBottom = 0x4,
        PositionRightBottom = 0x8,

        TypeWidget = 0x100,
        TypeGraphicsView = 0x200,
        TypeWindow = 0x400,
        TypeQuick1 = 0x800,
        TypeQuick2 = 0x1000
    };
    Q_DECLARE_FLAGS(Options, Option)

    /*!
     * \brief ~AbstractSurface
     */
    virtual ~AbstractSurface();

    /*!
     * \brief shows the surface.
     */
    virtual void show() = 0;
    /*!
     * \brief hides the surface and its children.
     */
    virtual void hide() = 0;

    /*!
     * \brief returns the real size of the surface
     * \return the surface's real size
     */
    virtual QSize size() const = 0;
    /*!
     * \brief sets the size of the surface
     * \param size the requested surface size
     */
    virtual void setSize(const QSize &size) = 0;

    /*!
     * \brief returns the position of the surface relative to its parent
     * \return the surface's position relative to its parent
     */
    virtual QPoint relativePosition() const = 0;

    /*!
     * \brief sets the surface's position relative to its parent (for a PositionOverlay surface)
     * \param position the requested relative position
     */
    virtual void setRelativePosition(const QPoint &position) = 0;

    /*!
     * \brief returns the parent of a surface
     * \return the surface's parent
     */
    virtual QSharedPointer<AbstractSurface> parent() const = 0;

    /*!
     * \brief translates the coordinates of an event into the surfaces coordinate system
     * \param eventPosition the coordinates of the event
     * \param eventSurface the surface where the event occured (by default this)
     * \return the event coordinates translated to the surface coordinate system
     */
    virtual QPoint translateEventPosition(const QPoint &event_position,
                                          const QSharedPointer<AbstractSurface> &event_surface = QSharedPointer<AbstractSurface>()) const = 0;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(AbstractSurface::Options)

} // namespace Plugins
} // namespace Maliit

#endif // MALIIT_PLUGINS_ABSTRACTSURFACE_H

