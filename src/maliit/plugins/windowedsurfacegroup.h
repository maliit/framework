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

#ifndef MALIIT_PLUGINS_WINDOWEDSURFACEGROUP_H
#define MALIIT_PLUGINS_WINDOWEDSURFACEGROUP_H

#include <maliit/plugins/abstractsurfacegroup.h>
#include <maliit/plugins/windowedsurface.h>

namespace Maliit {
namespace Plugins {

class WindowedSurfaceGroup : public Maliit::Plugins::AbstractSurfaceGroup
{
public:
    WindowedSurfaceGroup();

    virtual AbstractSurfaceFactory *factory();

    virtual void activate();
    virtual void deactivate();

    virtual void setRotation(Maliit::OrientationAngle angle);

private:
    QScopedPointer<WindowedSurfaceFactory> mSurfaceFactory;
};

class WindowedSurfaceGroupFactory : public Maliit::Plugins::AbstractSurfaceGroupFactory
{
public:
    virtual QSharedPointer<AbstractSurfaceGroup> createSurfaceGroup();
};

} // namespace Server
} // namespace Maliit

#endif // MALIIT_PLUGINS_WINDOWEDSURFACEGROUP
