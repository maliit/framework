/* This file is part of Maliit framework
 *
 * Copyright (C) 2012 Openismus GmbH
 *
 * Contact: maliit-discuss@lists.maliit.org
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

#include "mockmaliitserver.h"

#include <connection-glib/meego-im-connector.h>
#include <connection-glib/meego-im-connector-private.h>
#include <connection-glib/meego-imcontext-dbus-private.h>

static MockMaliitServer *global_server_instance = NULL;

struct _MockMaliitServerPriv {
    MeegoImConnector *connector;
};

/* Record the call, and respond with a list of settings */
gboolean
load_plugin_settings (MeegoIMProxy *proxy G_GNUC_UNUSED,
                      const gchar *locale_name G_GNUC_UNUSED)
{
    MockMaliitServer *self = global_server_instance;

    self->load_plugin_settings_called = TRUE;
    meego_imcontext_dbus_plugin_settings_loaded(self->priv->connector->dbusobj, self->settings, NULL);
    return TRUE;
}

MockMaliitServer *
mock_maliit_server_new()
{
    MockMaliitServer *self = g_new(MockMaliitServer, 1);
    self->priv = g_new(MockMaliitServerPriv, 1);

    self->load_plugin_settings_called = FALSE;
    self->priv->connector = meego_im_connector_new();
    self->priv->connector->try_reconnect = FALSE;
    meego_im_connector_set_singleton(self->priv->connector);
    self->priv->connector->proxy->load_plugin_settings_observer = load_plugin_settings;
    self->settings = NULL;

    g_assert(!global_server_instance); // XXX: cannot use multiple instances at the same time
    global_server_instance = self;
    return self;
}

void
mock_maliit_server_free(MockMaliitServer *self)
{
    meego_im_connector_free(self->priv->connector);
    meego_im_connector_set_singleton(NULL);

    g_free(self->priv);
    g_free(self);

    g_assert(global_server_instance == self); // XXX: cannot use multiple instances at the same time
    global_server_instance = NULL;
}

/* Fake the MeegoIMProxy::connection-established signal that connecting to maliit-server will emit */
void
mock_maliit_server_emulate_connect(MockMaliitServer *self)
{
    g_signal_emit_by_name(self->priv->connector->proxy, "connection-established");
}

/* Fake the MeegoIMProxy::connection-dropped signal that disconnecting to maliit-server will emit */
void
mock_maliit_server_emulate_disconnect(MockMaliitServer *self)
{
    g_signal_emit_by_name(self->priv->connector->proxy, "connection-dropped");
}
