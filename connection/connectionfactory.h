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

#include <QString>

class MInputContextConnection;

namespace Maliit {
namespace DBus {

MInputContextConnection *createInputContextConnectionWithDynamicAddress();
MInputContextConnection *createInputContextConnectionWithFixedAddress(const QString &fixedAddress, bool allowAnonymous);

} // namespace DBus

#ifdef HAVE_WAYLAND
MInputContextConnection *createWestonIMProtocolConnection();
#endif

} // namespace Maliit

#endif // MALIIT_DBUS_CONNECTIONFACTORY_H
