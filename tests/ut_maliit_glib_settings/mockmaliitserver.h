/* This file is part of Maliit framework
 *
 * Copyright (C) 2012 Canonical Ltd
 *
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the licence, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef MOCKMALIITSERVER_H
#define MOCKMALIITSERVER_H

#include <glib.h>
#include <gio/gio.h>

typedef struct _MockMaliitServerPriv MockMaliitServerPriv;

typedef struct {
    gboolean load_plugin_settings_called;
    MockMaliitServerPriv *priv;
    GVariant *settings; /* To be set by tests */
} MockMaliitServer;

MockMaliitServer *mock_maliit_server_new();
void mock_maliit_server_free(MockMaliitServer *self);

GDBusConnection *mock_maliit_server_get_bus(MockMaliitServer *server);

#endif // MOCKMALIITSERVER_H
