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

#include <QPoint>
#include <QRect>
#include <QString>
#include <QByteArray>
#include <QDataStream>
#include <QVariant>
#include <QTimer>
#include <QDateTime>

#include <MDebug>

#include <unistd.h>
#include <sys/types.h>

namespace
{
    const char * const DBusPath("/com/meego/inputmethod/uiserver1");
    const char * const DBusInterface("com.meego.inputmethod.uiserver1");
    const char * const ActivationBusName("com.meego.inputmethod.uiserver1");
    const char * const ActivationPath("/com/meego/inputmethod/activation");
    const char * const ActivationInterface("com.meego.inputmethod.uiserveractivation");
    const int ConnectionRetryInterval(6*1000); // in ms
}


GlibDBusIMServerProxy::GlibDBusIMServerProxy(GObject *inputContextAdaptor, const QString &icAdaptorPath)
    : glibObjectProxy(NULL),
      connection(NULL),
      inputContextAdaptor(inputContextAdaptor),
      icAdaptorPath(icAdaptorPath),
      active(true),
      activationProxy(NULL),
      sessionBusConnection(NULL)
{
    dbus_g_thread_init();
    g_type_init();

    connectToActivationService();
    connect();
}

GlibDBusIMServerProxy::~GlibDBusIMServerProxy()
{
    active = false;
    // Proxies should be taken care of automatically
    if (connection) {
        dbus_g_connection_unref(connection);
    }
    if (sessionBusConnection) {
        dbus_g_connection_unref(sessionBusConnection);
    }
}


// Auxiliary connection handling.............................................

void GlibDBusIMServerProxy::connectToActivationService()
{
    GError *error = NULL;

    sessionBusConnection = dbus_g_bus_get(DBUS_BUS_SESSION, &error);
    if (!sessionBusConnection) {
        qWarning("MInputContext: unable to create session D-Bus connection: %s", error->message);
        g_error_free(error);
        return;
    }

    activationProxy = dbus_g_proxy_new_for_name(sessionBusConnection, ActivationBusName,
                                                ActivationPath, ActivationInterface);
    if (!activationProxy) {
        qWarning("MInputContext: unable to find the IM server activation service.");
        dbus_g_connection_unref(sessionBusConnection);
        sessionBusConnection = 0;
        return;
    }
}

void GlibDBusIMServerProxy::onDisconnectionTrampoline(DBusGProxy */*proxy*/, gpointer userData)
{
    mDebug("MInputContext") << __PRETTY_FUNCTION__;
    static_cast<GlibDBusIMServerProxy *>(userData)->onDisconnection();
}

void GlibDBusIMServerProxy::connect()
{
    mDebug("MInputContext") << __PRETTY_FUNCTION__;
    GError *error = NULL;

    if (!activationProxy) {
        return;
    }

    char *address;
    if (!dbus_g_proxy_call(activationProxy, "address", &error, G_TYPE_INVALID,
                           G_TYPE_STRING, &address, G_TYPE_INVALID)) {
        qWarning("MInputContext: unable to query input method server address: %s", error->message);
        g_error_free(error);
        QTimer::singleShot(ConnectionRetryInterval, this, SLOT(connect()));
        return;
    }

    connection = dbus_g_connection_open(address, &error);
    g_free(address);
    if (!connection) {
        qWarning("MInputContext: unable to create D-Bus connection: %s", error->message);
        g_error_free(error);
        QTimer::singleShot(ConnectionRetryInterval, this, SLOT(connect()));
        return;
    }

    glibObjectProxy = dbus_g_proxy_new_for_peer(connection, DBusPath, DBusInterface);
    if (!glibObjectProxy) {
        qWarning("MInputContext: unable to find the D-Bus service.");
        dbus_g_connection_unref(connection);
        connection = 0;
        QTimer::singleShot(ConnectionRetryInterval, this, SLOT(connect()));
        return;
    }
    g_signal_connect(G_OBJECT(glibObjectProxy), "destroy", G_CALLBACK(onDisconnectionTrampoline),
                     this);

    dbus_g_connection_register_g_object(connection, icAdaptorPath.toAscii().data(), inputContextAdaptor);

    emit dbusConnected();
}

void GlibDBusIMServerProxy::onDisconnection()
{
    mDebug("MInputContext") << __PRETTY_FUNCTION__;
    glibObjectProxy = 0;
    dbus_g_connection_unref(connection);
    connection = 0;
    emit dbusDisconnected();
    if (active) {
        QTimer::singleShot(ConnectionRetryInterval, this, SLOT(connect()));
    }
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

void GlibDBusIMServerProxy::setPreedit(const QString &text)
{
    if (!glibObjectProxy) {
        return;
    }
    dbus_g_proxy_call_no_reply(glibObjectProxy, "setPreedit",
                               G_TYPE_STRING, text.toUtf8().data(),
                               G_TYPE_INVALID);
}

GArray* GlibDBusIMServerProxy::serializeVariant(const QVariant &value)
{
    QByteArray temporaryStorage;
    QDataStream valueStream(&temporaryStorage, QIODevice::WriteOnly);
    valueStream << value;
    GArray * const gdata(g_array_sized_new(FALSE, FALSE, 1, temporaryStorage.size()));
    g_array_append_vals(gdata, temporaryStorage.constData(),
                        temporaryStorage.size());
    return gdata;
}

void GlibDBusIMServerProxy::updateWidgetInformation(const QMap<QString, QVariant> &stateInformation,
                                                    bool focusChanged)
{
    if (!glibObjectProxy) {
        return;
    }
    GArray *serializedState(serializeVariant(stateInformation));
    dbus_g_proxy_call_no_reply(glibObjectProxy, "updateWidgetInformation",
                               DBUS_TYPE_G_UCHAR_ARRAY, serializedState,
                               G_TYPE_BOOLEAN, focusChanged,
                               G_TYPE_INVALID);
    g_array_unref(serializedState);
}

void GlibDBusIMServerProxy::reset()
{
    if (!glibObjectProxy) {
        return;
    }
    dbus_g_proxy_call_no_reply(glibObjectProxy, "reset",
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
                                            quint32 nativeScanCode, quint32 nativeModifiers)
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
                               G_TYPE_INVALID);
}

void GlibDBusIMServerProxy::registerToolbar(int id, const QString &fileName)
{
    if (!glibObjectProxy) {
        return;
    }
    dbus_g_proxy_call_no_reply(glibObjectProxy, "registerToolbar",
                               G_TYPE_INT, id,
                               G_TYPE_STRING, fileName.toUtf8().data(),
                               G_TYPE_INVALID);
}

void GlibDBusIMServerProxy::unregisterToolbar(int id)
{
    if (!glibObjectProxy) {
        return;
    }
    dbus_g_proxy_call_no_reply(glibObjectProxy, "unregisterToolbar",
                               G_TYPE_INT, id,
                               G_TYPE_INVALID);
}

void GlibDBusIMServerProxy::setToolbarItemAttribute(int id, const QString &item,
                                                    const QString &attribute, const QVariant &value)
{
    if (!glibObjectProxy) {
        return;
    }
    GArray* serializedValue(serializeVariant(value));

    dbus_g_proxy_call_no_reply(glibObjectProxy, "setToolbarItemAttribute",
                               G_TYPE_INT, id,
                               G_TYPE_STRING, item.toUtf8().data(),
                               G_TYPE_STRING, attribute.toUtf8().data(),
                               DBUS_TYPE_G_UCHAR_ARRAY, serializedValue,
                               G_TYPE_INVALID);
    g_array_unref(serializedValue);
}
