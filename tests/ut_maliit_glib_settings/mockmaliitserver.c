/* This file is part of Maliit framework
 *
 * Copyright (C) 2012 Canonical Ltd
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

#include <unistd.h>
#include <gio/gunixinputstream.h>
#include <gio/gunixoutputstream.h>
#include <maliit-glib/maliitserver.h>
#include <maliit-glib/maliitcontext.h>

#define SERVER_OBJECT_PATH  "/com/meego/inputmethod/uiserver1"
#define CONTEXT_OBJECT_PATH "/com/meego/inputmethod/inputcontext"

struct _MockMaliitServerPriv {
    GDBusConnection *server_connection;
    GDBusConnection *client_connection;

    MaliitServer *server;
    MaliitContext *context;

    GThread *thread;
    GMainContext *thread_context;
    GMainLoop *thread_loop;
    GCond thread_cond;
    GMutex thread_mutex;
};

/* Record the call, and respond with a list of settings */
gboolean
load_plugin_settings (MaliitServer *server,
                      GDBusMethodInvocation *invocation,
                      const gchar *locale_name G_GNUC_UNUSED,
                      gpointer user_data)
{
    MockMaliitServer *self = user_data;

    self->load_plugin_settings_called = TRUE;

    maliit_server_complete_load_plugin_settings(server, invocation);
    return TRUE;
}

static gboolean
start_server(gpointer user_data)
{
    MockMaliitServer *self = user_data;

    g_main_context_push_thread_default(self->priv->thread_context);

    g_mutex_lock(&self->priv->thread_mutex);

    self->priv->server = maliit_server_skeleton_new();
    g_assert_true(g_dbus_interface_skeleton_export(G_DBUS_INTERFACE_SKELETON(self->priv->server),
                                                   self->priv->server_connection,
                                                   SERVER_OBJECT_PATH,
                                                   NULL));
    g_signal_connect(self->priv->server, "handle-load-plugin-settings", G_CALLBACK(load_plugin_settings), self);

    g_cond_signal(&self->priv->thread_cond);
    g_mutex_unlock(&self->priv->thread_mutex);

    g_main_context_pop_thread_default(self->priv->thread_context);

    return G_SOURCE_REMOVE;
}

static gpointer
run_server_thread(gpointer data)
{
    MockMaliitServer *self = data;

    g_main_context_push_thread_default(self->priv->thread_context);

    g_main_context_invoke(self->priv->thread_context, start_server, self);

    self->priv->thread_loop = g_main_loop_new(self->priv->thread_context, FALSE);
    g_main_loop_run(self->priv->thread_loop);
    g_main_loop_unref(self->priv->thread_loop);
    self->priv->thread_loop = NULL;

    g_main_context_pop_thread_default(self->priv->thread_context);

    return NULL;
}

MockMaliitServer *
mock_maliit_server_new()
{
    MockMaliitServer *self = g_new0(MockMaliitServer, 1);
    int server_to_client[2] = { 0 };
    int client_to_server[2] = { 0 };
    GInputStream *server_input_stream;
    GOutputStream *server_output_stream;
    GIOStream *server_stream;
    GInputStream *client_input_stream;
    GOutputStream *client_output_stream;
    GIOStream *client_stream;

    self->priv = g_new0(MockMaliitServerPriv, 1);

    g_assert_true(pipe(server_to_client) == 0);
    g_assert_true(pipe(client_to_server) == 0);

    server_input_stream = g_unix_input_stream_new(client_to_server[0], TRUE);
    server_output_stream = g_unix_output_stream_new(server_to_client[1], TRUE);
    server_stream = g_simple_io_stream_new(server_input_stream, server_output_stream);
    g_object_unref(server_output_stream);
    g_object_unref(server_input_stream);
    self->priv->server_connection = g_dbus_connection_new_sync(server_stream,
                                                               NULL,
                                                               G_DBUS_CONNECTION_FLAGS_NONE,
                                                               NULL,
                                                               NULL,
                                                               NULL);
    g_object_unref(server_stream);
    g_assert_nonnull(self->priv->server_connection);

    client_input_stream = g_unix_input_stream_new(server_to_client[0], TRUE);
    client_output_stream = g_unix_output_stream_new(client_to_server[1], TRUE);
    client_stream = g_simple_io_stream_new(client_input_stream, client_output_stream);
    g_object_unref(client_output_stream);
    g_object_unref(client_input_stream);
    self->priv->client_connection = g_dbus_connection_new_sync(client_stream,
                                                               NULL,
                                                               G_DBUS_CONNECTION_FLAGS_NONE,
                                                               NULL,
                                                               NULL,
                                                               NULL);
    g_object_unref(client_stream);
    g_assert_nonnull(self->priv->client_connection);

    self->priv->thread_context = g_main_context_new();
    self->priv->thread = g_thread_new(NULL, run_server_thread, self);

    g_mutex_lock(&self->priv->thread_mutex);

    while (!self->priv->server) {
        g_cond_wait(&self->priv->thread_cond, &self->priv->thread_mutex);
    }

    g_mutex_unlock(&self->priv->thread_mutex);

    return self;
}

void
mock_maliit_server_free(MockMaliitServer *self)
{
    g_clear_object(&self->priv->context);
    g_clear_object(&self->priv->server);
    g_clear_object(&self->priv->client_connection);
    g_clear_object(&self->priv->server_connection);
    g_free(self->priv);
    g_free(self);
}

GDBusConnection *
mock_maliit_server_get_bus(MockMaliitServer *self)
{
    return self->priv->client_connection;
}
