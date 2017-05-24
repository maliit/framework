/* * This file is part of Maliit framework *
 *
 * Copyright (C) 2013 Openismus GmbH
 *
 * Contact: maliit-discuss@lists.maliit.org
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 */

#include <wayland-client.h>

#include <QRegion>
#include <QWindow>

#include "waylandplatform.h"
#include "windowdata.h"

namespace Maliit
{

class WaylandPlatformPrivate
{
public:
    WaylandPlatformPrivate() = default;
    ~WaylandPlatformPrivate() = default;
};

WaylandPlatform::WaylandPlatform()
    : d_ptr(new WaylandPlatformPrivate)
{}

void WaylandPlatform::setupInputPanel(QWindow* window,
                                      Maliit::Position position)
{
    Q_UNUSED(window)
    Q_UNUSED(position)
}

void WaylandPlatform::setInputRegion(QWindow* window,
                                     const QRegion& region)
{
    if (not window) {
        return;
    }

    window->setMask(region);
}

WaylandPlatform::~WaylandPlatform()
{

}

} // namespace Maliit
