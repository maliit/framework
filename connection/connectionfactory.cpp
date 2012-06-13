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

#include "connectionfactory.h"

#ifndef MALIIT_DISABLE_DBUS
#ifdef HAVE_GLIB_DBUS
#include "glibdbusimserverproxy.h"
#include "minputcontextglibdbusconnection.h"
#else
#include "dbusserverconnection.h"
#include "dbusinputcontextconnection.h"
#endif
#endif
#include "mimdirectserverconnection.h"

namespace Maliit {
#ifndef MALIIT_DISABLE_DBUS
namespace DBus {

MImServerConnection *createServerConnectionWithDynamicAddress()
{
    const QSharedPointer<Maliit::InputContext::DBus::Address> address(new Maliit::InputContext::DBus::DynamicAddress);
#ifdef HAVE_GLIB_DBUS
    return new GlibDBusIMServerProxy(address);
#else
    return new DBusServerConnection(address);
#endif
}

MImServerConnection *createServerConnectionWithFixedAddress(const QString &fixedAddress)
{
    const QSharedPointer<Maliit::InputContext::DBus::Address> address(new Maliit::InputContext::DBus::FixedAddress(fixedAddress));
#ifdef HAVE_GLIB_DBUS
    return new GlibDBusIMServerProxy(address);
#else
    return new DBusServerConnection(address);
#endif
}

MInputContextConnection *createInputContextConnectionWithDynamicAddress()
{
#ifdef HAVE_GLIB_DBUS
    std::tr1::shared_ptr<Maliit::Server::DBus::Address> address(new Maliit::Server::DBus::DynamicAddress);
    return new MInputContextGlibDBusConnection(address, false);
#else
    QSharedPointer<Maliit::Server::DBus::Address> address(new Maliit::Server::DBus::DynamicAddress);
    return new DBusInputContextConnection(address);
#endif
}

MInputContextConnection *createInputContextConnectionWithFixedAddress(const QString &fixedAddress, bool allowAnonymous)
{
#ifdef HAVE_GLIB_DBUS
    std::tr1::shared_ptr<Maliit::Server::DBus::Address> address(new Maliit::Server::DBus::FixedAddress(fixedAddress));
    return new MInputContextGlibDBusConnection(address, allowAnonymous);
#else
    Q_UNUSED(allowAnonymous);
    QSharedPointer<Maliit::Server::DBus::Address> address(new Maliit::Server::DBus::FixedAddress(fixedAddress));
    return new DBusInputContextConnection(address);
#endif
}

} // namespace DBus
#endif // MALIIT_DISABLE_DBUS

QSharedPointer<MImServerConnection> createServerConnection(const QString &connectionType)
{
    typedef QSharedPointer<MImServerConnection> ConnectionPtr;
    static QWeakPointer<MImServerConnection> cached_connection;

    if (ConnectionPtr connection = cached_connection.toStrongRef())
        return connection;

    if (connectionType == MALIIT_INPUTCONTEXT_NAME) {
#ifndef MALIIT_DISABLE_DBUS
        const QByteArray overriddenAddress = qgetenv("MALIIT_SERVER_ADDRESS");
        ConnectionPtr connection;

        if (overriddenAddress.isEmpty()) {
            cached_connection = connection = ConnectionPtr(Maliit::DBus::createServerConnectionWithDynamicAddress());
        } else {
            cached_connection = connection = ConnectionPtr(Maliit::DBus::createServerConnectionWithFixedAddress(overriddenAddress));
        }

        return connection;
#else
        qCritical("This connection type to Maliit server is not available since DBus support is disabled");
#endif
    } else if (connectionType == MALIIT_INPUTCONTEXT_NAME"Direct") {
        ConnectionPtr connection(new MImDirectServerConnection);

        cached_connection = connection;

        return connection;
    } else {
        qCritical() << __PRETTY_FUNCTION__ << "Invalid connection type (" + connectionType + "), unable to create connection to Maliit server";

        return ConnectionPtr();
    }
}

} // namespace Maliit
