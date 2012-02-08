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

class AbstractSurfaceFactory
{
public:
    virtual ~AbstractSurfaceFactory();

    virtual QSize screenLandscapeSize() const;
    virtual QSize screenPortraitSize() const;

    virtual bool supported(AbstractSurface::Options options) const = 0;
    virtual QSharedPointer<AbstractSurface> create(AbstractSurface::Options options, const QSharedPointer<AbstractSurface> &parent = QSharedPointer<AbstractSurface>()) = 0;
};

} // namespace Plugins
} // namespace Maliit

#endif // MALIIT_PLUGINS_ABSTRACTSURFACEFACTORY_H
