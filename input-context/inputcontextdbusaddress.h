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
