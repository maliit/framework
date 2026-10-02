/* * This file is part of Maliit framework *
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 */

#ifndef WAYLAND_INPUT_METHOD_TRANSPORT_H
#define WAYLAND_INPUT_METHOD_TRANSPORT_H

#include <maliit/namespace.h>
#include "minputcontextconnection.h"

#include "wayland-client.h"
#include <xkbcommon/xkbcommon.h>

#include <QtCore>

#include <limits>

class QKeyEvent;

namespace Maliit {
namespace Wayland {

// D-Bus connection ids count up from 1 (0 means "no active connection"), so the
// Wayland client uses the top of the range to keep both id spaces disjoint.
const unsigned int ConnectionId = std::numeric_limits<unsigned int>::max();

class InputMethod;
class InputMethodContext;

// Speaks zwp_input_method_v1 on behalf of MInputContextConnection.
class InputMethodTransport
{
public:
    struct Key {
        xkb_keysym_t sym;
        wl_keyboard_key_state state;
        xkb_mod_mask_t modifiers;
    };

    explicit InputMethodTransport(MInputContextConnection *owner);
    ~InputMethodTransport();

    void handleRegistryGlobal(uint32_t name,
                              const char *interface,
                              uint32_t version);
    void handleRegistryGlobalRemove(uint32_t name);

    InputMethodContext *context();

    void sendPreeditString(const QString &string,
                           const QList<Maliit::PreeditTextFormat> &preedit_formats,
                           int replace_start,
                           int replace_length,
                           int cursor_pos,
                           int widget_cursor);
    void sendCommitString(const QString &string,
                          int replace_start,
                          int replace_length,
                          int cursor_pos,
                          int widget_cursor);
    static bool keyFromQKeyEvent(const QKeyEvent &keyEvent, Key *key);
    void sendKey(const Key &key, uint32_t timestamp);
    QString selection(bool &valid);
    void setLanguage(const QString &language);
    void setSelection(int start, int length, const QString &surrounding);

private:
    MInputContextConnection *owner;
    wl_display *display;
    wl_registry *registry;
    QScopedPointer<InputMethod> input_method;
};

} // namespace Wayland
} // namespace Maliit

#endif // WAYLAND_INPUT_METHOD_TRANSPORT_H
