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

#include "glibdbusimserverproxy.h"
#include "minputcontextglibdbusconnection.h"

namespace Maliit {
namespace DBus {

MImServerConnection *createServerConnectionWithDynamicAddress()
{
    const QSharedPointer<Maliit::InputContext::DBus::Address> address(new Maliit::InputContext::DBus::DynamicAddress);
    return new GlibDBusIMServerProxy(address);
}

MImServerConnection *createServerConnectionWithFixedAddress(const QString &fixedAddress)
{
    const QSharedPointer<Maliit::InputContext::DBus::Address> address(new Maliit::InputContext::DBus::FixedAddress(fixedAddress));
    return new GlibDBusIMServerProxy(address);
}

MInputContextConnection *createInputContextConnectionWithDynamicAddress()
{
    std::tr1::shared_ptr<Maliit::Server::DBus::Address> address(new Maliit::Server::DBus::DynamicAddress);
    return new MInputContextGlibDBusConnection(address, false);
}

MInputContextConnection *createInputContextConnectionWithFixedAddress(const QString &fixedAddress, bool allowAnonymous)
{
    std::tr1::shared_ptr<Maliit::Server::DBus::Address> address(new Maliit::Server::DBus::FixedAddress(fixedAddress));
    return new MInputContextGlibDBusConnection(address, allowAnonymous);
}

} // namespace DBus
} // namespace Maliit
