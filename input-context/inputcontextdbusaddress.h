#ifndef MALIIT_INPUTCONTEXT_DBUS_INPUTCONTEXTDBUSADDRESS_H
#define MALIIT_INPUTCONTEXT_DBUS_INPUTCONTEXTDBUSADDRESS_H

#include <string>

namespace Maliit {
namespace InputContext {
namespace DBus {

class Address
{
public:
    explicit Address();

    virtual const std::string get() const;
};

} // namespace DBus
} // namespace InputContext
} // namespace Maliit

#endif // MALIIT_INPUTCONTEXT_DBUS_INPUTCONTEXTDBUSADDRESS_H
