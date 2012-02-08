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

#include "windowedsurfacegroup.h"

#include "abstractsurfacegroup.h"
#include "windowedsurface.h"

namespace Maliit {
namespace Server {

class WindowedSurfaceGroup : public AbstractSurfaceGroup
{
public:
    WindowedSurfaceGroup();

    virtual Maliit::Plugins::AbstractSurfaceFactory *factory();

    virtual void activate();
    virtual void deactivate();

    virtual void setRotation(Maliit::OrientationAngle angle);

private:
    QScopedPointer<WindowedSurfaceFactory> mSurfaceFactory;
};

WindowedSurfaceGroup::WindowedSurfaceGroup()
    : AbstractSurfaceGroup(),
      mSurfaceFactory(new WindowedSurfaceFactory)
{
}

Maliit::Plugins::AbstractSurfaceFactory *WindowedSurfaceGroup::factory()
{
    return mSurfaceFactory.data();
}

void WindowedSurfaceGroup::activate()
{
    mSurfaceFactory->activate();
}

void WindowedSurfaceGroup::deactivate()
{
    mSurfaceFactory->deactivate();
}

void WindowedSurfaceGroup::setRotation(Maliit::OrientationAngle)
{
}

QSharedPointer<AbstractSurfaceGroup> WindowedSurfaceGroupFactory::createSurfaceGroup()
{
    return QSharedPointer<AbstractSurfaceGroup>(new WindowedSurfaceGroup);
}


} // namespace Plugins
} // namespace Maliit
