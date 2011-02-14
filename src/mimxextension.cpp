/* * This file is part of meego-im-framework *
 *
 * Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * Contact: Nokia Corporation (directui@nokia.com)
 *
 * If you have questions regarding the use of this file, please contact
 * Nokia at directui@nokia.com.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 */

#include <QDebug>
#include <QX11Info>

#include "mimxextension.h"

#include <X11/extensions/Xcomposite.h>
#include <X11/extensions/Xdamage.h>

MImXExtension::MImXExtension(const char *name)
    : available(false),
      major_code(0),
      event_base(0),
      error_base(0)
{
    available = XQueryExtension(QX11Info::display(), name, &major_code, &event_base, &error_base);
}

bool MImXExtension::supported(int required_major, int required_minor) const
{
    if (!available)
        return false;

    int major = required_major, minor = required_minor;
    if (!queryVersion(&major, &minor))
        return false;

    if (required_major == 0 && required_minor == 0)
        return true;

    if (major < required_major || (major == required_major && minor < required_minor))
        return false;

    return true;
}

MImXCompositeExtension::MImXCompositeExtension()
    : MImXExtension(COMPOSITE_NAME)
{
}

bool MImXCompositeExtension::queryVersion(int *major, int *minor) const
{
    return XCompositeQueryVersion(QX11Info::display(), major, minor) != 0;
}

MImXDamageExtension::MImXDamageExtension()
    : MImXExtension(DAMAGE_NAME)
{
}

bool MImXDamageExtension::queryVersion(int *major, int *minor) const
{
    return XDamageQueryVersion(QX11Info::display(), major, minor) != 0;
}
