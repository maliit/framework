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

class AbstractSurface;
typedef QSharedPointer<AbstractSurface> SharedSurface;

class AbstractSurface
{
public:
    //! Used to describe the positioning and type of a surface
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

    virtual ~AbstractSurface();
    virtual void show() = 0;
    virtual void hide() = 0;
    virtual QSize size() const = 0;
    virtual void setSize(const QSize &size) = 0;
    virtual QPoint relativePosition() const = 0;
    virtual void setRelativePosition(const QPoint &position) = 0;
    virtual SharedSurface parent() const = 0;
    virtual QPoint translateEventPosition(const QPoint &event_position,
                                          const SharedSurface &event_surface = SharedSurface()) const = 0;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(AbstractSurface::Options)

}} // namespace Plugins, Maliit

#endif // MALIIT_PLUGINS_ABSTRACTSURFACE_H

