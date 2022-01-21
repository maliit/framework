/* * This file is part of Maliit framework *
 *
 * Copyright (C) 2013 Openismus GmbH
 *
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 */

#include <QtGui/QGuiApplication>
#include "abstractplatform.h"

#include "unknownplatform.h"
#ifdef HAVE_XCB
#include "xcbplatform.h"
#endif
#ifdef HAVE_WAYLAND
#include "waylandplatform.h"
#endif

namespace Maliit
{

AbstractPlatform::~AbstractPlatform()
{}

void AbstractPlatform::setApplicationWindow(QWindow *window, WId appWindowId)
{
    Q_UNUSED(window)
    Q_UNUSED(appWindowId)
}

std::unique_ptr<AbstractPlatform> createPlatform()
{
#ifdef HAVE_WAYLAND
    if (QGuiApplication::platformName().startsWith("wayland")) {
        return std::unique_ptr<AbstractPlatform>(new Maliit::WaylandPlatform);
    }
#endif
#ifdef HAVE_XCB
    if (QGuiApplication::platformName() == "xcb") {
        return std::unique_ptr<AbstractPlatform>(new Maliit::XCBPlatform);
    }
#endif
    return std::unique_ptr<AbstractPlatform>(new Maliit::UnknownPlatform);
}

} // namespace Maliit
