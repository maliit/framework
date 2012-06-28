/* * This file is part of maliit-framework *
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

#include "serverdbusaddress.h"

#include <QDebug>
#include <QDBusConnection>

#ifdef HAVE_GLIB_DBUS
#include <dbus/dbus.h>
#else
#include <QDBusServer>
#endif

#include <cstdlib>

namespace {
    const char * const MaliitServerName = "org.maliit.server";
    const char * const MaliitServerObjectPath = "/org/maliit/server/address";
}

namespace Maliit {
namespace Server {
namespace DBus {

AddressPublisher::AddressPublisher(const QString &address)
    : QObject()
    , mAddress(address)
{
    QDBusConnection::sessionBus().registerObject(MaliitServerObjectPath, this, QDBusConnection::ExportAllProperties);
    if (!QDBusConnection::sessionBus().registerService(MaliitServerName)) {
        qWarning("maliit-server is already running");
        std::exit(0);
    }
}

AddressPublisher::~AddressPublisher()
{
    QDBusConnection::sessionBus().unregisterObject(MaliitServerObjectPath);
}

QString AddressPublisher::address() const
{
    return mAddress;
}

Address::Address()
{}

Address::~Address()
{}

DynamicAddress::DynamicAddress()
{}

#ifdef HAVE_GLIB_DBUS
DBusServer* DynamicAddress::connect()
{
    std::string dbusAddress("unix:tmpdir=/tmp/maliit-server");

    DBusError error;
    dbus_error_init(&error);

    DBusServer *server = dbus_server_listen(dbusAddress.c_str(), &error);
    if (!server) {
        qFatal("Couldn't create D-Bus server: %s", error.message);
    }

    char *address = dbus_server_get_address(server);
    publisher.reset(new AddressPublisher(QString::fromLatin1(address)));
    dbus_free(address);

    return server;
}
#else
QDBusServer* DynamicAddress::connect()
{
    QLatin1String dbusAddress("unix:tmpdir=/tmp/maliit-server");

    QDBusServer *server = new QDBusServer(dbusAddress);

    publisher.reset(new AddressPublisher(server->address()));

    return server;
}
#endif

#ifdef HAVE_GLIB_DBUS
DBusServer* FixedAddress::connect()
{
    DBusError error;
    dbus_error_init(&error);

    DBusServer *server = dbus_server_listen(mAddress.toUtf8().constData(), &error);
    if (!server) {
        qFatal("Couldn't create D-Bus server: %s", error.message);
    }

    return server;
}
#else
QDBusServer* FixedAddress::connect()
{
    QDBusServer *server = new QDBusServer(mAddress);

    return server;
}
#endif

FixedAddress::FixedAddress(const QString &address)
    : mAddress(address)
{}

} // namespace DBus
} // namespace Server
} // namespace Maliit
