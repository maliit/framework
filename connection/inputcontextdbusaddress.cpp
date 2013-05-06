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

#include "inputcontextdbusaddress.h"

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusVariant>
#include <QDBusError>

namespace {
    const char * const MaliitServerName = "org.maliit.server";
    const char * const MaliitServerObjectPath = "/org/maliit/server/address";
    const char * const MaliitServerInterface = "org.maliit.Server.Address";
    const char * const MaliitServerAddressProperty = "address";

    const char * const DBusPropertiesInterface = "org.freedesktop.DBus.Properties";
    const char * const DBusPropertiesGetMethod = "Get";
}

namespace Maliit {
namespace InputContext {
namespace DBus {

Address::Address()
{
}

Address::~Address()
{
}

void DynamicAddress::get()
{
    QList<QVariant> arguments;
    arguments.push_back(QVariant(QString::fromLatin1(MaliitServerInterface)));
    arguments.push_back(QVariant(QString::fromLatin1(MaliitServerAddressProperty)));

    QDBusMessage message = QDBusMessage::createMethodCall(MaliitServerName, MaliitServerObjectPath,
                                                          DBusPropertiesInterface, DBusPropertiesGetMethod);
    message.setArguments(arguments);

    QDBusConnection::sessionBus().callWithCallback(message, this,
                                                   SLOT(successCallback(QDBusVariant)),
                                                   SLOT(errorCallback(QDBusError)));
}

void DynamicAddress::successCallback(const QDBusVariant &address)
{
    Q_EMIT addressReceived(address.variant().toString());
}

void DynamicAddress::errorCallback(const QDBusError &error)
{
    Q_EMIT addressFetchError(error.message());
}

FixedAddress::FixedAddress(const QString &address)
    : mAddress(address)
{
}

void FixedAddress::get()
{
    Q_EMIT this->addressReceived(mAddress);
}

} // namespace DBus
} // namespace InputContext
} // namespace Maliit
