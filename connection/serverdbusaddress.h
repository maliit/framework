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

#ifndef MALIIT_SERVER_DBUS_SERVERDBUSADDRESS_H
#define MALIIT_SERVER_DBUS_SERVERDBUSADDRESS_H

#include <QObject>
#include <QString>

QT_BEGIN_NAMESPACE
class QDBusServer;
QT_END_NAMESPACE

namespace Maliit {
namespace Server {
namespace DBus {

class AddressPublisher : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.maliit.Server.Address")

    Q_PROPERTY(QString address READ address)

public:
    explicit AddressPublisher(const QString &address);
    ~AddressPublisher();
    QString address() const;

private:
    const QString mAddress;
};

class Address
{
public:
    explicit Address();
    virtual ~Address();

    virtual QDBusServer* connect() = 0;
};

class DynamicAddress : public Address
{

public:
    explicit DynamicAddress();

    //! reimpl
    virtual QDBusServer* connect();

private:
    QScopedPointer<AddressPublisher> publisher;
};

class FixedAddress : public Address
{

public:
    explicit FixedAddress(const QString &address);

    //! reimpl
    virtual QDBusServer* connect();

private:
    QString mAddress;
};

} // namespace DBus
} // namespace Server
} // namespace Maliit

#endif // MALIIT_SERVER_DBUS_SERVERDBUSADDRESS_H
