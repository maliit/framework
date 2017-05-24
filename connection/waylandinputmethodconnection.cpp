/* * This file is part of Maliit framework *
 *
 * Copyright (C) 2012 Canonical Ltd
 *
 * Contact: maliit-discuss@lists.maliit.org
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 */

#include <cerrno> // for errno
#include <cstring> // for strerror
#include <QGuiApplication>
#include <QKeyEvent>
#include <qpa/qplatformnativeinterface.h>

#include "wayland-client.h"
#include <qwayland-input-method-unstable-v1.h>
#include <qwayland-text-input-unstable-v1.h>

#include <xkbcommon/xkbcommon.h>

#include "waylandinputmethodconnection.h"

Q_LOGGING_CATEGORY(lcWaylandConnection, "maliit.connection.wayland")

namespace {

// TODO: Deduplicate it. Those values are used in
// minputcontextconnection, mimpluginmanager,
// mattributeextensionmanager and in input context implementations.
const char * const FocusStateAttribute = "focusState";
const char * const ContentTypeAttribute = "contentType";
const char * const CorrectionAttribute = "correctionEnabled";
const char * const PredictionAttribute = "predictionEnabled";
const char * const AutoCapitalizationAttribute = "autocapitalizationEnabled";
const char * const SurroundingTextAttribute = "surroundingText";
const char * const AnchorPositionAttribute = "anchorPosition";
const char * const CursorPositionAttribute = "cursorPosition";
const char * const HasSelectionAttribute = "hasSelection";
const char * const HiddenTextAttribute = "hiddenText";

typedef QPair<Qt::KeyboardModifiers, const char *> Modifier;
const Modifier modifiers[] = {
    Modifier(Qt::ShiftModifier, XKB_MOD_NAME_SHIFT),
    Modifier(Qt::ControlModifier, XKB_MOD_NAME_CTRL),
    Modifier(Qt::AltModifier, XKB_MOD_NAME_ALT),
    Modifier(Qt::MetaModifier, XKB_MOD_NAME_LOGO),
    Modifier(Qt::KeypadModifier, XKB_LED_NAME_NUM)
};

QByteArray modifiersMap()
{
    QByteArray mod_map;
    for (unsigned int iter(0); iter < (sizeof(modifiers) / sizeof(modifiers[0])); ++iter) {
        mod_map.append(modifiers[iter].second);
    }
    return mod_map;
}

xkb_mod_mask_t modifiersFromQt(const Qt::KeyboardModifiers qt_mods)
{
    xkb_mod_mask_t mod_mask(0);

    if (qt_mods == Qt::NoModifier) {
        return mod_mask;
    }

    for (unsigned int iter(0); iter < (sizeof(modifiers) / sizeof(modifiers[0])); ++iter) {
        if ((qt_mods & modifiers[iter].first) == modifiers[iter].first) {
            mod_mask |= 1 << iter;
        }
    }

    return mod_mask;
}

xkb_keysym_t keyFromQt(int qt_key)
{
    switch (qt_key) {
    case Qt::Key_Backspace:
        return XKB_KEY_BackSpace;
    case Qt::Key_Return:
        return XKB_KEY_Return;
    case Qt::Key_Left:
        return XKB_KEY_Left;
    case Qt::Key_Up:
        return XKB_KEY_Up;
    case Qt::Key_Right:
        return XKB_KEY_Right;
    case Qt::Key_Down:
        return XKB_KEY_Down;
    default:
        return XKB_KEY_NoSymbol;
    }
}

QtWayland::zwp_text_input_v1::preedit_style preeditStyleFromMaliit(Maliit::PreeditFace face)
{
    switch (face) {
    case Maliit::PreeditDefault:
        return QtWayland::zwp_text_input_v1::preedit_style_default;
    case Maliit::PreeditNoCandidates:
        return QtWayland::zwp_text_input_v1::preedit_style_incorrect;
    case Maliit::PreeditKeyPress:
        return QtWayland::zwp_text_input_v1::preedit_style_highlight;
    case Maliit::PreeditUnconvertible:
        return QtWayland::zwp_text_input_v1::preedit_style_inactive;
    case Maliit::PreeditActive:
        return QtWayland::zwp_text_input_v1::preedit_style_active;
    default:
        return QtWayland::zwp_text_input_v1::preedit_style_none;
    }
}

Maliit::TextContentType contentTypeFromWayland(uint32_t purpose)
{
    switch (purpose) {
    case QtWayland::zwp_text_input_v1::content_purpose_normal:
        return Maliit::FreeTextContentType;
    case QtWayland::zwp_text_input_v1::content_purpose_number:
        return Maliit::NumberContentType;
    case QtWayland::zwp_text_input_v1::content_purpose_phone:
        return Maliit::PhoneNumberContentType;
    case QtWayland::zwp_text_input_v1::content_purpose_url:
        return Maliit::UrlContentType;
    case QtWayland::zwp_text_input_v1::content_purpose_email:
        return Maliit::EmailContentType;
    default:
        return Maliit::CustomContentType;
    }
}

bool matchesFlag(int value,
                 int flag)
{
    return ((value & flag) == flag);
}

const unsigned int wayland_connection_id(1);

} // unnamed namespace

namespace Maliit {
namespace Wayland {

class InputMethodContext;

class InputMethod : public QtWayland::zwp_input_method_v1
{
public:
    InputMethod(MInputContextConnection *connection, struct wl_registry *registry, int id);
    ~InputMethod();

    InputMethodContext *context() const;

protected:
    void zwp_input_method_v1_activate(struct ::zwp_input_method_context_v1 *id) Q_DECL_OVERRIDE;
    void zwp_input_method_v1_deactivate(struct ::zwp_input_method_context_v1 *context) Q_DECL_OVERRIDE;

private:
    MInputContextConnection *m_connection;
    QScopedPointer<InputMethodContext> m_context;
};

class InputMethodContext : public QtWayland::zwp_input_method_context_v1
{
public:
    InputMethodContext(MInputContextConnection *connection, struct ::zwp_input_method_context_v1 *object);
    ~InputMethodContext();

    QString selection() const;
    uint32_t serial() const;

protected:
    void zwp_input_method_context_v1_commit_state(uint32_t serial) Q_DECL_OVERRIDE;
    void zwp_input_method_context_v1_content_type(uint32_t hint, uint32_t purpose) Q_DECL_OVERRIDE;
    void zwp_input_method_context_v1_invoke_action(uint32_t button, uint32_t index) Q_DECL_OVERRIDE;
    void zwp_input_method_context_v1_preferred_language(const QString &language) Q_DECL_OVERRIDE;
    void zwp_input_method_context_v1_reset() Q_DECL_OVERRIDE;
    void zwp_input_method_context_v1_surrounding_text(const QString &text, uint32_t cursor, uint32_t anchor) Q_DECL_OVERRIDE;

private:
    MInputContextConnection *m_connection;
    QVariantMap m_stateInfo;
    uint32_t m_serial;
    QString m_selection;
};

}
}

struct WaylandInputMethodConnectionPrivate
{
    Q_DECLARE_PUBLIC(WaylandInputMethodConnection)

    WaylandInputMethodConnectionPrivate(WaylandInputMethodConnection *connection);
    ~WaylandInputMethodConnectionPrivate();

    void handleRegistryGlobal(uint32_t name,
                              const char *interface,
                              uint32_t version);
    void handleRegistryGlobalRemove(uint32_t name);

    Maliit::Wayland::InputMethodContext *context();

    WaylandInputMethodConnection *q_ptr;
    wl_display *display;
    wl_registry *registry;
    QScopedPointer<Maliit::Wayland::InputMethod> input_method;
};

namespace {

void registryGlobal(void *data,
                    wl_registry *registry,
                    uint32_t name,
                    const char *interface,
                    uint32_t version)
{
    WaylandInputMethodConnectionPrivate *d =
            static_cast<WaylandInputMethodConnectionPrivate *>(data);

    Q_UNUSED(registry);
    d->handleRegistryGlobal(name, interface, version);
}

void registryGlobalRemove(void *data,
                          wl_registry *registry,
                          uint32_t name)
{
    WaylandInputMethodConnectionPrivate *d =
            static_cast<WaylandInputMethodConnectionPrivate *>(data);

    Q_UNUSED(registry);
    d->handleRegistryGlobalRemove(name);
}

const wl_registry_listener maliit_registry_listener = {
    registryGlobal,
    registryGlobalRemove
};


} // unnamed namespace

WaylandInputMethodConnectionPrivate::WaylandInputMethodConnectionPrivate(WaylandInputMethodConnection *connection)
    : q_ptr(connection),
      display(0),
      registry(0),
      input_method()
{
    display = static_cast<wl_display *>(QGuiApplication::platformNativeInterface()->nativeResourceForIntegration("display"));
    if (!display) {
        qCritical() << Q_FUNC_INFO << "Failed to get a display.";
        return;
    }
    registry = wl_display_get_registry(display);
    wl_registry_add_listener(registry, &maliit_registry_listener, this);
}

WaylandInputMethodConnectionPrivate::~WaylandInputMethodConnectionPrivate()
{
    input_method.reset();
    if (registry) {
        wl_registry_destroy(registry);
    }
}

void WaylandInputMethodConnectionPrivate::handleRegistryGlobal(uint32_t name,
                                                               const char *interface,
                                                               uint32_t version)
{
    Q_UNUSED(version);
    Q_Q(WaylandInputMethodConnection);

    if (!strcmp(interface, "zwp_input_method_v1")) {
        input_method.reset(new Maliit::Wayland::InputMethod(q, registry, name));
    }
}

void WaylandInputMethodConnectionPrivate::handleRegistryGlobalRemove(uint32_t name)
{
    qCDebug(lcWaylandConnection) << Q_FUNC_INFO << name;
}

Maliit::Wayland::InputMethodContext *WaylandInputMethodConnectionPrivate::context()
{
    return input_method ? input_method->context() : 0;
}

// MInputContextWestonIMProtocolConnection

WaylandInputMethodConnection::WaylandInputMethodConnection()
    : d_ptr(new WaylandInputMethodConnectionPrivate(this))
{
}

WaylandInputMethodConnection::~WaylandInputMethodConnection()
{
}

void WaylandInputMethodConnection::sendPreeditString(const QString &string,
                                                     const QList<Maliit::PreeditTextFormat> &preedit_formats,
                                                     int replace_start,
                                                     int replace_length,
                                                     int cursor_pos)
{
    Q_D(WaylandInputMethodConnection);

    qCDebug(lcWaylandConnection) << Q_FUNC_INFO << string << replace_start << replace_length << cursor_pos;

    if (!d->context())
        return;

    MInputContextConnection::sendPreeditString(string, preedit_formats,
                                               replace_start, replace_length,
                                               cursor_pos);

    if (replace_length > 0) {
        int cursor = widgetState().value(CursorPositionAttribute).toInt();
        uint32_t index = string.midRef(qMin(cursor + replace_start, cursor), qAbs(replace_start)).toUtf8().size();
        uint32_t length = string.midRef(cursor + replace_start, replace_length).toUtf8().size();
        d->context()->delete_surrounding_text(index, length);
    }

    Q_FOREACH (const Maliit::PreeditTextFormat& format, preedit_formats) {
        QtWayland::zwp_text_input_v1::preedit_style style = preeditStyleFromMaliit(format.preeditFace);
        uint32_t index = string.leftRef(format.start).toUtf8().size();
        uint32_t length = string.leftRef(format.start + format.length).toUtf8().size() - index;
        qCDebug(lcWaylandConnection) << Q_FUNC_INFO << "preedit_styling" << index << length;
        d->context()->preedit_styling(index, length, style);
    }

    // TODO check if defined like that/required
    if (cursor_pos < 0) {
        cursor_pos = string.size() + 1 - cursor_pos;
    }

    qCDebug(lcWaylandConnection) << Q_FUNC_INFO << "preedit_cursor" << string.leftRef(cursor_pos).toUtf8().size();
    d->context()->preedit_cursor(string.leftRef(cursor_pos).toUtf8().size());
    qCDebug(lcWaylandConnection) << Q_FUNC_INFO << "preedit_string" << string;
    d->context()->preedit_string(d->context()->serial(), string, string);
}


void WaylandInputMethodConnection::sendCommitString(const QString &string,
                                                    int replace_start,
                                                    int replace_length,
                                                    int cursor_pos)
{
    Q_D(WaylandInputMethodConnection);

    qCDebug(lcWaylandConnection) << Q_FUNC_INFO << string << replace_start << replace_length << cursor_pos;

    if (!d->context())
        return;

    MInputContextConnection::sendCommitString(string, replace_start, replace_length, cursor_pos);

    if (cursor_pos != 0) {
        qCWarning(lcWaylandConnection) << Q_FUNC_INFO << "cursor_pos:" << cursor_pos << "!= 0 not supported yet";
        cursor_pos = 0;
    }

    if (replace_length > 0) {
        int cursor = widgetState().value(CursorPositionAttribute).toInt();
        uint32_t index = string.midRef(qMin(cursor + replace_start, cursor), qAbs(replace_start)).toUtf8().size();
        uint32_t length = string.midRef(cursor + replace_start, replace_length).toUtf8().size();
        d->context()->delete_surrounding_text(index, length);
    }

    cursor_pos = string.leftRef(cursor_pos).toUtf8().size();
    d->context()->cursor_position(cursor_pos, cursor_pos);
    d->context()->commit_string(d->context()->serial(), string);
}

void WaylandInputMethodConnection::sendKeyEvent(const QKeyEvent &keyEvent,
                                                Maliit::EventRequestType requestType)
{
    Q_D(WaylandInputMethodConnection);

    qCDebug(lcWaylandConnection) << Q_FUNC_INFO;

    if (!d->context())
        return;

    xkb_keysym_t sym(keyFromQt(keyEvent.key()));

    if (sym == XKB_KEY_NoSymbol) {
        qCWarning(lcWaylandConnection) << "No conversion from Qt::Key:" << keyEvent.key() << "to XKB key. Update the keyFromQt() function.";
        return;
    }

    wl_keyboard_key_state state;

    switch (keyEvent.type()) {
    case QEvent::KeyPress:
        state = WL_KEYBOARD_KEY_STATE_PRESSED;
        break;

    case QEvent::KeyRelease:
        state = WL_KEYBOARD_KEY_STATE_RELEASED;
        break;

    default:
        qCWarning(lcWaylandConnection) << "Unknown QKeyEvent type:" << keyEvent.type();
        return;
    }

    xkb_mod_mask_t modifiers(modifiersFromQt(keyEvent.modifiers()));

    MInputContextConnection::sendKeyEvent(keyEvent, requestType);

    d->context()->keysym(d->context()->serial(),
                         keyEvent.timestamp(),
                         sym, state, modifiers);
}

QString WaylandInputMethodConnection::selection(bool &valid)
{
    Q_D(WaylandInputMethodConnection);

    qCDebug(lcWaylandConnection) << Q_FUNC_INFO;

    Maliit::Wayland::InputMethodContext *context = d->input_method->context();

    valid = context && !context->selection().isEmpty();
    return context ? context->selection() : QString();
}

void WaylandInputMethodConnection::setLanguage(const QString &language)
{
    Q_D(WaylandInputMethodConnection);

    qCDebug(lcWaylandConnection) << Q_FUNC_INFO;

    if (!d->context())
        return;

    d->context()->language(d->context()->serial(), language);
}

void WaylandInputMethodConnection::setSelection(int start, int length)
{
    Q_D (WaylandInputMethodConnection);

    qCDebug(lcWaylandConnection) << Q_FUNC_INFO;

    if (!d->context())
        return;

    QString surrounding = widgetState().value(SurroundingTextAttribute).toString();
    uint32_t index(surrounding.leftRef(start + length).toUtf8().size());
    uint32_t anchor(surrounding.leftRef(start).toUtf8().size());

    d->context()->cursor_position(index, anchor);
    d->context()->commit_string(d->context()->serial(), QString());
}

namespace Maliit {
namespace Wayland {

InputMethod::InputMethod(MInputContextConnection *connection, struct wl_registry *registry, int id)
    : QtWayland::zwp_input_method_v1(registry, id, 1)
    , m_connection(connection)
    , m_context()
{
    qCDebug(lcWaylandConnection) << Q_FUNC_INFO;
}

InputMethod::~InputMethod()
{
}

InputMethodContext *InputMethod::context() const
{
    return m_context.data();
}

void InputMethod::zwp_input_method_v1_activate(struct ::zwp_input_method_context_v1 *id)
{
    qCDebug(lcWaylandConnection) << Q_FUNC_INFO;

    m_context.reset(new InputMethodContext(m_connection, id));

    m_context->modifiers_map(modifiersMap());

}

void InputMethod::zwp_input_method_v1_deactivate(struct zwp_input_method_context_v1 *)
{
    qCDebug(lcWaylandConnection) << Q_FUNC_INFO;

    m_context.reset();

    m_connection->handleDisconnection(wayland_connection_id);
}

InputMethodContext::InputMethodContext(MInputContextConnection *connection, struct ::zwp_input_method_context_v1 *object)
    : QtWayland::zwp_input_method_context_v1(object)
    , m_connection(connection)
    , m_stateInfo()
    , m_serial(0)
    , m_selection()
{
    qCDebug(lcWaylandConnection) << Q_FUNC_INFO;

    m_stateInfo[FocusStateAttribute] = true;
    m_connection->activateContext(wayland_connection_id);
    m_connection->showInputMethod(wayland_connection_id);
}

InputMethodContext::~InputMethodContext()
{
    qCDebug(lcWaylandConnection) << Q_FUNC_INFO;

    m_stateInfo.clear();
    m_stateInfo[FocusStateAttribute] = false;
    m_connection->updateWidgetInformation(wayland_connection_id, m_stateInfo, true);
    m_connection->hideInputMethod(wayland_connection_id);
}

QString InputMethodContext::selection() const
{
    return m_selection;
}

uint32_t InputMethodContext::serial() const
{
    return m_serial;
}

void InputMethodContext::zwp_input_method_context_v1_commit_state(uint32_t serial)
{
    qCDebug(lcWaylandConnection) << Q_FUNC_INFO;

    m_serial = serial;
    m_connection->updateWidgetInformation(wayland_connection_id, m_stateInfo, false);
}

void InputMethodContext::zwp_input_method_context_v1_content_type(uint32_t hint, uint32_t purpose)
{
    qCDebug(lcWaylandConnection) << Q_FUNC_INFO;

    m_stateInfo[ContentTypeAttribute] = contentTypeFromWayland(purpose);
    m_stateInfo[AutoCapitalizationAttribute] = matchesFlag(hint, QtWayland::zwp_text_input_v1::content_hint_auto_capitalization);
    m_stateInfo[CorrectionAttribute] = matchesFlag(hint, QtWayland::zwp_text_input_v1::content_hint_auto_correction);
    m_stateInfo[PredictionAttribute] = matchesFlag(hint, QtWayland::zwp_text_input_v1::content_hint_auto_completion);
    m_stateInfo[HiddenTextAttribute] = matchesFlag(hint, QtWayland::zwp_text_input_v1::content_hint_hidden_text);
}

void InputMethodContext::zwp_input_method_context_v1_invoke_action(uint32_t button, uint32_t index)
{
    qCDebug(lcWaylandConnection) << Q_FUNC_INFO << button << index;
}

void InputMethodContext::zwp_input_method_context_v1_preferred_language(const QString &language)
{
    qCDebug(lcWaylandConnection) << Q_FUNC_INFO << language;
}

void InputMethodContext::zwp_input_method_context_v1_reset()
{
    qCDebug(lcWaylandConnection) << Q_FUNC_INFO;

    m_connection->reset(wayland_connection_id);
}

void InputMethodContext::zwp_input_method_context_v1_surrounding_text(const QString &text, uint32_t cursor, uint32_t anchor)
{
    qCDebug(lcWaylandConnection) << Q_FUNC_INFO;

    const QByteArray &utf8_text(text.toUtf8());

    m_stateInfo[SurroundingTextAttribute] = text;
    m_stateInfo[CursorPositionAttribute] = QString::fromUtf8(utf8_text.constData(), cursor).size();
    m_stateInfo[AnchorPositionAttribute] = QString::fromUtf8(utf8_text.constData(), anchor).size();
    if (cursor == anchor) {
        m_stateInfo[HasSelectionAttribute] = false;
        m_selection.clear();
    } else {
        m_stateInfo[HasSelectionAttribute] = true;
        uint32_t begin = qMin(anchor, cursor);
        uint32_t end = qMax(anchor, cursor);
        m_selection = QString::fromUtf8(utf8_text.constData() + begin, end - begin);
    }
}

}
}
