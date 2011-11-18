#include "serverdbusaddress.h"

#include <QDebug>
#include <QDBusConnection>

#include <dbus/dbus.h>

namespace {
    const char * const MaliitServerName = "org.maliit.server";
    const char * const MaliitServerObjectPath = "/org/maliit/server/address";
}

namespace Maliit {
namespace Server {
namespace DBus {

AddressPublisher::AddressPublisher(const QString &address)
        : QObject()
        , mAddress(address)
{
    QDBusConnection::sessionBus().registerObject(MaliitServerObjectPath, this, QDBusConnection::ExportAllProperties);
    QDBusConnection::sessionBus().registerService(MaliitServerName);
}

AddressPublisher::~AddressPublisher()
{
    QDBusConnection::sessionBus().unregisterObject(MaliitServerObjectPath);
}

Address::Address()
{}

DBusServer* Address::connect()
{
    std::string dbusAddress("unix:tmpdir=/tmp/maliit-server");

    DBusError error;
    dbus_error_init(&error);

    DBusServer *server = dbus_server_listen(dbusAddress.c_str(), &error);
    if (!server) {
        qFatal("Couldn't create D-Bus server: %s", error.message);
    }

    char *address = dbus_server_get_address(server);
    publisher.reset(new AddressPublisher(QString::fromLatin1(address)));
    dbus_free(address);

    return server;
}

} // namespace DBus
} // namespace Server
} // namespace Maliit
