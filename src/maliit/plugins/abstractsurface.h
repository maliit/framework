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

#ifndef MALIIT_SERVER_SURFACE_H
#define MALIIT_SERVER_SURFACE_H

#include <QPoint>
#include <QSharedPointer>
#include <QSize>
#include <QUrl>

class QGraphicsItem;
class QGraphicsScene;
class QGraphicsView;
class QWidget;

namespace Maliit {
namespace Plugins {

enum SurfacePosition {
    SurfacePositionInvalid,
    SurfacePositionOverlay,
    SurfacePositionCenterBottom,
    SurfacePositionLeftBottom,
    SurfacePositionRightBottom
};

class SurfacePolicy {
public:
    SurfacePolicy(const SurfacePosition position, const QSize &defaultLandscapeSize = QSize(400, 100), const QSize &defaultPortraitSize = QSize(400, 100));

    SurfacePosition position() const;
    QSize defaultLandscapeSize() const;
    QSize defaultPortraitSize() const;

private:
    const SurfacePosition mPosition;
    const QSize mDefaultLandscapeSize;
    const QSize mDefaultPortraitSize;
};

class AbstractSurface;
class AbstractWidgetSurface;
class AbstractWindowSurface;

class AbstractSurface
{
public:
    virtual ~AbstractSurface();

    virtual void show() = 0;
    virtual void hide() = 0;

    virtual QSize size() const = 0;
    virtual void setSize(const QSize &size) = 0;

    virtual QPoint relativePosition() const = 0;
    virtual void setRelativePosition(const QPoint &position) = 0;

    virtual QSharedPointer<AbstractSurface> parent() const = 0;
};

class AbstractGraphicsViewSurface : public virtual AbstractSurface
{
public:
    virtual QGraphicsScene *scene() const = 0;
    virtual QGraphicsView *view() const = 0;

    virtual QGraphicsItem *root() const = 0;
    virtual void clear() = 0;
};

class AbstractWidgetSurface : public virtual AbstractSurface
{
public:
    virtual QWidget *widget() const = 0;
};

class AbstractSurfaceFactory
{
public:
    virtual ~AbstractSurfaceFactory();

    virtual QSize screenLandscapeSize() const;
    virtual QSize screenPortraitSize() const;

    virtual QSharedPointer<AbstractGraphicsViewSurface> createGraphicsViewSurface(const SurfacePolicy &policy, const QSharedPointer<AbstractSurface> &parent = QSharedPointer<AbstractSurface>()) = 0;
    virtual QSharedPointer<AbstractWidgetSurface> createWidgetSurface(const SurfacePolicy &policy, const QSharedPointer<AbstractSurface> &parent = QSharedPointer<AbstractSurface>()) = 0;
};

} // namespace Server
} // namespace Maliit

#endif // MALIIT_SERVER_SURFACE_H

