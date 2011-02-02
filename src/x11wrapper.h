/* * This file is part of meego-im-framework *
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef MIM_X11WRAPPER_H
#define MIM_X11WRAPPER_H

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h> // defines IconicState
#include <X11/extensions/Xdamage.h>
#include <X11/extensions/Xcomposite.h>

class X11Wrapper {
public:
    X11Wrapper();

    Window remoteWindowId;

    Pixmap remoteWindowXPixmap;
    QPixmap remoteWindowPixmap;
 
    Damage pixmapDamage;
};

class MIMXError {
public:
    MIMXError();

    void check(int major, int minor);
    bool matches(XErrorEvent *e);
    int result();

    int request_code;
    int minor_code;
    int error_code;
    XErrorHandler oldHandler;
};

#endif
