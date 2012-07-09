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

#include "meego-im-connector.h"

#include <glib.h>
#include <gio/gio.h>

#include <dbus/dbus.h>
#include <dbus/dbus-glib.h>
#include <dbus/dbus-glib-lowlevel.h>

#define MALIIT_SERVER_NAME "org.maliit.server"
#define MALIIT_SERVER_OBJECT_PATH "/org/maliit/server/address"
#define MALIIT_SERVER_INTERFACE "org.maliit.Server.Address"
#define MALIIT_SERVER_ADDRESS_PROPERTY "address"

#ifndef MALIIT_USE_GIO_API
#define DBUS_PROPERTIES_INTERFACE "org.freedesktop.DBus.Properties"
#define DBUS_PROPERTIES_GET_METHOD "Get"
#endif

#define MALIIT_SERVER_ADDRESS_ENV "MALIIT_SERVER_ADDRESS"

/* For glib < 2.30 */
#ifndef G_VALUE_INIT
#define G_VALUE_INIT { 0, { { 0 } } }
#endif

struct _MeegoImConnectorPrivate {
    DBusGConnection *connection;
};

MeegoImConnector *meego_im_connector_new();

static gboolean
try_reconnect(MeegoImConnector *connector)
{
    if (connector->try_reconnect) {
        meego_im_connector_run(connector);
    }
    return FALSE; // _run is responsible for setting up a new timeout if needed
}


static void
connection_dropped(gpointer instance, MeegoImConnector *connector)
{
    if (connector->priv->connection) {
        dbus_g_connection_unref(connector->priv->connection);
    }
    try_reconnect(connector);
}

#ifdef MALIIT_USE_GIO_API
static char *
get_dbus_address()
{
    const char *overridden_address = g_getenv(MALIIT_SERVER_ADDRESS_ENV);

    if (overridden_address && *overridden_address) {
        return g_strdup(overridden_address);
    }

    GDBusProxyFlags flags = G_DBUS_PROXY_FLAGS_NONE;

#if defined(NO_DBUS_ACTIVATION)
    flags = G_DBUS_PROXY_FLAGS_DO_NOT_AUTO_START;
#endif

    GDBusProxy *proxy = g_dbus_proxy_new_for_bus_sync(G_BUS_TYPE_SESSION,
                                                      flags,
                                                      0,
                                                      MALIIT_SERVER_NAME,
                                                      MALIIT_SERVER_OBJECT_PATH,
                                                      MALIIT_SERVER_INTERFACE,
                                                      0, 0);


    if (!proxy)
        return 0;

    GVariant *variant = g_dbus_proxy_get_cached_property(proxy, MALIIT_SERVER_ADDRESS_PROPERTY);

    if (!variant) {
        g_object_unref(proxy);
        return 0;
    }

    char* address = g_strdup(g_variant_get_string(variant, 0));

    g_variant_unref(variant);
    g_object_unref(proxy);

    return address;
}

#else
static char *
get_dbus_address()
{
    const char *overridden_address = g_getenv(MALIIT_SERVER_ADDRESS_ENV);

    if (overridden_address && *overridden_address) {
        return g_strdup(overridden_address);
    }

    GValue value = G_VALUE_INIT;
    GError *error = NULL;
    DBusGConnection *connection = dbus_g_bus_get(DBUS_BUS_SESSION, &error);

    if (!connection) {
        g_warning("%s: %s", __PRETTY_FUNCTION__, error->message);
        g_error_free(error);
        return NULL;
    }

    DBusGProxy *proxy = dbus_g_proxy_new_for_name(connection,
                                            MALIIT_SERVER_NAME,
                                            MALIIT_SERVER_OBJECT_PATH,
                                            DBUS_PROPERTIES_INTERFACE);

    if (!dbus_g_proxy_call(proxy,
                           DBUS_PROPERTIES_GET_METHOD,
                           &error,
                           G_TYPE_STRING, MALIIT_SERVER_INTERFACE,
                           G_TYPE_STRING, MALIIT_SERVER_ADDRESS_PROPERTY,
                           G_TYPE_INVALID,
                           G_TYPE_VALUE, &value, G_TYPE_INVALID)) {

        g_warning("%s: %s", __PRETTY_FUNCTION__, error->message);
        g_error_free(error);
        return NULL;
    }

    gchar *result = g_value_dup_string(&value);
    g_value_unset(&value);
    g_object_unref(proxy);

    return result;
}
#endif

/**
 * MeegoImConnector:
 *
 * Connects and maintains the DBusConnection, and the objects
 * that depend on it. Makes sure that MeegoIMProxy and MeegoIMDbusObj
 * has the correct DBusConnection by calling _connect on them when
 * the connection has changed.
 *
 * MeegoIMProxy is responsible for letting the connector know that the
 * connection was dropped by emitting the "connection-dropped signal".
 */
MeegoImConnector *
meego_im_connector_new()
{
    MeegoImConnector *self = g_new(MeegoImConnector, 1);
    self->priv = g_new(MeegoImConnectorPrivate, 1);

    self->priv->connection = NULL;
    self->dbusobj = meego_imcontext_dbusobj_new();
    self->proxy = meego_im_proxy_new();
    self->try_reconnect = TRUE;

    g_signal_connect(self->proxy, "connection-dropped",
                     G_CALLBACK(connection_dropped), (gpointer)self);

    return self;
}

void
meego_im_connector_free(MeegoImConnector *self)
{
    if (self->priv->connection) {
        dbus_g_connection_unref(self->priv->connection);
    }
    g_object_unref(self->dbusobj);
    g_object_unref(self->proxy);

    g_free(self->priv);
    g_free(self);
}


static MeegoImConnector *meego_im_connector_singleton = NULL;

void
meego_im_connector_set_singleton(MeegoImConnector *self)
{
    meego_im_connector_singleton = self;
}

MeegoImConnector *
meego_im_connector_get_singleton()
{
    MeegoImConnector *connector = meego_im_connector_singleton;
    if (!connector) {
        connector = meego_im_connector_new();
        meego_im_connector_set_singleton(connector);
        meego_im_connector_run(connector);
    }
    return connector;
}

void
meego_im_connector_run(MeegoImConnector *self)
{
    DBusGConnection *connection = NULL;
    DBusConnection *dbus_connection;
    DBusError error;
    char *address = NULL;

    g_return_if_fail(self != NULL);

    address = get_dbus_address();
    if (!address) {
        g_warning("Couldn't connect to Maliit server. Retrying...");

        g_timeout_add_seconds(2, (GSourceFunc)try_reconnect, self);
        return;
    }

    dbus_error_init(&error);

    // Input contexts should not share the connection to the maliit server with
    // each other (even not when they are in the same application). Therefore,
    // use private connection for IC to server connection.
    dbus_connection = dbus_connection_open_private(address, &error);
    g_free(address);

    if (!dbus_connection) {
        g_warning("Couldn't connect to Maliit server: %s. Retrying...", error.message);

        dbus_error_free(&error);

        g_timeout_add_seconds(2, (GSourceFunc)try_reconnect, self);
        return;
    }

    dbus_connection_setup_with_g_main(dbus_connection, NULL);
    connection = dbus_connection_get_g_connection(dbus_connection);

    self->priv->connection = connection;

    meego_im_proxy_connect(self->proxy, (gpointer)self->priv->connection);
    meego_imcontext_dbusobj_connect(self->dbusobj, (gpointer)self->priv->connection);
}
