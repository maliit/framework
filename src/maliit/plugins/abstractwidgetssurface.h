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

#ifndef MALIIT_PLUGINS_ABSTRACTWIDGETSSURFACE_H
#define MALIIT_PLUGINS_ABSTRACTWIDGETSSURFACE_H

#include <maliit/plugins/abstractsurface.h>

class QGraphicsItem;
class QGraphicsScene;
class QGraphicsView;
class QWidget;

namespace Maliit {
namespace Plugins {

/*!
 * \brief The AbstractWidgetSurface class provides an AbstractSurface backed by a QWidget
 */
class AbstractWidgetSurface : public virtual AbstractSurface
{
public:
    virtual ~AbstractWidgetSurface();

    /*!
     * \brief returns a QWidget to put children widgets into it
     * \return the QWidget backing this surface
     */
    virtual QWidget *widget() const = 0;
};

/*!
 * \brief The AbstractGraphicsViewSurface class provides an AbstractSurface backed by a QGraphicsView
 */
class AbstractGraphicsViewSurface : public virtual AbstractSurface
{
public:
    virtual ~AbstractGraphicsViewSurface();

    /*!
     * \brief returns the QGraphicsScene of the surface
     * \return the QGraphicsScene of the surface
     */
    virtual QGraphicsScene *scene() const = 0;

    /*!
     * \brief returns the QGraphicsView of the surface
     * \return the QGraphicsView of the surface
     */
    virtual QGraphicsView *view() const = 0;

    /*!
     * \brief returns the root QGraphicsItem of the surface
     * \return the root QGraphicsItem of the surface
     */
    virtual QGraphicsItem *root() const = 0;

    /*!
     * \brief clear this surface
     */
    virtual void clear() = 0;
};

} // namespace Plugins
} // namespace Maliit

#endif // MALIIT_PLUGINS_ABSTRACTWIDGETSSURFACE_H
