/* This file is part of Maliit framework
 *
 * Copyright (C) 2015 Canonical Ltd
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

#include "maliitbus.h"

#define ADDRESS_ENV "MALIIT_SERVER_ADDRESS"
#define ADDRESS_BUS_NAME "org.maliit.server"
#define ADDRESS_OBJECT_PATH "/org/maliit/server/address"
#define ADDRESS_INTERFACE "org.maliit.Server.Address"
#define ADDRESS_PROPERTY "address"
#define SERVER_OBJECT_PATH  "/com/meego/inputmethod/uiserver1"
#define CONTEXT_OBJECT_PATH "/com/meego/inputmethod/inputcontext"

static gchar *address;
static GDBusConnection *bus;
static MaliitServer *server;
static MaliitContext *context;

static const gchar *
maliit_get_address_sync (gboolean verbose)
{
  GDBusProxy *proxy;
  GVariant *property;
  GError *error = NULL;

  if (!address)
    {
      address = g_strdup (g_getenv (ADDRESS_ENV));

      if (!address)
        {
          proxy = g_dbus_proxy_new_for_bus_sync (G_BUS_TYPE_SESSION,
                                                 G_DBUS_PROXY_FLAGS_GET_INVALIDATED_PROPERTIES,
                                                 NULL,
                                                 ADDRESS_BUS_NAME,
                                                 ADDRESS_OBJECT_PATH,
                                                 ADDRESS_INTERFACE,
                                                 NULL,
                                                 &error);

          if (proxy)
            {
              property = g_dbus_proxy_get_cached_property (proxy, ADDRESS_PROPERTY);

              if (property)
                {
                  address = g_strdup (g_variant_get_string (property, NULL));

                  g_variant_unref (property);
                }
              else if (verbose)
                g_warning ("Unable to find address property");

              g_object_unref (proxy);
            }
          else if (verbose)
            {
              g_warning ("Unable to find bus address: %s", error->message);
              g_clear_error (&error);
            }
        }
    }

  return address;
}

static void
maliit_get_bus (GCancellable        *cancellable,
                GAsyncReadyCallback  callback,
                gpointer             user_data)
{
  g_return_if_fail (callback);

  if (!bus)
    g_dbus_connection_new_for_address (maliit_get_address_sync (TRUE),
                                       G_DBUS_CONNECTION_FLAGS_AUTHENTICATION_CLIENT,
                                       NULL,
                                       cancellable,
                                       callback,
                                       user_data);
  else
    callback (NULL, NULL, user_data);
}

static GDBusConnection *
maliit_get_bus_finish (GAsyncResult  *res,
                       GError       **error)
{
  if (!bus)
    bus = g_dbus_connection_new_for_address_finish (res, error);

  return bus;
}

static GDBusConnection *
maliit_get_bus_sync (GCancellable  *cancellable,
                     GError       **error)
{
  if (!bus)
    bus = g_dbus_connection_new_for_address_sync (maliit_get_address_sync (TRUE),
                                                  G_DBUS_CONNECTION_FLAGS_AUTHENTICATION_CLIENT,
                                                  NULL,
                                                  cancellable,
                                                  error);

  return bus;
}

void
maliit_set_bus (GDBusConnection *bus_)
{
  if (bus_ != bus)
    {
      g_clear_object (&context);
      g_clear_object (&server);
      g_clear_object (&bus);
      g_clear_pointer (&address, g_free);

      if (bus_)
        bus = g_object_ref (bus_);
    }
}

gboolean
maliit_is_running (void)
{
  gboolean running = FALSE;
  const gchar *address;
  GDBusConnection *bus;
  MaliitServer *server;

  address = maliit_get_address_sync (FALSE);

  if (address)
    {
      bus = maliit_get_bus_sync (NULL, NULL);

      if (bus)
        {
          server = maliit_server_proxy_new_sync (bus,
                                                 G_DBUS_PROXY_FLAGS_GET_INVALIDATED_PROPERTIES,
                                                 NULL,
                                                 SERVER_OBJECT_PATH,
                                                 NULL,
                                                 NULL);

          if (server)
            {
              running = TRUE;

              g_object_unref (server);
            }
        }
    }

  return running;
}

void
maliit_get_server (GCancellable        *cancellable,
                   GAsyncReadyCallback  callback,
                   gpointer             user_data)
{
  GDBusConnection *bus;
  GError *error = NULL;

  g_return_if_fail (callback);

  if (!server)
    {
      bus = maliit_get_bus_sync (cancellable, &error);

      if (bus)
        maliit_server_proxy_new (bus,
                                 G_DBUS_PROXY_FLAGS_GET_INVALIDATED_PROPERTIES,
                                 NULL,
                                 SERVER_OBJECT_PATH,
                                 cancellable,
                                 callback,
                                 user_data);
      else
        {
          g_warning ("Unable to connect to bus: %s", error->message);

          g_clear_error (&error);
        }
    }
  else
    callback (NULL, NULL, user_data);
}

MaliitServer *
maliit_get_server_finish (GAsyncResult  *res,
                          GError       **error)
{
  if (!server)
    server = maliit_server_proxy_new_finish (res, error);

  return server;
}

MaliitServer *
maliit_get_server_sync (GCancellable  *cancellable,
                        GError       **error)
{
  GDBusConnection *bus;

  if (!server)
    {
      bus = maliit_get_bus_sync (cancellable, error);

      if (bus)
        server = maliit_server_proxy_new_sync (bus,
                                               G_DBUS_PROXY_FLAGS_GET_INVALIDATED_PROPERTIES,
                                               NULL,
                                               SERVER_OBJECT_PATH,
                                               cancellable,
                                               error);
    }

  return server;
}

static gboolean
maliit_context_handle_plugin_settings_loaded (MaliitContext         *context,
                                              GDBusMethodInvocation *invocation,
                                              GVariant              *plugins     G_GNUC_UNUSED,
                                              gpointer               user_data   G_GNUC_UNUSED)
{
  maliit_context_complete_plugin_settings_loaded (context, invocation);

  return TRUE;
}

static gboolean
maliit_context_handle_update_input_method_area (MaliitContext         *context,
                                                GDBusMethodInvocation *invocation,
                                                gint                   x          G_GNUC_UNUSED,
                                                gint                   y          G_GNUC_UNUSED,
                                                gint                   width      G_GNUC_UNUSED,
                                                gint                   height     G_GNUC_UNUSED,
                                                gpointer               user_data  G_GNUC_UNUSED)
{
  maliit_context_complete_update_input_method_area (context, invocation);

  return TRUE;
}

void
maliit_get_context (GCancellable        *cancellable,
                    GAsyncReadyCallback  callback,
                    gpointer             user_data)
{
  g_return_if_fail (callback);

  maliit_get_bus (cancellable, callback, user_data);
}

MaliitContext *
maliit_get_context_finish (GAsyncResult  *res,
                           GError       **error)
{
  maliit_get_bus_finish (res, error);

  return maliit_get_context_sync (NULL, error);
}

MaliitContext *
maliit_get_context_sync (GCancellable  *cancellable,
                         GError       **error)
{
  GDBusConnection *bus;

  if (!context)
    {
      bus = maliit_get_bus_sync (cancellable, error);

      if (bus)
        {
          context = maliit_context_skeleton_new ();

          g_signal_connect_after (context,
                                  "handle-plugin-settings-loaded",
                                  G_CALLBACK (maliit_context_handle_plugin_settings_loaded),
                                  NULL);

          g_signal_connect_after (context,
                                  "handle-update-input-method-area",
                                  G_CALLBACK (maliit_context_handle_update_input_method_area),
                                  NULL);

          if (!g_dbus_interface_skeleton_export (G_DBUS_INTERFACE_SKELETON (context),
                                                 bus,
                                                 CONTEXT_OBJECT_PATH,
                                                 error))
            g_clear_object (&context);
        }
    }

  return context;
}
