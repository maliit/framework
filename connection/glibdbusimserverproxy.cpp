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
#include <variantmarshalling.h>

#include "glibdbusimserverproxy_p.h"

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

#include <dbus/dbus.h>

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

    void resetNotifyTrampoline(DBusGProxy *proxy, DBusGProxyCall *callId, gpointer userData)
    {
        static_cast<GlibDBusIMServerProxyPrivate *>(userData)->resetNotify(proxy, callId);
    }
}


GlibDBusIMServerProxyPrivate::GlibDBusIMServerProxyPrivate()
    : glibObjectProxy(NULL),
      connection(),
      active(true),
      mAddress(new Maliit::InputContext::DBus::Address)
{
    g_type_init();
}

GlibDBusIMServerProxyPrivate::~GlibDBusIMServerProxyPrivate()
{}

void GlibDBusIMServerProxyPrivate::resetNotify(DBusGProxy *proxy, DBusGProxyCall *callId)
{
    if (debugEnabled()) qDebug() << "MInputContext" << __PRETTY_FUNCTION__;

    dbus_g_proxy_end_call(proxy, callId, 0, G_TYPE_INVALID);
    pendingResetCalls.remove(callId);
}


void GlibDBusIMServerProxy::onDisconnectionTrampoline(void */*proxy*/, void *userData)
{
    if (debugEnabled()) qDebug() << "MInputContext" << __PRETTY_FUNCTION__;
    static_cast<GlibDBusIMServerProxy *>(userData)->onDisconnection();
}


GlibDBusIMServerProxy::GlibDBusIMServerProxy(QObject *parent)
    : MImServerConnection(parent),
    d_ptr(new GlibDBusIMServerProxyPrivate)
{
    Q_UNUSED(parent);
    Q_D(GlibDBusIMServerProxy);

    MDBusGlibInputContextAdaptor *adaptor = M_DBUS_GLIB_INPUT_CONTEXT_ADAPTOR(
            g_object_new(M_TYPE_DBUS_GLIB_INPUT_CONTEXT_ADAPTOR, NULL));
    adaptor->imServerConnection = this;

    d->inputContextAdaptor = G_OBJECT(adaptor);

    dbus_g_thread_init();

    QTimer::singleShot(0, this, SLOT(connectToDBus()));
}

GlibDBusIMServerProxy::~GlibDBusIMServerProxy()
{
    Q_D(GlibDBusIMServerProxy);

    d->active = false;

    Q_FOREACH (DBusGProxyCall *callId, d->pendingResetCalls) {
        dbus_g_proxy_cancel_call(d->glibObjectProxy, callId);
    }
}


// Auxiliary connection handling.............................................

void GlibDBusIMServerProxy::connectToDBus()
{
    Q_D(GlibDBusIMServerProxy);

    if (debugEnabled()) qDebug() << "MInputContext" << __PRETTY_FUNCTION__;


    const std::string &address = d->mAddress->get();
    if (address.empty()) {
        QTimer::singleShot(ConnectionRetryInterval, this, SLOT(connectToDBus()));
        return;
    }

    DBusError error;
    dbus_error_init(&error);

    // Input contexts should not share the connection to the maliit server with
    // each other (even not when they are in the same application). Therefore,
    // use private connection for IC to server connection.
    DBusConnection *dbusConnection = dbus_connection_open_private(address.c_str(), &error);
    if (!dbusConnection) {
        qWarning("MInputContext: unable to create D-Bus connection: %s", error.message);
        dbus_error_free(&error);
        QTimer::singleShot(ConnectionRetryInterval, this, SLOT(connectToDBus()));
        return;
    }

    dbus_connection_setup_with_g_main(dbusConnection, 0);
    d->connection = toRef(dbus_connection_get_g_connection(dbusConnection));

    d->glibObjectProxy = dbus_g_proxy_new_for_peer(d->connection.get(), DBusPath, DBusInterface);
    if (!d->glibObjectProxy) {
        qWarning("MInputContext: unable to find the D-Bus service.");
        d->connection.reset();
        QTimer::singleShot(ConnectionRetryInterval, this, SLOT(connectToDBus()));
        return;
    }
    g_signal_connect(G_OBJECT(d->glibObjectProxy), "destroy", G_CALLBACK(onDisconnectionTrampoline),
                     this);

    dbus_g_connection_register_g_object(d->connection.get(), icAdaptorPath.toAscii().data(), d->inputContextAdaptor);

    Q_EMIT connected();
}

void GlibDBusIMServerProxy::onDisconnection()
{
    Q_D(GlibDBusIMServerProxy);

    if (debugEnabled()) qDebug() << "MInputContext" << __PRETTY_FUNCTION__;

    d->glibObjectProxy = 0;
    d->connection.reset();
    Q_EMIT disconnected();

    if (d->active) {
        QTimer::singleShot(ConnectionRetryInterval, this, SLOT(connectToDBus()));
    }
}

// Remote methods............................................................

void GlibDBusIMServerProxy::activateContext()
{
    Q_D(GlibDBusIMServerProxy);

    if (!d->glibObjectProxy) {
        return;
    }
    dbus_g_proxy_call_no_reply(d->glibObjectProxy, "activateContext",
                               G_TYPE_INVALID);
}

void GlibDBusIMServerProxy::showInputMethod()
{
    Q_D(GlibDBusIMServerProxy);

    if (!d->glibObjectProxy) {
        return;
    }
    dbus_g_proxy_call_no_reply(d->glibObjectProxy, "showInputMethod",
                               G_TYPE_INVALID);
}

void GlibDBusIMServerProxy::hideInputMethod()
{
    Q_D(GlibDBusIMServerProxy);

    if (!d->glibObjectProxy) {
        return;
    }
    dbus_g_proxy_call_no_reply(d->glibObjectProxy, "hideInputMethod",
                               G_TYPE_INVALID);
}

void GlibDBusIMServerProxy::mouseClickedOnPreedit(const QPoint &pos, const QRect &preeditRect)
{
    Q_D(GlibDBusIMServerProxy);

    if (!d->glibObjectProxy) {
        return;
    }
    dbus_g_proxy_call_no_reply(d->glibObjectProxy, "mouseClickedOnPreedit",
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
    Q_D(GlibDBusIMServerProxy);

    if (!d->glibObjectProxy) {
        return;
    }
    dbus_g_proxy_call_no_reply(d->glibObjectProxy, "setPreedit",
                               G_TYPE_STRING, text.toUtf8().data(),
                               G_TYPE_INT, cursorPos,
                               G_TYPE_INVALID);
}

void GlibDBusIMServerProxy::updateWidgetInformation(const QMap<QString, QVariant> &stateInformation,
                                                    bool focusChanged)
{
    Q_D(GlibDBusIMServerProxy);

    if (!d->glibObjectProxy) {
        return;
    }
    GHashTable *encodedState = encodeVariantMap(stateInformation);
    if (encodedState == 0)
        return;

    GType encodedStateType = dbus_g_type_get_map("GHashTable", G_TYPE_STRING, G_TYPE_VALUE);
    dbus_g_proxy_call_no_reply(d->glibObjectProxy, "updateWidgetInformation",
                               encodedStateType, encodedState,
                               G_TYPE_BOOLEAN, focusChanged,
                               G_TYPE_INVALID);
    g_hash_table_unref(encodedState);
}

void GlibDBusIMServerProxy::reset(bool requireSynchronization)
{
    Q_D(GlibDBusIMServerProxy);

    if (!d->glibObjectProxy) {
        return;
    }

    if (requireSynchronization) {
        DBusGProxyCall *resetCall = dbus_g_proxy_begin_call(d->glibObjectProxy, "reset",
                                                            resetNotifyTrampoline, d,
                                                            0, G_TYPE_INVALID);
        d->pendingResetCalls.insert(resetCall);
    } else {
        dbus_g_proxy_call_no_reply(d->glibObjectProxy, "reset",
                                   G_TYPE_INVALID);
    }
}

bool GlibDBusIMServerProxy::pendingResets()
{
    Q_D(GlibDBusIMServerProxy);

    return (d->pendingResetCalls.size() > 0);
}

void GlibDBusIMServerProxy::appOrientationAboutToChange(int angle)
{
    Q_D(GlibDBusIMServerProxy);

    if (!d->glibObjectProxy) {
        return;
    }
    dbus_g_proxy_call_no_reply(d->glibObjectProxy, "appOrientationAboutToChange",
                               G_TYPE_INT, angle,
                               G_TYPE_INVALID);
}

void GlibDBusIMServerProxy::appOrientationChanged(int angle)
{
    Q_D(GlibDBusIMServerProxy);

    if (!d->glibObjectProxy) {
        return;
    }
    dbus_g_proxy_call_no_reply(d->glibObjectProxy, "appOrientationChanged",
                               G_TYPE_INT, angle,
                               G_TYPE_INVALID);
}

void GlibDBusIMServerProxy::setCopyPasteState(bool copyAvailable, bool pasteAvailable)
{
    Q_D(GlibDBusIMServerProxy);

    if (!d->glibObjectProxy) {
        return;
    }
    dbus_g_proxy_call_no_reply(d->glibObjectProxy, "setCopyPasteState",
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
    Q_D(GlibDBusIMServerProxy);

    if (!d->glibObjectProxy) {
        return;
    }
    dbus_g_proxy_call_no_reply(d->glibObjectProxy, "processKeyEvent",
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
    Q_D(GlibDBusIMServerProxy);

    if (!d->glibObjectProxy) {
        return;
    }
    dbus_g_proxy_call_no_reply(d->glibObjectProxy, "registerAttributeExtension",
                               G_TYPE_INT, id,
                               G_TYPE_STRING, fileName.toUtf8().data(),
                               G_TYPE_INVALID);
}

void GlibDBusIMServerProxy::unregisterAttributeExtension(int id)
{
    Q_D(GlibDBusIMServerProxy);

    if (!d->glibObjectProxy) {
        return;
    }
    dbus_g_proxy_call_no_reply(d->glibObjectProxy, "unregisterAttributeExtension",
                               G_TYPE_INT, id,
                               G_TYPE_INVALID);
}

void GlibDBusIMServerProxy::setExtendedAttribute(int id, const QString &target, const QString &targetItem,
                                                 const QString &attribute, const QVariant &value)
{
    Q_D(GlibDBusIMServerProxy);

    if (!d->glibObjectProxy) {
        return;
    }
    GValue valueData = {0, {{0}, {0}}};
    if (!encodeVariant(&valueData, value)) {
        return;
    }

    dbus_g_proxy_call_no_reply(d->glibObjectProxy, "setExtendedAttribute",
                               G_TYPE_INT, id,
                               G_TYPE_STRING, target.toUtf8().data(),
                               G_TYPE_STRING, targetItem.toUtf8().data(),
                               G_TYPE_STRING, attribute.toUtf8().data(),
                               G_TYPE_VALUE, &valueData,
                               G_TYPE_INVALID);
    g_value_unset(&valueData);
}
