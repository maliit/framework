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

#ifndef MALIIT_DBUS_CONNECTIONFACTORY_H
#define MALIIT_DBUS_CONNECTIONFACTORY_H

#include "mimserverconnection.h"
#include "minputcontextconnection.h"

namespace Maliit {
#ifndef MALIIT_DISABLE_DBUS
namespace DBus {

MImServerConnection *createServerConnectionWithDynamicAddress();
MImServerConnection *createServerConnectionWithFixedAddress(const QString &address);

MInputContextConnection *createInputContextConnectionWithDynamicAddress();
MInputContextConnection *createInputContextConnectionWithFixedAddress(const QString &fixedAddress, bool allowAnonymous);

} // namespace DBus
#endif

/*!
    \brief Create a connection to Maliit server
    \param connectionType input context name (same as in QT_IM_MODULE environment variable)

    Might return a cached connection.

    If \a connectionType is "Maliit", and MALIIT_SERVER_ADDRESS environment variable is set,
    uses its value as the server address.
*/
QSharedPointer<MImServerConnection> createServerConnection(const QString &connectionType);

} // namespace Maliit

#endif // MALIIT_DBUS_CONNECTIONFACTORY_H
