/* * This file is part of Maliit framework *
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "meego-im-proxy.h"
#include "meego-imcontext-dbus.h"

#include <dbus/dbus-glib.h>

typedef struct {
    MeegoIMProxy *proxy;
    MeegoIMContextDbusObj *dbusobj;
    DBusGConnection *connection;
} MeegoImConnector;

void meego_im_connector_run(MeegoImConnector *self);
MeegoImConnector *meego_im_connector_get_singleton();
