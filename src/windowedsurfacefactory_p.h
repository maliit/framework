/*
 * This file is part of Maliit framework *
 *
 * Copyright (C) 2012 Intel Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 *
 * Contact: maliit-discuss@lists.maliit.org
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 */

#ifndef MALIIT_SERVER_WINDOWED_SURFACE_FACTORY_P_H
#define MALIIT_SERVER_WINDOWED_SURFACE_FACTORY_P_H

#include <QObject>
#include <QWeakPointer>

#include <vector>

#ifdef HAVE_WAYLAND
#include <wayland-client.h>
#include "wayland-desktop-shell-client-protocol.h"
#endif

namespace Maliit {
namespace Server {

class WindowedSurface;
class WindowedSurfaceFactory;

class WindowedSurfaceFactoryPrivate : public QObject
{
    Q_OBJECT
    Q_DECLARE_PUBLIC(WindowedSurfaceFactory)

public:
    WindowedSurfaceFactoryPrivate(WindowedSurfaceFactory *factory);

#ifdef HAVE_WAYLAND
    void handleRegistryGlobal(uint32_t name,
                              const char *interface,
                              uint32_t version);
    void handleRegistryGlobalRemove(uint32_t name);
    void handleOutputGeometry(int32_t x,
                              int32_t y,
                              int32_t physical_width,
                              int32_t physical_height,
                              int32_t subpixel,
                              const char *make,
                              const char *model,
                              int32_t transform);
    void handleOutputMode(uint32_t flags,
                          int32_t width,
                          int32_t height,
                          int32_t refresh);
#endif

    Q_SLOT void screenResized(int screen);

    WindowedSurfaceFactory *q_ptr;
    std::vector<QWeakPointer<WindowedSurface> > surfaces;
    bool active;
#ifdef HAVE_WAYLAND
    struct wl_registry *registry;
    struct wl_output *output;
    struct input_panel *panel;
    bool output_configured;
#endif
};

} // namespace Server
} // namespace Maliit

#endif // MALIIT_SERVER_WINDOWED_SURFACE_FACTORY_P_H
