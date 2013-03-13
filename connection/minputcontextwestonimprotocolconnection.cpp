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
#include "wayland-input-method-client-protocol.h"
#include "wayland-text-client-protocol.h"

#include <xkbcommon/xkbcommon.h>

#include "minputcontextwestonimprotocolconnection.h"

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

struct ModTuple
{
    Qt::KeyboardModifier qt;
    const char *name;
    int size;
};

ModTuple tuples [] = {
    {Qt::ShiftModifier, XKB_MOD_NAME_SHIFT, sizeof(XKB_MOD_NAME_SHIFT)},
    {Qt::ControlModifier, XKB_MOD_NAME_CTRL, sizeof(XKB_MOD_NAME_CTRL)},
    {Qt::AltModifier, XKB_MOD_NAME_ALT, sizeof(XKB_MOD_NAME_ALT)},
    {Qt::MetaModifier, XKB_MOD_NAME_LOGO, sizeof(XKB_MOD_NAME_LOGO)},
    {Qt::KeypadModifier, XKB_LED_NAME_NUM, sizeof(XKB_LED_NAME_NUM)}
};

class Modifiers
{
public:
    Modifiers();
    ~Modifiers();

    xkb_mod_mask_t fromQt(const Qt::KeyboardModifiers& qt_mods) const;
    wl_array *getModMap();

private:
    wl_array mod_map;
};

Modifiers::Modifiers()
{
    wl_array_init(&mod_map);
    for (unsigned int iter(0); iter < (sizeof (tuples) / sizeof (tuples[0])); ++iter) {
        void *data(wl_array_add(&mod_map, tuples[iter].size));

        if (!data) {
            qCritical() << __PRETTY_FUNCTION__ << "Couldn't allocate memory in wl_array.";
            return;
        }

        memcpy(data, static_cast<const void *>(tuples[iter].name), tuples[iter].size);
    }
}

Modifiers::~Modifiers()
{
    wl_array_release (&mod_map);
}

xkb_mod_mask_t Modifiers::fromQt(const Qt::KeyboardModifiers& qt_mods) const
{
    xkb_mod_mask_t mod_mask(0);

    if (qt_mods != Qt::NoModifier) {
        for (unsigned int iter(0); iter < (sizeof (tuples) / sizeof (tuples[0])); ++iter) {
            if ((qt_mods & tuples[iter].qt) == tuples[iter].qt) {
                mod_mask |= 1 << iter;
            }
        }
    }

    return mod_mask;
}

// not const because I would be forced to use const_cast<> on mod_map
wl_array *Modifiers::getModMap()
{
    return &mod_map;
}

} // unnamed namespace

struct MInputContextWestonIMProtocolConnectionPrivate
{
    Q_DECLARE_PUBLIC(MInputContextWestonIMProtocolConnection);

    MInputContextWestonIMProtocolConnectionPrivate(MInputContextWestonIMProtocolConnection *connection);
    ~MInputContextWestonIMProtocolConnectionPrivate();

    void handleRegistryGlobal(uint32_t name,
                              const char *interface,
                              uint32_t version);
    void handleRegistryGlobalRemove(uint32_t name);
    void handleInputMethodActivate(input_method_context *context,
                                   uint32_t serial);
    void handleInputMethodDeactivate(input_method_context *context);
    void handleInputMethodContextSurroundingText(const char *text,
                                                 uint32_t cursor,
                                                 uint32_t anchor);
    void handleInputMethodContextReset(uint32_t serial);
    void handleInputMethodContextContentType(uint32_t hint,
                                             uint32_t purpose);
    void handleInputMethodContextInvokeAction(uint32_t button,
                                              uint32_t index);

    MInputContextWestonIMProtocolConnection *q_ptr;
    wl_display *display;
    wl_registry *registry;
    input_method *im;
    input_method_context *im_context;
    uint32_t im_serial;
    QString selection;
    Modifiers mods;
    QMap<QString, QVariant> state_info;
};

namespace {

const unsigned int connection_id(1);

void registryGlobal(void *data,
                    wl_registry *registry,
                    uint32_t name,
                    const char *interface,
                    uint32_t version)
{
    qDebug() << __PRETTY_FUNCTION__;
    MInputContextWestonIMProtocolConnectionPrivate *d =
        static_cast<MInputContextWestonIMProtocolConnectionPrivate *>(data);

    Q_UNUSED(registry);
    d->handleRegistryGlobal(name, interface, version);
}

void registryGlobalRemove(void *data,
                          wl_registry *registry,
                          uint32_t name)
{
    qDebug() << __PRETTY_FUNCTION__;
    MInputContextWestonIMProtocolConnectionPrivate *d =
        static_cast<MInputContextWestonIMProtocolConnectionPrivate *>(data);

    Q_UNUSED(registry);
    d->handleRegistryGlobalRemove(name);
}

const wl_registry_listener maliit_registry_listener = {
    registryGlobal,
    registryGlobalRemove
};

void inputMethodActivate(void *data,
                         input_method *input_method,
                         input_method_context *context,
                         uint32_t serial)
{
    qDebug() << __PRETTY_FUNCTION__;
    MInputContextWestonIMProtocolConnectionPrivate *d =
        static_cast<MInputContextWestonIMProtocolConnectionPrivate *>(data);

    Q_UNUSED(input_method);
    d->handleInputMethodActivate(context, serial);
}

void inputMethodDeactivate(void *data,
                           input_method *input_method,
                           input_method_context *context)
{
    qDebug() << __PRETTY_FUNCTION__;
    MInputContextWestonIMProtocolConnectionPrivate *d =
        static_cast<MInputContextWestonIMProtocolConnectionPrivate *>(data);

    Q_UNUSED(input_method);
    d->handleInputMethodDeactivate(context);
}

const input_method_listener maliit_input_method_listener = {
    inputMethodActivate,
    inputMethodDeactivate
};

void inputMethodContextSurroundingText(void *data,
                                       input_method_context *context,
                                       const char *text,
                                       uint32_t cursor,
                                       uint32_t anchor)
{
    qDebug() << __PRETTY_FUNCTION__;
    MInputContextWestonIMProtocolConnectionPrivate *d =
        static_cast<MInputContextWestonIMProtocolConnectionPrivate *>(data);

    Q_UNUSED(context);
    d->handleInputMethodContextSurroundingText(text, cursor, anchor);
}

void inputMethodContextReset(void *data,
                             input_method_context *context,
                             uint32_t serial)
{
    qDebug() << __PRETTY_FUNCTION__;
    MInputContextWestonIMProtocolConnectionPrivate *d =
        static_cast<MInputContextWestonIMProtocolConnectionPrivate *>(data);

    Q_UNUSED(context);
    d->handleInputMethodContextReset(serial);
}

void inputMethodContextContentType(void *data,
                                   input_method_context *context,
                                   uint32_t hint,
                                   uint32_t purpose)
{
    qDebug() << __PRETTY_FUNCTION__;
    MInputContextWestonIMProtocolConnectionPrivate *d =
        static_cast<MInputContextWestonIMProtocolConnectionPrivate *>(data);

    Q_UNUSED(context);
    d->handleInputMethodContextContentType(hint, purpose);
}

void inputMethodContextInvokeAction(void *data,
                                    input_method_context *context,
                                    uint32_t button,
                                    uint32_t index)
{
    qDebug() << __PRETTY_FUNCTION__;
    MInputContextWestonIMProtocolConnectionPrivate *d =
        static_cast<MInputContextWestonIMProtocolConnectionPrivate *>(data);

    Q_UNUSED(context);
    d->handleInputMethodContextInvokeAction(button, index);
}

const input_method_context_listener maliit_input_method_context_listener = {
    inputMethodContextSurroundingText,
    inputMethodContextReset,
    inputMethodContextContentType,
    inputMethodContextInvokeAction
};

text_model_preedit_style
face_to_uint (Maliit::PreeditFace face)
{
    switch (face) {
    case Maliit::PreeditDefault:
        return TEXT_MODEL_PREEDIT_STYLE_DEFAULT;

    case Maliit::PreeditNoCandidates:
        return TEXT_MODEL_PREEDIT_STYLE_INCORRECT;

    case Maliit::PreeditKeyPress:
        return TEXT_MODEL_PREEDIT_STYLE_HIGHLIGHT;

    case Maliit::PreeditUnconvertible:
        return TEXT_MODEL_PREEDIT_STYLE_INACTIVE;

    case Maliit::PreeditActive:
        return TEXT_MODEL_PREEDIT_STYLE_ACTIVE;

    default:
        return static_cast<text_model_preedit_style>(0); // none
    }
}

xkb_keysym_t qtKeyToXKBKey (int qt_key)
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

Maliit::TextContentType
westonPurposeToMaliit(text_model_content_purpose purpose)
{
    switch (purpose) {
    case TEXT_MODEL_CONTENT_PURPOSE_NORMAL:
        return Maliit::FreeTextContentType;

    //case TEXT_MODEL_CONTENT_PURPOSE_DIGITS:
    case TEXT_MODEL_CONTENT_PURPOSE_NUMBER:
        return Maliit::NumberContentType;

    case TEXT_MODEL_CONTENT_PURPOSE_PHONE:
        return Maliit::PhoneNumberContentType;

    case TEXT_MODEL_CONTENT_PURPOSE_URL:
        return Maliit::UrlContentType;

    case TEXT_MODEL_CONTENT_PURPOSE_EMAIL:
        return Maliit::EmailContentType;

    default:
        return Maliit::CustomContentType;
    }
}

bool
matchesFlag(int value,
            int flag)
{
    return ((value & flag) == flag);
}

} // unnamed namespace

MInputContextWestonIMProtocolConnectionPrivate::MInputContextWestonIMProtocolConnectionPrivate(MInputContextWestonIMProtocolConnection *connection)
    : q_ptr(connection),
      display(0),
      registry(0),
      im(0),
      im_context(0),
      im_serial(0),
      selection(),
      mods(),
      state_info()
{
    qDebug() << __PRETTY_FUNCTION__;
    display = static_cast<wl_display *>(QGuiApplication::platformNativeInterface()->nativeResourceForIntegration("display"));
    if (!display) {
        qCritical() << __PRETTY_FUNCTION__ << "Failed to get a display.";
        return;
    }
    registry = wl_display_get_registry(display);
    wl_registry_add_listener(registry, &maliit_registry_listener, this);
    // QtWayland will do dispatching for us.
}

MInputContextWestonIMProtocolConnectionPrivate::~MInputContextWestonIMProtocolConnectionPrivate()
{
    qDebug() << __PRETTY_FUNCTION__;
    if (im_context) {
        input_method_context_destroy(im_context);
    }
    if (display) {
        wl_display_disconnect(display);
    }
}

void MInputContextWestonIMProtocolConnectionPrivate::handleRegistryGlobal(uint32_t name,
                                                                          const char *interface,
                                                                          uint32_t version)
{
    Q_UNUSED(version);

    qDebug() << __PRETTY_FUNCTION__ << "Name:" << name << "Interface:" << interface;
    if (!strcmp(interface, "input_method")) {
        im = static_cast<input_method *>(wl_registry_bind(registry, name, &input_method_interface, 1));
        input_method_add_listener(im, &maliit_input_method_listener, this);
    }
}

void MInputContextWestonIMProtocolConnectionPrivate::handleRegistryGlobalRemove(uint32_t name)
{
    qDebug() << __PRETTY_FUNCTION__ << "Name:" << name;
}

void MInputContextWestonIMProtocolConnectionPrivate::handleInputMethodActivate(input_method_context *context,
                                                                               uint32_t serial)
{
    Q_Q(MInputContextWestonIMProtocolConnection);

    qDebug() << __PRETTY_FUNCTION__ << "context:" << static_cast<void *>(context) << "serial:" << serial;
    if (im_context) {
        input_method_context_destroy(im_context);
    }
    im_context = context;
    im_serial = serial;
    input_method_context_add_listener(im_context, &maliit_input_method_context_listener, this);

    input_method_context_modifiers_map(im_context, mods.getModMap());

    state_info[FocusStateAttribute] = true;
    q->updateWidgetInformation(connection_id, state_info, true);
    q->activateContext(connection_id);
    q->showInputMethod(connection_id);
}

void MInputContextWestonIMProtocolConnectionPrivate::handleInputMethodDeactivate(input_method_context *context)
{
    Q_Q(MInputContextWestonIMProtocolConnection);

    qDebug() << __PRETTY_FUNCTION__ << (long)context;
    if (!im_context) {
        return;
    }
    input_method_context_destroy(im_context);
    im_context = NULL;
    state_info.clear();
    state_info[FocusStateAttribute] = false;
    q->updateWidgetInformation(connection_id, state_info, true);
    q->hideInputMethod(connection_id);
    q->handleDisconnection(connection_id);
}

void MInputContextWestonIMProtocolConnectionPrivate::handleInputMethodContextSurroundingText(const char *text,
                                                                                             uint32_t cursor,
                                                                                             uint32_t anchor)
{
    Q_Q(MInputContextWestonIMProtocolConnection);

    qDebug() << __PRETTY_FUNCTION__ << "text:" << text << "cursor:" << cursor << "anchor:" << anchor;

    state_info[SurroundingTextAttribute] = QString(text);
    state_info[CursorPositionAttribute] = cursor;
    state_info[AnchorPositionAttribute] = anchor;
    state_info[HasSelectionAttribute] = (cursor != anchor);
    if (cursor == anchor) {
        selection.clear();
    } else {
        uint32_t begin(qMin(cursor, anchor));
        uint32_t end(qMax(cursor, anchor));

        selection = QString::fromUtf8(text + begin, end - begin);
    }
    q->updateWidgetInformation(connection_id, state_info, false);
}

void MInputContextWestonIMProtocolConnectionPrivate::handleInputMethodContextReset(uint32_t serial)
{
    Q_Q(MInputContextWestonIMProtocolConnection);

    qDebug() << __PRETTY_FUNCTION__ << "Serial:" << serial;
    im_serial = serial;
    q->reset(connection_id);
}

void MInputContextWestonIMProtocolConnectionPrivate::handleInputMethodContextContentType(uint32_t hint,
                                                                                         uint32_t purpose)
{
    Q_Q(MInputContextWestonIMProtocolConnection);

    state_info[ContentTypeAttribute] = westonPurposeToMaliit(static_cast<text_model_content_purpose>(purpose));
    state_info[AutoCapitalizationAttribute] = matchesFlag(hint, TEXT_MODEL_CONTENT_HINT_AUTO_CAPITALIZATION);
    state_info[CorrectionAttribute] = matchesFlag(hint, TEXT_MODEL_CONTENT_HINT_AUTO_CORRECTION);
    state_info[PredictionAttribute] = matchesFlag(hint, TEXT_MODEL_CONTENT_HINT_AUTO_COMPLETION);
    state_info[HiddenTextAttribute] = matchesFlag(hint, TEXT_MODEL_CONTENT_HINT_HIDDEN_TEXT);
    q->updateWidgetInformation(connection_id, state_info, false);
}

void MInputContextWestonIMProtocolConnectionPrivate::handleInputMethodContextInvokeAction(uint32_t button,
                                                                                          uint32_t index)
{
    qDebug() << __PRETTY_FUNCTION__ << "Button:" << button << "Index:" << index;
}

// MInputContextWestonIMProtocolConnection

MInputContextWestonIMProtocolConnection::MInputContextWestonIMProtocolConnection()
    : d_ptr(new MInputContextWestonIMProtocolConnectionPrivate(this))
{
    qDebug() << __PRETTY_FUNCTION__;
}

MInputContextWestonIMProtocolConnection::~MInputContextWestonIMProtocolConnection()
{
    qDebug() << __PRETTY_FUNCTION__;
}

void MInputContextWestonIMProtocolConnection::sendPreeditString(const QString &string,
                                                                const QList<Maliit::PreeditTextFormat> &preedit_formats,
                                                                int replace_start,
                                                                int replace_length,
                                                                int cursor_pos)
{
    qDebug() << __PRETTY_FUNCTION__
             << "Preedit:" << string
             << "replace start:" << replace_start
             << "replace length:" << replace_length
             << "cursor position:" << cursor_pos;
    Q_D(MInputContextWestonIMProtocolConnection);

    if (d->im_context) {
        MInputContextConnection::sendPreeditString(string, preedit_formats,
                                                   replace_start, replace_length,
                                                   cursor_pos);
        const QByteArray raw(string.toUtf8());

        if (replace_length > 0) {
            input_method_context_delete_surrounding_text(d->im_context, d->im_serial,
                                                         replace_start, replace_length);
        }
        Q_FOREACH (const Maliit::PreeditTextFormat& format, preedit_formats) {
            input_method_context_preedit_styling(d->im_context, d->im_serial,
                                                 format.start, format.length,
                                                 face_to_uint (format.preeditFace));
        }
        if (cursor_pos < 0) {
            cursor_pos = string.size() + 1 - cursor_pos;
        }
        input_method_context_preedit_cursor(d->im_context, d->im_serial,
                                            // convert from internal pos to byte pos
                                            string.left(cursor_pos).toUtf8().size());
        input_method_context_preedit_string(d->im_context, d->im_serial, raw.data(),
                                            raw.data());
    }
}


void MInputContextWestonIMProtocolConnection::sendCommitString(const QString &string,
                                                               int replace_start,
                                                               int replace_length,
                                                               int cursor_pos)
{
    qDebug() << __PRETTY_FUNCTION__
             << "commit:" << string
             << "replace start:" << replace_start
             << "replace length:" << replace_length
             << "cursor position:" << cursor_pos;
    Q_D(MInputContextWestonIMProtocolConnection);

    if (d->im_context) {
        MInputContextConnection::sendCommitString(string, replace_start, replace_length, cursor_pos);
        const QByteArray raw(string.toUtf8());

        if (cursor_pos < 0) {
            cursor_pos = string.size();
        }
        input_method_context_preedit_string(d->im_context, d->im_serial, "", "");
        input_method_context_delete_surrounding_text(d->im_context, d->im_serial,
                                                     replace_start, replace_length);
        input_method_context_commit_string(d->im_context, d->im_serial, raw.data(),
                                           string.left(cursor_pos).toUtf8().size());
    }
}


void MInputContextWestonIMProtocolConnection::sendKeyEvent(const QKeyEvent &keyEvent,
                                                           Maliit::EventRequestType requestType)
{
    Q_D(MInputContextWestonIMProtocolConnection);

    if (d->im_context) {
        xkb_keysym_t key_sym(qtKeyToXKBKey(keyEvent.key()));

        if (!key_sym) {
            qWarning() << "No conversion from Qt::Key:" << keyEvent.key() << "to XKB key. Update the qtKeyToXKBKey function.";
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
            qWarning() << "Unknown QKeyEvent type:" << keyEvent.type();
            return;
        }

        xkb_mod_mask_t mod_mask(d->mods.fromQt(keyEvent.modifiers()));

        MInputContextConnection::sendKeyEvent(keyEvent, requestType);

        qDebug() << "key_sym:" << key_sym << "state:" << state << "mod mask:" << mod_mask;

        input_method_context_keysym(d->im_context, d->im_serial, keyEvent.timestamp(),
                                    key_sym, state, mod_mask);
    }
}

/* Rather not yet implemented in Weston.
void MInputContextWestonIMProtocolConnection::notifyImInitiatedHiding()
{
    Q_D(MInputContextWestonIMProtocol);

    if (d->im_context) {
        input_method_context_destroy(d->im_context);
        d->im_context = 0;
    }
}

void MInputContextWestonIMProtocolConnection::setGlobalCorrectionEnabled(bool enabled)
{
    if ((enabled != globalCorrectionEnabled()) && activeContext()) {
        dbus_g_proxy_call_no_reply(activeContext()->inputContextProxy, "setGlobalCorrectionEnabled",
                                   G_TYPE_BOOLEAN, enabled,
                                   G_TYPE_INVALID);

        MInputContextConnection::setGlobalCorrectionEnabled(enabled);
    }
}
*/

QString MInputContextWestonIMProtocolConnection::selection(bool &valid)
{
    Q_D(MInputContextWestonIMProtocolConnection);

    valid = !d->selection.isEmpty();
    return d->selection;
}

/* Not implemented in Weston
void MInputContextWestonIMProtocolConnection::setLanguage(const QString &language)
{
    lastLanguage = language;
    setLanguage(activeContext(), language);
}

void MInputContextWestonIMProtocolConnection::setLanguage(MDBusGlibICConnection *targetIcConnection,
                                                  const QString &language)
{
    if (targetIcConnection) {
        dbus_g_proxy_call_no_reply(targetIcConnection->inputContextProxy, "setLanguage",
                                   G_TYPE_STRING, language.toUtf8().data(),
                                   G_TYPE_INVALID);
    }
}

QRect MInputContextWestonIMProtocolConnection::preeditRectangle(bool &valid)
{
    GError *error = NULL;

    gboolean gvalidity;
    gint32 x, y, width, height;

    if (activeContext() &&
        dbus_g_proxy_call(activeContext()->inputContextProxy, "preeditRectangle", &error, G_TYPE_INVALID,
                          G_TYPE_BOOLEAN, &gvalidity, G_TYPE_INT, &x, G_TYPE_INT, &y,
                          G_TYPE_INT, &width, G_TYPE_INT, &height, G_TYPE_INVALID)) {
        valid = gvalidity == TRUE;
    } else {
        if (error) { // dbus_g_proxy_call may return FALSE and not set error despite what the doc says
            g_error_free(error);
        }
        valid = false;
        return QRect();
    }

    return QRect(x, y, width, height);
}

void MInputContextWestonIMProtocolConnection::setRedirectKeys(bool enabled)
{
    if ((redirectKeysEnabled() != enabled) && activeContext()) {
        dbus_g_proxy_call_no_reply(activeContext()->inputContextProxy, "setRedirectKeys",
                                   G_TYPE_BOOLEAN, enabled ? TRUE : FALSE,
                                   G_TYPE_INVALID);

        MInputContextConnection::setRedirectKeys(enabled);
    }
}

void MInputContextWestonIMProtocolConnection::setDetectableAutoRepeat(bool enabled)
{
    if ((detectableAutoRepeat() != enabled) && activeContext()) {
        dbus_g_proxy_call_no_reply(activeContext()->inputContextProxy, "setDetectableAutoRepeat",
                                   G_TYPE_BOOLEAN, enabled,
                                   G_TYPE_INVALID);

        MInputContextConnection::setDetectableAutoRepeat(enabled);
    }
}

void MInputContextWestonIMProtocolConnection::invokeAction(const QString &action, const QKeySequence &sequence)
{
    if (activeContext()) {
        DBusMessage *message = dbus_message_new_signal(DBusPath,
                                                       "com.meego.inputmethod.uiserver1",
                                                       "invokeAction");
        char *action_string = strdup(action.toUtf8().constData());
        char *sequence_string = strdup(sequence.toString(QKeySequence::PortableText).toUtf8().constData());
        dbus_message_append_args(message,
                                 DBUS_TYPE_STRING, &action_string,
                                 DBUS_TYPE_STRING, &sequence_string,
                                 DBUS_TYPE_INVALID);
        free(action_string);
        free(sequence_string);
        dbus_connection_send(dbus_g_connection_get_connection(activeContext()->dbusConnection),
                             message,
                             NULL);
        dbus_message_unref(message);
    }
}

void MInputContextWestonIMProtocolConnection::setSelection(int start, int length)
{
    if (activeContext()) {
        dbus_g_proxy_call_no_reply(activeContext()->inputContextProxy, "setSelection",
                                   G_TYPE_INT, start,
                                   G_TYPE_INT, length,
                                   G_TYPE_INVALID);
    }
}

void MInputContextWestonIMProtocolConnection::updateInputMethodArea(const QRegion &region)
{
    if (activeContext()) {
        QRect rect = region.boundingRect();
        dbus_g_proxy_call_no_reply(activeContext()->inputContextProxy, "updateInputMethodArea",
                                   G_TYPE_INT, rect.left(),
                                   G_TYPE_INT, rect.top(),
                                   G_TYPE_INT, rect.width(),
                                   G_TYPE_INT, rect.height(),
                                   G_TYPE_INVALID);
    }
}

void MInputContextWestonIMProtocolConnection::notifyExtendedAttributeChanged(int id,
                                                                     const QString &target,
                                                                     const QString &targetItem,
                                                                     const QString &attribute,
                                                                     const QVariant &value)
{
    if (!activeContext()) {
        return;
    }
    GValue valueData = {0, {{0}, {0}}};
    if (!encodeVariant(&valueData, value)) {
        return;
    }

    dbus_g_proxy_call_no_reply(activeContext()->inputContextProxy, "notifyExtendedAttributeChanged",
                               G_TYPE_INT, id,
                               G_TYPE_STRING, target.toUtf8().data(),
                               G_TYPE_STRING, targetItem.toUtf8().data(),
                               G_TYPE_STRING, attribute.toUtf8().data(),
                               G_TYPE_VALUE, &valueData,
                               G_TYPE_INVALID);
    g_value_unset(&valueData);
}

void MInputContextWestonIMProtocolConnection::notifyExtendedAttributeChanged(const QList<int> &clientIds,
                                                                     int id,
                                                                     const QString &target,
                                                                     const QString &targetItem,
                                                                     const QString &attribute,
                                                                     const QVariant &value)
{
    GValue valueData = {0, {{0}, {0}}};
    if (!encodeVariant(&valueData, value)) {
        return;
    }
    Q_FOREACH (int clientId, clientIds) {
        dbus_g_proxy_call_no_reply(connectionObj(clientId)->inputContextProxy, "notifyExtendedAttributeChanged",
                                   G_TYPE_INT, id,
                                   G_TYPE_STRING, target.toUtf8().data(),
                                   G_TYPE_STRING, targetItem.toUtf8().data(),
                                   G_TYPE_STRING, attribute.toUtf8().data(),
                                   G_TYPE_VALUE, &valueData,
                                   G_TYPE_INVALID);
    }
    g_value_unset(&valueData);
}


void MInputContextWestonIMProtocolConnection::pluginSettingsLoaded(int clientId, const QList<MImPluginSettingsInfo> &info)
{
    MDBusGlibICConnection *client = connectionObj(clientId);
    if (!client) {
        return;
    }

    GType settingsType;
    GPtrArray *settingsData;
    if (!encodeSettings(&settingsType, &settingsData, info)) {
        return;
    }

    dbus_g_proxy_call_no_reply(client->inputContextProxy, "pluginSettingsLoaded",
                               settingsType, settingsData,
                               G_TYPE_INVALID);
    dbus_g_type_collection_peek_vtable(settingsType)->base_vtable.free_func(settingsType, settingsData);
}


void MInputContextWestonIMProtocolConnection::sendActivationLostEvent()
{
    if (activeContext()) {
        dbus_g_proxy_call_no_reply(activeContext()->inputContextProxy, "activationLostEvent",
                                   G_TYPE_INVALID);
    }
}
*/
