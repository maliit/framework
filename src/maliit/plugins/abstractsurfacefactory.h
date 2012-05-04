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

#ifndef MALIIT_PLUGINS_ABSTRACTSURFACEFACTORY_H
#define MALIIT_PLUGINS_ABSTRACTSURFACEFACTORY_H

#include <maliit/plugins/abstractsurface.h>

class QSize;

namespace Maliit {
namespace Plugins {

/*! \ingroup pluginapi
 * \brief The AbstractSurfaceFactory class is used to create AbstractSurface instances
 */
class AbstractSurfaceFactory
{
public:
    /*!
     * \brief ~AbstractSurfaceFactory
     */
    virtual ~AbstractSurfaceFactory();

    /*!
     * \brief returns the available size in landscape orientation
     * \return the size in landscape orientation
     */
    virtual QSize screenLandscapeSize() const;
    /*!
     * \brief returns the available size in portrait orientation
     * \return the size in portrait orientation
     */
    virtual QSize screenPortraitSize() const;

    /*!
     * \brief returns if a surface with options can be created
     * \param options the options the surface should have
     * \return true if such a surface can be created
     */
    virtual bool supported(AbstractSurface::Options options) const = 0;

    /*!
     * \brief creates a new AbstractSurface instance with options and parent
     * \param options the options the surface should have
     * \param parent the parent of the new surface
     * \return a new AbstarctSurface
     */
    virtual QSharedPointer<AbstractSurface> create(AbstractSurface::Options options, const QSharedPointer<AbstractSurface> &parent = QSharedPointer<AbstractSurface>()) = 0;
};

} // namespace Plugins
} // namespace Maliit

#endif // MALIIT_PLUGINS_ABSTRACTSURFACEFACTORY_H
