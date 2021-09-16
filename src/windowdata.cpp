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

#include "windowdata.h"

namespace Maliit
{

WindowData::WindowData()
    : m_window(),
      m_position(Maliit::PositionCenterBottom)
{}

WindowData::WindowData(QWindow *window, Maliit::Position position)
    : m_window(window),
      m_position(position)
{}

} // namespace Maliit
