/* * This file is part of Maliit framework *
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 *
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 */

#include "connectionfactory.h"

#include "minputcontextconnection.h"
#include "dbusinputcontextconnection.h"

#include <QDebug>
#include <QGuiApplication>

#ifdef HAVE_WAYLAND
#include "waylandinputmethodconnection.h"
#include "dbuswaylandinputcontextconnection.h"
#endif

namespace Maliit {
namespace DBus {

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

#ifdef HAVE_WAYLAND
MInputContextConnection *createWestonIMProtocolConnection()
{
    return new WaylandInputMethodConnection;
}

MInputContextConnection *createDBusAndWaylandConnectionWithDynamicAddress()
{
    QSharedPointer<Maliit::Server::DBus::Address> address(new Maliit::Server::DBus::DynamicAddress);
    return new DBusWaylandInputContextConnection(address);
}

MInputContextConnection *createDBusAndWaylandConnectionWithFixedAddress(const QString &fixedAddress, bool allowAnonymous)
{
    Q_UNUSED(allowAnonymous);
    QSharedPointer<Maliit::Server::DBus::Address> address(new Maliit::Server::DBus::FixedAddress(fixedAddress));
    return new DBusWaylandInputContextConnection(address);
}
#endif

ConnectionMode connectionModeFromEnvironment()
{
#ifdef HAVE_WAYLAND
    const bool onWayland = QGuiApplication::platformName().startsWith("wayland");
    const QByteArray mode = qgetenv("MALIIT_CONNECTION");

    if (mode == "dbus") {
        return ConnectionMode::DBus;
    } else if (mode == "wayland" || mode == "both") {
        if (!onWayland) {
            qWarning() << "MALIIT_CONNECTION=" << mode << "needs a Wayland platform, using D-Bus only";
            return ConnectionMode::DBus;
        }
        return mode == "both" ? ConnectionMode::DBusAndWayland : ConnectionMode::Wayland;
    } else if (!mode.isEmpty()) {
        qWarning() << "Unknown MALIIT_CONNECTION value" << mode << ", expected dbus, wayland or both";
    }

    const QByteArray forceDbus = qgetenv("MALIIT_FORCE_DBUS_CONNECTION");
    if (onWayland && (forceDbus.isEmpty() || forceDbus == "0")) {
        return ConnectionMode::Wayland;
    }
#endif
    return ConnectionMode::DBus;
}

} // namespace Maliit
