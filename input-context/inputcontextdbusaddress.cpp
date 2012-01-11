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

#include <gio/gio.h>
#include <tr1/memory>
#include <tr1/functional>
#include <QDebug>

namespace {
    const char * const MaliitServerName = "org.maliit.server";
    const char * const MaliitServerObjectPath = "/org/maliit/server/address";
    const char * const MaliitServerInterface = "org.maliit.Server.Address";
    const char * const MaliitServerAddressProperty = "address";

    template<typename T>
    class SafeUnref {
    public:
        explicit SafeUnref(std::tr1::function<void (T*)>unref)
            : mUnref(unref)
        {}

        void operator()(T* t)
        {
            if (t)
                mUnref(t);
        }

    private:
        std::tr1::function<void (T*)> mUnref;
    };
}

namespace Maliit {
namespace InputContext {
namespace DBus {

Address::Address()
{
}

const std::string Address::get() const
{
    GDBusProxyFlags flags = G_DBUS_PROXY_FLAGS_NONE;

#if defined(NO_SERVER_DBUS_ACTIVATION)
    flags = G_DBUS_PROXY_FLAGS_DO_NOT_AUTO_START;
#endif

    GError *error = NULL;
    std::tr1::shared_ptr<GDBusProxy> proxy(g_dbus_proxy_new_for_bus_sync(G_BUS_TYPE_SESSION,
                                                                         flags,
                                                                         0,
                                                                         MaliitServerName,
                                                                         MaliitServerObjectPath,
                                                                         MaliitServerInterface,
                                                                         0, &error),
                                           SafeUnref<GDBusProxy>(g_object_unref));

    if (!proxy) {
        qWarning() << __PRETTY_FUNCTION__ << error->message;
        g_error_free(error);
        return std::string();
    }

    std::tr1::shared_ptr<GVariant> address(g_dbus_proxy_get_cached_property(proxy.get(), MaliitServerAddressProperty),
                                           SafeUnref<GVariant>(g_variant_unref));

    if (!address) {
        return std::string();
    }

    std::string result(g_variant_get_string(address.get(), 0));

    return result;
}

} // namespace DBus
} // namespace InputContext
} // namespace Maliit
