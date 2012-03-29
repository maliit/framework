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

#ifndef MALIIT_DBUS_CONNECTIONFACTORY_H
#define MALIIT_DBUS_CONNECTIONFACTORY_H

#include "mimserverconnection.h"
#include "minputcontextconnection.h"

namespace Maliit {
namespace DBus {

MImServerConnection *createServerConnectionWithDynamicAddress();
MImServerConnection *createServerConnectionWithFixedAddress(const QString &address);

MInputContextConnection *createInputContextConnectionWithDynamicAddress();
MInputContextConnection *createInputContextConnectionWithFixedAddress(const QString &fixedAddress, bool allowAnonymous);

} // namespace DBus
} // namespace Maliit

#endif // MALIIT_DBUS_CONNECTIONFACTORY_H
