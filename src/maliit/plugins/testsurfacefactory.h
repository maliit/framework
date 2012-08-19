/* * This file is part of Maliit framework *
 *
 * Copyright (C) 2012 Openismus GmbH
 *
 * Contact: maliit-discuss@lists.maliit.org
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 */

#ifndef MALIIT_PLUGINS_TESTSURFACEFACTORY_H
#define MALIIT_PLUGINS_TESTSURFACEFACTORY_H

#include <maliit/plugins/abstractsurface.h>
#include <maliit/plugins/abstractwidgetssurface.h>

namespace Maliit {
namespace Plugins {

SharedSurface createTestSurface(AbstractSurface::Options options,
                                const SharedSurface &parent = SharedSurface());
SharedGraphicsViewSurface createTestGraphicsViewSurface(const SharedSurface &parent = SharedSurface());
SharedWidgetSurface createTestWidgetSurface(const SharedSurface &parent = SharedSurface());

}} // namespace Plugins, Maliit

#endif // MALIIT_PLUGINS_TESTSURFACEFACTORY_H
