/* * This file is part of Maliit framework *
 *
 * Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#include <QX11Info>

#include "mimxerrortrap.h"

static MImXErrorTrap *current_trap = 0;

int mim_x_error_handler(Display *display, XErrorEvent *e)
{
    if (current_trap == 0)
        return 0;

    if (current_trap->matches(e)) {
        current_trap->error_code = e->error_code;
        return 0;
    }

    return current_trap->old_handler(display, e);
}

MImXErrorTrap::MImXErrorTrap(int majorCode, int minorCode)
    : major_code(majorCode),
      minor_code(minorCode),
      error_code(Success),
      old_handler(0)
{
    old_handler = XSetErrorHandler(mim_x_error_handler);
    current_trap = this;
}

MImXErrorTrap::MImXErrorTrap(const MImXExtension &extension, int minorCode)
    : major_code(extension.majorCode()),
      minor_code(minorCode),
      error_code(Success),
      old_handler(0)
{
    old_handler = XSetErrorHandler(mim_x_error_handler);
    current_trap = this;
}

MImXErrorTrap::~MImXErrorTrap()
{
    if (current_trap == this)
        untrap();
}

int MImXErrorTrap::untrap(bool sync)
{
    if (sync)
        XSync(QX11Info::display(), false);

    if (current_trap != this)
        return Success;

    XSetErrorHandler(old_handler);

    current_trap = 0;

    return error_code;
}

bool MImXErrorTrap::matches(XErrorEvent *e)
{
    return e->request_code == major_code &&
           e->minor_code == minor_code;
}
