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

#include <QDBusConnection>
#include <QDBusMessage>

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

void Address::get(QObject *receiver, const char *returnMethod, const char *errorMethod)
{
    QList<QVariant> arguments;
    arguments.push_back(QVariant(QString::fromLatin1(MaliitServerInterface)));
    arguments.push_back(QVariant(QString::fromLatin1(MaliitServerAddressProperty)));

    QDBusMessage message = QDBusMessage::createMethodCall(MaliitServerName, MaliitServerObjectPath, DBusPropertiesInterface, DBusPropertiesGetMethod);
    message.setArguments(arguments);

    QDBusConnection::sessionBus().callWithCallback(message, receiver, returnMethod, errorMethod);
}

} // namespace DBus
} // namespace InputContext
} // namespace Maliit
