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

#include "glibdbusimserverproxy.h"
#include "mdbusglibinputcontextadaptor.h"

#include <QPoint>
#include <QRect>
#include <QString>
#include <QDataStream>
#include <QVariant>
#include <QTimer>
#include <QDateTime>
#include <QDebug>

#include <unistd.h>
#include <sys/types.h>

namespace
{
    const char * const DBusPath("/com/meego/inputmethod/uiserver1");
    const char * const DBusInterface("com.meego.inputmethod.uiserver1");
    const int ConnectionRetryInterval(6*1000); // in ms
    const QString icAdaptorPath("/com/meego/inputmethod/inputcontext");

    Maliit::DBusGLib::ConnectionRef toRef(DBusGConnection *connection)
    {
        if (!connection)
            return Maliit::DBusGLib::ConnectionRef();

        return Maliit::DBusGLib::ConnectionRef(connection,
                                               dbus_g_connection_unref);
    }

    bool encodeVariant(GValue *dest, const QVariant &source)
    {
        switch (static_cast<QMetaType::Type>(source.type())) {
        case QMetaType::Bool:
            g_value_init(dest, G_TYPE_BOOLEAN);
            g_value_set_boolean(dest, source.toBool());
            return true;
        case QMetaType::Int:
            g_value_init(dest, G_TYPE_INT);
            g_value_set_int(dest, source.toInt());
            return true;
        case QMetaType::UInt:
            g_value_init(dest, G_TYPE_UINT);
            g_value_set_uint(dest, source.toUInt());
            return true;
        case QMetaType::LongLong:
            g_value_init(dest, G_TYPE_INT64);
            g_value_set_int64(dest, source.toLongLong());
            return true;
        case QMetaType::ULongLong:
            g_value_init(dest, G_TYPE_UINT64);
            g_value_set_uint64(dest, source.toULongLong());
            return true;
        case QMetaType::Double:
            g_value_init(dest, G_TYPE_DOUBLE);
            g_value_set_double(dest, source.toDouble());
            return true;
        case QMetaType::QString:
            g_value_init(dest, G_TYPE_STRING);
            // string is copied by g_value_set_string
            g_value_set_string(dest, source.toString().toUtf8().constData());
            return true;
        case QMetaType::QRect:
            {
                // QRect is encoded as (iiii).
                // This is compatible with QDBusArgument encoding. see more in decoder
                GType structType = dbus_g_type_get_struct("GValueArray",
                                                          G_TYPE_INT, G_TYPE_INT,
                                                          G_TYPE_INT, G_TYPE_INT,
                                                          G_TYPE_INVALID);
                g_value_init(dest, structType);
                GValueArray *array = (GValueArray*)dbus_g_type_specialized_construct(structType);
                if (!array) {
                    qWarning() << Q_FUNC_INFO << "failed to initialize Rect instance";
                }
                g_value_take_boxed(dest, array);
                QRect rect = source.toRect();
                if (!dbus_g_type_struct_set(dest,
                                        0, rect.left(),
                                        1, rect.top(),
                                        2, rect.width(),
                                        3, rect.height(),
                                        G_MAXUINT)) {
                    g_value_unset(dest);
                    qWarning() << Q_FUNC_INFO << "failed to fill Rect instance";
                    return false;
                }
                return true;
            }
        case QMetaType::ULong:
            g_value_init(dest, G_TYPE_ULONG);
            g_value_set_ulong(dest, source.value<ulong>());
            return true;
        default:
            qWarning() << Q_FUNC_INFO << "unsupported data:" << source.type() << source;
            return false;
        }
    }

    void destroyGValue(GValue *value)
    {
        g_value_unset(value);
        g_free(value);
    }

    GHashTable *encodeVariantMap(const QMap<QString, QVariant> &source)
    {
        GHashTable* result = g_hash_table_new_full(&g_str_hash, &g_str_equal,
                                                   &g_free, GDestroyNotify(&destroyGValue));
        Q_FOREACH (QString key, source.keys()) {
            GValue *valueVariant = g_new0(GValue, 1);
            if (!encodeVariant(valueVariant, source[key])) {
                g_free(valueVariant);
                g_hash_table_unref(result);
                return 0;
            }
            g_hash_table_insert(result, g_strdup(key.toUtf8().constData()), valueVariant);
        }
        return result;
    }


    static bool debugEnabled()
    {
        static bool enabled = false;
        static bool initialized = false;

        if (initialized) {
            return enabled;
        }

        QByteArray debugEnvVar = qgetenv("MIC_ENABLE_DEBUG");
        if (!debugEnvVar.isEmpty() && debugEnvVar != "false") {
            enabled = true;
        }

        initialized = true;
        return enabled;
    }

}

GlibDBusIMServerProxy::GlibDBusIMServerProxy(QObject *parent)
    : glibObjectProxy(NULL),
      connection(),
      active(true),
      mAddress(new Maliit::InputContext::DBus::Address)
{
    Q_UNUSED(parent);

    g_type_init();

    MDBusGlibInputContextAdaptor *adaptor = M_DBUS_GLIB_INPUT_CONTEXT_ADAPTOR(
            g_object_new(M_TYPE_DBUS_GLIB_INPUT_CONTEXT_ADAPTOR, NULL));
    adaptor->imServerConnection = this;

    inputContextAdaptor = G_OBJECT(adaptor);

    dbus_g_thread_init();

    QTimer::singleShot(0, this, SLOT(connectToDBus()));
}

GlibDBusIMServerProxy::~GlibDBusIMServerProxy()
{
    active = false;

    Q_FOREACH (DBusGProxyCall *callId, pendingResetCalls) {
        dbus_g_proxy_cancel_call(glibObjectProxy, callId);
    }
}


// Auxiliary connection handling.............................................

void GlibDBusIMServerProxy::onDisconnectionTrampoline(DBusGProxy */*proxy*/, gpointer userData)
{
    if (debugEnabled()) qDebug() << "MInputContext" << __PRETTY_FUNCTION__;
    static_cast<GlibDBusIMServerProxy *>(userData)->onDisconnection();
}

void GlibDBusIMServerProxy::connectToDBus()
{
    if (debugEnabled()) qDebug() << "MInputContext" << __PRETTY_FUNCTION__;

    GError *error = NULL;

    const std::string &address = mAddress->get();
    if (address.empty()) {
        QTimer::singleShot(ConnectionRetryInterval, this, SLOT(connectToDBus()));
        return;
    }

    connection = toRef(dbus_g_connection_open(address.c_str(), &error));
    if (!connection) {
        if (error) {
            qWarning("MInputContext: unable to create D-Bus connection: %s", error->message);
            g_error_free(error);
        }
        QTimer::singleShot(ConnectionRetryInterval, this, SLOT(connectToDBus()));
        return;
    }

    glibObjectProxy = dbus_g_proxy_new_for_peer(connection.get(), DBusPath, DBusInterface);
    if (!glibObjectProxy) {
        qWarning("MInputContext: unable to find the D-Bus service.");
        connection.reset();
        QTimer::singleShot(ConnectionRetryInterval, this, SLOT(connectToDBus()));
        return;
    }
    g_signal_connect(G_OBJECT(glibObjectProxy), "destroy", G_CALLBACK(onDisconnectionTrampoline),
                     this);

    dbus_g_connection_register_g_object(connection.get(), icAdaptorPath.toAscii().data(), inputContextAdaptor);

    Q_EMIT connected();
}

void GlibDBusIMServerProxy::onDisconnection()
{
    if (debugEnabled()) qDebug() << "MInputContext" << __PRETTY_FUNCTION__;

    glibObjectProxy = 0;
    connection.reset();
    Q_EMIT disconnected();

    if (active) {
        QTimer::singleShot(ConnectionRetryInterval, this, SLOT(connectToDBus()));
    }
}

void GlibDBusIMServerProxy::resetNotifyTrampoline(DBusGProxy *proxy, DBusGProxyCall *callId,
                                                  gpointer userData)
{
    static_cast<GlibDBusIMServerProxy *>(userData)->resetNotify(proxy, callId);
}

void GlibDBusIMServerProxy::resetNotify(DBusGProxy *proxy, DBusGProxyCall *callId)
{
    if (debugEnabled()) qDebug() << "MInputContext" << __PRETTY_FUNCTION__;

    dbus_g_proxy_end_call(proxy, callId, 0, G_TYPE_INVALID);
    pendingResetCalls.remove(callId);
}


// Remote methods............................................................

void GlibDBusIMServerProxy::activateContext()
{
    if (!glibObjectProxy) {
        return;
    }
    dbus_g_proxy_call_no_reply(glibObjectProxy, "activateContext",
                               G_TYPE_INVALID);
}

void GlibDBusIMServerProxy::showInputMethod()
{
    if (!glibObjectProxy) {
        return;
    }
    dbus_g_proxy_call_no_reply(glibObjectProxy, "showInputMethod",
                               G_TYPE_INVALID);
}

void GlibDBusIMServerProxy::hideInputMethod()
{
    if (!glibObjectProxy) {
        return;
    }
    dbus_g_proxy_call_no_reply(glibObjectProxy, "hideInputMethod",
                               G_TYPE_INVALID);
}

void GlibDBusIMServerProxy::mouseClickedOnPreedit(const QPoint &pos, const QRect &preeditRect)
{
    if (!glibObjectProxy) {
        return;
    }
    dbus_g_proxy_call_no_reply(glibObjectProxy, "mouseClickedOnPreedit",
                               G_TYPE_INT, pos.x(),
                               G_TYPE_INT, pos.y(),
                               G_TYPE_INT, preeditRect.x(),
                               G_TYPE_INT, preeditRect.y(),
                               G_TYPE_INT, preeditRect.width(),
                               G_TYPE_INT, preeditRect.height(),
                               G_TYPE_INVALID);
}

void GlibDBusIMServerProxy::setPreedit(const QString &text, int cursorPos)
{
    if (!glibObjectProxy) {
        return;
    }
    dbus_g_proxy_call_no_reply(glibObjectProxy, "setPreedit",
                               G_TYPE_STRING, text.toUtf8().data(),
                               G_TYPE_INT, cursorPos,
                               G_TYPE_INVALID);
}

void GlibDBusIMServerProxy::updateWidgetInformation(const QMap<QString, QVariant> &stateInformation,
                                                    bool focusChanged)
{
    if (!glibObjectProxy) {
        return;
    }
    GHashTable *encodedState = encodeVariantMap(stateInformation);
    if (encodedState == 0)
        return;

    GType encodedStateType = dbus_g_type_get_map("GHashTable", G_TYPE_STRING, G_TYPE_VALUE);
    dbus_g_proxy_call_no_reply(glibObjectProxy, "updateWidgetInformation",
                               encodedStateType, encodedState,
                               G_TYPE_BOOLEAN, focusChanged,
                               G_TYPE_INVALID);
    g_hash_table_unref(encodedState);
}

void GlibDBusIMServerProxy::reset(bool requireSynchronization)
{
    if (!glibObjectProxy) {
        return;
    }

    if (requireSynchronization) {
        DBusGProxyCall *resetCall = dbus_g_proxy_begin_call(glibObjectProxy, "reset",
                                                            resetNotifyTrampoline, this,
                                                            0, G_TYPE_INVALID);
        pendingResetCalls.insert(resetCall);
    } else {
        dbus_g_proxy_call_no_reply(glibObjectProxy, "reset",
                                   G_TYPE_INVALID);
    }
}

bool GlibDBusIMServerProxy::pendingResets()
{
    return (pendingResetCalls.size() > 0);
}

void GlibDBusIMServerProxy::appOrientationAboutToChange(int angle)
{
    if (!glibObjectProxy) {
        return;
    }
    dbus_g_proxy_call_no_reply(glibObjectProxy, "appOrientationAboutToChange",
                               G_TYPE_INT, angle,
                               G_TYPE_INVALID);
}

void GlibDBusIMServerProxy::appOrientationChanged(int angle)
{
    if (!glibObjectProxy) {
        return;
    }
    dbus_g_proxy_call_no_reply(glibObjectProxy, "appOrientationChanged",
                               G_TYPE_INT, angle,
                               G_TYPE_INVALID);
}

void GlibDBusIMServerProxy::setCopyPasteState(bool copyAvailable, bool pasteAvailable)
{
    if (!glibObjectProxy) {
        return;
    }
    dbus_g_proxy_call_no_reply(glibObjectProxy, "setCopyPasteState",
                               G_TYPE_BOOLEAN, copyAvailable,
                               G_TYPE_BOOLEAN, pasteAvailable,
                               G_TYPE_INVALID);
}

void GlibDBusIMServerProxy::processKeyEvent(QEvent::Type keyType, Qt::Key keyCode,
                                            Qt::KeyboardModifiers modifiers,
                                            const QString &text, bool autoRepeat, int count,
                                            quint32 nativeScanCode, quint32 nativeModifiers,
                                            unsigned long time)
{
    if (!glibObjectProxy) {
        return;
    }
    dbus_g_proxy_call_no_reply(glibObjectProxy, "processKeyEvent",
                               G_TYPE_INT, static_cast<int>(keyType),
                               G_TYPE_INT, static_cast<int>(keyCode),
                               G_TYPE_INT, static_cast<int>(modifiers),
                               G_TYPE_STRING, text.toUtf8().data(),
                               G_TYPE_BOOLEAN, autoRepeat, G_TYPE_INT, count,
                               G_TYPE_UINT, nativeScanCode, G_TYPE_UINT, nativeModifiers,
                               G_TYPE_ULONG, time,
                               G_TYPE_INVALID);
}

void GlibDBusIMServerProxy::registerAttributeExtension(int id, const QString &fileName)
{
    if (!glibObjectProxy) {
        return;
    }
    dbus_g_proxy_call_no_reply(glibObjectProxy, "registerAttributeExtension",
                               G_TYPE_INT, id,
                               G_TYPE_STRING, fileName.toUtf8().data(),
                               G_TYPE_INVALID);
}

void GlibDBusIMServerProxy::unregisterAttributeExtension(int id)
{
    if (!glibObjectProxy) {
        return;
    }
    dbus_g_proxy_call_no_reply(glibObjectProxy, "unregisterAttributeExtension",
                               G_TYPE_INT, id,
                               G_TYPE_INVALID);
}

void GlibDBusIMServerProxy::setExtendedAttribute(int id, const QString &target, const QString &targetItem,
                                                 const QString &attribute, const QVariant &value)
{
    if (!glibObjectProxy) {
        return;
    }
    GValue valueData = {0, {{0}, {0}}};
    if (!encodeVariant(&valueData, value)) {
        return;
    }

    dbus_g_proxy_call_no_reply(glibObjectProxy, "setExtendedAttribute",
                               G_TYPE_INT, id,
                               G_TYPE_STRING, target.toUtf8().data(),
                               G_TYPE_STRING, targetItem.toUtf8().data(),
                               G_TYPE_STRING, attribute.toUtf8().data(),
                               G_TYPE_VALUE, &valueData,
                               G_TYPE_INVALID);
    g_value_unset(&valueData);
}
