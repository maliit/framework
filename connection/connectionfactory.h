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

#ifndef MALIIT_DBUS_CONNECTIONFACTORY_H
#define MALIIT_DBUS_CONNECTIONFACTORY_H

#include <QString>

class MInputContextConnection;

namespace Maliit {
namespace DBus {

MInputContextConnection *createInputContextConnectionWithDynamicAddress();
MInputContextConnection *createInputContextConnectionWithFixedAddress(const QString &fixedAddress, bool allowAnonymous);

} // namespace DBus

#ifdef HAVE_WAYLAND
MInputContextConnection *createWestonIMProtocolConnection();
MInputContextConnection *createDBusAndWaylandConnectionWithDynamicAddress();
MInputContextConnection *createDBusAndWaylandConnectionWithFixedAddress(const QString &fixedAddress, bool allowAnonymous);
#endif

enum class ConnectionMode {
    DBus,
    Wayland,
    DBusAndWayland
};

/*! Picks the input context connection from the environment.
 *
 * MALIIT_CONNECTION=dbus|wayland|both selects the connection explicitly.
 * Without it, MALIIT_FORCE_DBUS_CONNECTION keeps its old meaning. Wayland
 * connections are only used when running on a Wayland Qt platform.
 */
ConnectionMode connectionModeFromEnvironment();

} // namespace Maliit

#endif // MALIIT_DBUS_CONNECTIONFACTORY_H
