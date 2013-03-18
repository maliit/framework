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

#ifndef MALIIT_WINDOW_DATA_H
#define MALIIT_WINDOW_DATA_H

#include <QPointer>
#include <QWindow>

#include <maliit/namespace.h>

namespace Maliit
{

struct WindowData
{
    // apparently QVector<WindowData>::append() needs it...
    WindowData();
    WindowData(QWindow *window, Maliit::Position position);

    QPointer<QWindow> m_window;
    Maliit::Position m_position;
};

} // namespace Maliit

#endif // MALIIT_WINDOW_DATA_H
