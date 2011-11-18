#ifndef MALIIT_SERVER_DBUS_SERVERDBUSADDRESS_H
#define MALIIT_SERVER_DBUS_SERVERDBUSADDRESS_H

#include <string>

#include <QObject>

struct DBusServer;

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

    const QString address() const
    { return mAddress; }

private:
    const QString mAddress;
};

class Address
{
public:
    explicit Address();

    virtual DBusServer* connect();

private:
    QScopedPointer<AddressPublisher> publisher;
};

} // namespace DBus
} // namespace Server
} // namespace Maliit

#endif // MALIIT_SERVER_DBUS_SERVERDBUSADDRESS_H
