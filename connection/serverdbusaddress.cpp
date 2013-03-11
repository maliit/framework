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

#include "serverdbusaddress.h"

#include <QDebug>
#include <QDBusConnection>

#include <QDBusServer>

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

QDBusServer* DynamicAddress::connect()
{
    QLatin1String dbusAddress("unix:tmpdir=/tmp/maliit-server");

    QDBusServer *server = new QDBusServer(dbusAddress);

    publisher.reset(new AddressPublisher(server->address()));

    return server;
}

QDBusServer* FixedAddress::connect()
{
    QDBusServer *server = new QDBusServer(mAddress);

    return server;
}

FixedAddress::FixedAddress(const QString &address)
    : mAddress(address)
{}

} // namespace DBus
} // namespace Server
} // namespace Maliit
