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

#ifndef GLIBDBUSIMSERVERPROXY_P_H
#define GLIBDBUSIMSERVERPROXY_P_H

#include <QSet>
#include <QSharedPointer>
#include <dbus/dbus-glib.h>

#include <tr1/memory>

#include "inputcontextdbusaddress.h"

namespace Maliit
{
    namespace DBusGLib
    {
    // std::tr1::shared_ptr allows to specify a deleter. Hiding it behind a typedef,
    // as we do not really need all the shared pointer semantics here.
    typedef std::tr1::shared_ptr<DBusGConnection> ConnectionRef;
    }
}

class GlibDBusIMServerProxyPrivate
{

public:
    GlibDBusIMServerProxyPrivate(const QSharedPointer<Maliit::InputContext::DBus::Address> &address);
    ~GlibDBusIMServerProxyPrivate();

public:
    void resetNotify(DBusGProxy *proxy, DBusGProxyCall *callId);

public:
    DBusGProxy *glibObjectProxy;
    Maliit::DBusGLib::ConnectionRef connection;
    GObject *inputContextAdaptor;
    bool active;
    QSet<DBusGProxyCall *> pendingResetCalls;
    const QSharedPointer<Maliit::InputContext::DBus::Address> mAddress;
};



#endif // GLIBDBUSIMSERVERPROXY_P_H
