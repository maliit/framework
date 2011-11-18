#include "inputcontextdbusaddress.h"

#include <gio/gio.h>

#include <QDebug>

#include <tr1/memory>
#include <tr1/functional>

namespace {
    const char * const MaliitServerName = "org.maliit.server";
    const char * const MaliitServerObjectPath = "/org/maliit/server/address";
    const char * const MaliitServerInterface = "org.maliit.Server.Address";
    const char * const MaliitServerAddressProperty = "address";

    template<typename T>
    class safe_unref {
    public:
        explicit safe_unref(std::tr1::function<void (T*)>unref) :
            mUnref(unref)
        {}

        void operator()(T* t) {
            if (t)
                mUnref(t);
        }
    private:
        std::tr1::function<void (T*)> mUnref;
    };
}

namespace Maliit {
namespace InputContext {
namespace DBus {

Address::Address()
{
}

const std::string Address::get() const
{
    GError *error = NULL;
    std::tr1::shared_ptr<GDBusProxy> proxy(g_dbus_proxy_new_for_bus_sync(G_BUS_TYPE_SESSION,
                                                                         G_DBUS_PROXY_FLAGS_NONE,
                                                                         0,
                                                                         MaliitServerName,
                                                                         MaliitServerObjectPath,
                                                                         MaliitServerInterface,
                                                                         0, &error),
                                           safe_unref<GDBusProxy>(g_object_unref));

    if (!proxy) {
        qWarning() << __PRETTY_FUNCTION__ << error->message;
        g_error_free(error);
        return std::string();
    }

    std::tr1::shared_ptr<GVariant> address(g_dbus_proxy_get_cached_property(proxy.get(), MaliitServerAddressProperty),
                                           safe_unref<GVariant>(g_variant_unref));

    if (!address) {
        return std::string();
    }

    std::string result(g_variant_get_string(address.get(), 0));

    return result;
}

} // namespace DBus
} // namespace InputContext
} // namespace Maliit
