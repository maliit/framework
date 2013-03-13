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

#include "dbusinputcontextconnection.h"

#ifdef HAVE_WAYLAND
#include "minputcontextwestonimprotocolconnection.h"
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
    return new MInputContextWestonIMProtocolConnection;
}
#endif

} // namespace Maliit
