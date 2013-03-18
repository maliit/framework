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

#include "unknownplatform.h"

namespace Maliit
{

void UnknownPlatform::setupInputPanel(QWindow* window,
                                      Maliit::Position position)
{
    Q_UNUSED(window);
    Q_UNUSED(position);
    // nothing to do.
}

void UnknownPlatform::setInputRegion(QWindow* window,
                                     const QRegion& region)
{
    Q_UNUSED(window);
    Q_UNUSED(region);
    // nothing to do.
}

} // namespace Maliit
