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

namespace Maliit {
namespace Server {

class WindowedSurface;
class WindowedSurfaceFactory;

class WindowedSurfaceFactoryPrivate : public QObject
{
    Q_OBJECT
    Q_DECLARE_PUBLIC(WindowedSurfaceFactory);

public:
    WindowedSurfaceFactoryPrivate(WindowedSurfaceFactory *factory);

    Q_SLOT void screenResized(int screen);

    WindowedSurfaceFactory *q_ptr;
    std::vector<QWeakPointer<WindowedSurface> > surfaces;
    bool active;
};

} // namespace Server
} // namespace Maliit

#endif // MALIIT_SERVER_WINDOWED_SURFACE_FACTORY_P_H
