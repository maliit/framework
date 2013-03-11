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

#include "connectionfactory.h"

#ifndef MALIIT_DISABLE_DBUS
#include "dbusserverconnection.h"
#include "dbusinputcontextconnection.h"
#endif
#include "mimdirectserverconnection.h"

#ifdef HAVE_WAYLAND
#include "minputcontextwestonimprotocolconnection.h"
#endif

namespace Maliit {
#ifndef MALIIT_DISABLE_DBUS
namespace DBus {

MImServerConnection *createServerConnectionWithDynamicAddress()
{
    const QSharedPointer<Maliit::InputContext::DBus::Address> address(new Maliit::InputContext::DBus::DynamicAddress);
    return new DBusServerConnection(address);
}

MImServerConnection *createServerConnectionWithFixedAddress(const QString &fixedAddress)
{
    const QSharedPointer<Maliit::InputContext::DBus::Address> address(new Maliit::InputContext::DBus::FixedAddress(fixedAddress));
    return new DBusServerConnection(address);
}

MInputContextConnection *createInputContextConnectionWithDynamicAddress()
{
    QSharedPointer<Maliit::Server::DBus::Address> address(new Maliit::Server::DBus::DynamicAddress);
    return new DBusInputContextConnection(address);
}

MInputContextConnection *createInputContextConnectionWithFixedAddress(const QString &fixedAddress, bool allowAnonymous)
{
    Q_UNUSED(allowAnonymous);
    QSharedPointer<Maliit::Server::DBus::Address> address(new Maliit::Server::DBus::FixedAddress(fixedAddress));
    return new DBusInputContextConnection(address);
}

} // namespace DBus
#endif // MALIIT_DISABLE_DBUS

#ifdef HAVE_WAYLAND
MInputContextConnection *createWestonIMProtocolConnection()
{
    return new MInputContextWestonIMProtocolConnection;
}
#endif

QSharedPointer<MImServerConnection> createServerConnection(const QString &connectionType)
{
    typedef QSharedPointer<MImServerConnection> ConnectionPtr;
    static QWeakPointer<MImServerConnection> cached_connection;

    if (ConnectionPtr connection = cached_connection.toStrongRef()) {
        return connection;
    }

    if (connectionType.isEmpty()) {
        qCritical() << "Empty connection type name. Refusing to connect to Maliit server."
                    << "\nHave you checked the environment variables for loading input method"
                    << "modules ([QT|GTK]_IM_MODULE)?";
        return ConnectionPtr();
    }

    if (connectionType == MALIIT_INPUTCONTEXT_NAME) {
#ifndef MALIIT_DISABLE_DBUS
        const QByteArray overriddenAddress = qgetenv("MALIIT_SERVER_ADDRESS");
        ConnectionPtr connection = ConnectionPtr(overriddenAddress.isEmpty()
                                                 ? Maliit::DBus::createServerConnectionWithDynamicAddress()
                                                 : Maliit::DBus::createServerConnectionWithFixedAddress(overriddenAddress));
        cached_connection = connection;

        return connection;
#else
        qCritical("This connection type to Maliit server is not available since DBus support is disabled");
#endif
    } else if (connectionType == MALIIT_INPUTCONTEXT_NAME"Direct") {
        ConnectionPtr connection(new MImDirectServerConnection);

        cached_connection = connection;

        return connection;
    } else {
        qCritical() << __PRETTY_FUNCTION__
                    << "Invalid connection type (" + connectionType + "),"
                    << "unable to create connection to Maliit server";
        return ConnectionPtr();
    }
}

} // namespace Maliit
