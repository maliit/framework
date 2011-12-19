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

#include "inputcontextdbusaddress.h"

#ifdef MALIIT_USE_GIO_API
#include <gio/gio.h>
#else
#include <glib-object.h>
#include <dbus/dbus-glib.h>
#endif
#include <tr1/memory>
#include <tr1/functional>
#include <QDebug>

namespace {
const char * const MaliitServerName = "org.maliit.server";
const char * const MaliitServerObjectPath = "/org/maliit/server/address";
const char * const MaliitServerInterface = "org.maliit.Server.Address";
const char * const MaliitServerAddressProperty = "address";

#ifndef MALIIT_USE_GIO_API
const char * const DBusPropertiesInterface = "org.freedesktop.DBus.Properties";
const char * const DBusPropertiesGetMethod = "Get";
#endif

    struct SafeUnref
    {
#ifdef MALIIT_USE_GIO_API
        static inline void cleanup(GDBusProxy *pointer)
        {
            g_object_unref(pointer);
        }
        static inline void cleanup(GVariant *pointer)
        {
            g_variant_unref(pointer);
        }
#else
        static inline void cleanup(DBusGConnection *pointer)
        {
            dbus_g_connection_unref(pointer);
        }
        static inline void cleanup(DBusGProxy *pointer)
        {
            g_object_unref(pointer);
        }
#endif
    };
}

namespace Maliit {
namespace InputContext {
namespace DBus {

Address::Address()
{
}

#ifdef MALIIT_USE_GIO_API
const std::string Address::get() const
{
    GError *error = NULL;

    QScopedPointer<GDBusProxy, SafeUnref>
            proxy(g_dbus_proxy_new_for_bus_sync(G_BUS_TYPE_SESSION,
                                                G_DBUS_PROXY_FLAGS_NONE,
                                                0,
                                                MaliitServerName,
                                                MaliitServerObjectPath,
                                                MaliitServerInterface,
                                                0, &error));

    if (proxy.isNull()) {
        qWarning() << __PRETTY_FUNCTION__ << error->message;
        g_error_free(error);
        return std::string();
    }

    QScopedPointer<GVariant, SafeUnref>
            address(g_dbus_proxy_get_cached_property(proxy.data(),
                                                     MaliitServerAddressProperty));

    if (address.isNull()) {
        return std::string();
    }

    std::string result(g_variant_get_string(address.data(), 0));

    return result;
}
#else
const std::string Address::get() const
{
    GValue value = {0,};
    GError *error = NULL;
    
    QScopedPointer<DBusGConnection, SafeUnref>
            connection(dbus_g_bus_get(DBUS_BUS_SESSION, &error));

    if (connection.isNull()) {
        qWarning() << __PRETTY_FUNCTION__ << error->message;
        g_error_free(error);
        return std::string();
    }

    QScopedPointer<DBusGProxy, SafeUnref>
            proxy(dbus_g_proxy_new_for_name(connection.data(),
                                            MaliitServerName,
                                            MaliitServerObjectPath,
                                            DBusPropertiesInterface));

    if (!dbus_g_proxy_call(proxy.data(),
                           DBusPropertiesGetMethod,
                           &error,
                           G_TYPE_STRING, MaliitServerInterface,
                           G_TYPE_STRING, MaliitServerAddressProperty,
                           G_TYPE_INVALID,
                           G_TYPE_VALUE, &value, G_TYPE_INVALID)) {

        qWarning() << __PRETTY_FUNCTION__ << error->message;
        g_error_free(error);
        return std::string();
    }

    std::string result(g_value_get_string(&value));
    g_value_unset(&value);

    return result;
}
#endif
} // namespace DBus
} // namespace InputContext
} // namespace Maliit
