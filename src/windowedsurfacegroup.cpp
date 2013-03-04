/* * This file is part of Maliit framework *
 *
 * Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * Copyright (C) 2012 Canonical Ltd
 *
 * Contact: maliit-discuss@lists.maliit.org
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
    WindowedSurfaceGroup(WindowedSurfaceFactory *surfaceFactory);

    virtual Maliit::Plugins::AbstractSurfaceFactory *factory();

    virtual void activate();
    virtual void deactivate();

    virtual void setRotation(Maliit::OrientationAngle angle);

    void applicationFocusChanged(WId winId);

private:
    QScopedPointer<WindowedSurfaceFactory> mSurfaceFactory;
};

WindowedSurfaceGroup::WindowedSurfaceGroup(WindowedSurfaceFactory *surfaceFactory)
    : AbstractSurfaceGroup(),
      mSurfaceFactory(surfaceFactory)
{
    connect(mSurfaceFactory.data(), SIGNAL(inputMethodAreaChanged(QRegion)),
            this, SIGNAL(inputMethodAreaChanged(QRegion)));
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

void WindowedSurfaceGroup::applicationFocusChanged(WId winId)
{
    mSurfaceFactory->applicationFocusChanged(winId);
}

QSharedPointer<AbstractSurfaceGroup> WindowedSurfaceGroupFactory::createSurfaceGroup()
{
    WindowedSurfaceFactory *factory = new WindowedSurfaceFactory;

    connect(factory, SIGNAL(surfaceWidgetCreated(QWidget*,int)),
            this, SIGNAL(surfaceWidgetCreated(QWidget*,int)));

    QSharedPointer<WindowedSurfaceGroup> group(new WindowedSurfaceGroup(factory));
    mGroups.push_back(group);

    return group;
}

void WindowedSurfaceGroupFactory::applicationFocusChanged(WId winId)
{
    Q_FOREACH(QWeakPointer<WindowedSurfaceGroup> weakGroup, mGroups) {
        QSharedPointer<WindowedSurfaceGroup> group = weakGroup.toStrongRef();
        if (group) {
            group->applicationFocusChanged(winId);
        }
    }
}

} // namespace Plugins
} // namespace Maliit
