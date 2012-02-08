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

#ifndef MALIIT_PLUGINS_ABSTRACTSURFACEGROUP_H
#define MALIIT_PLUGINS_ABSTRACTSURFACEGROUP_H

#include "abstractsurface.h"
#include "maliit/namespace.h"

namespace Maliit {
namespace Plugins {

class AbstractSurfaceGroup
{
public:
    virtual AbstractSurfaceFactory *factory() = 0;

    virtual void activate() = 0;
    virtual void deactivate() = 0;

    virtual void setRotation(Maliit::OrientationAngle angle) = 0;
};

class AbstractSurfaceGroupFactory
{
public:
    virtual ~AbstractSurfaceGroupFactory();

    virtual QSharedPointer<AbstractSurfaceGroup> createSurfaceGroup() = 0;
};

}
}

#endif // MALIIT_PLUGINS_ABSTRACTSURFACEGROUP_H
