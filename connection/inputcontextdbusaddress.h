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

#ifndef MALIIT_INPUTCONTEXT_DBUS_INPUTCONTEXTDBUSADDRESS_H
#define MALIIT_INPUTCONTEXT_DBUS_INPUTCONTEXTDBUSADDRESS_H

#include <QObject>

class QDBusVariant;
class QDBusError;

namespace Maliit {
namespace InputContext {
namespace DBus {

class Address : public QObject
{
    Q_OBJECT

public:
    Address();
    virtual ~Address();

    virtual void get() = 0;

Q_SIGNALS:
    void addressReceived(const QString &address);
    void addressFetchError(const QString &errorMessage);
};


class DynamicAddress : public Address
{
    Q_OBJECT

public:
    void get();

private Q_SLOTS:
    void successCallback(const QDBusVariant &address);
    void errorCallback(const QDBusError &error);
};

class FixedAddress : public Address
{
    Q_OBJECT

public:
    FixedAddress(const QString &address);
    void get();

private:
    QString mAddress;
};

} // namespace DBus
} // namespace InputContext
} // namespace Maliit

#endif // MALIIT_INPUTCONTEXT_DBUS_INPUTCONTEXTDBUSADDRESS_H
