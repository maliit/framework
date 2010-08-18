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

#include "qtdbusimserverproxy.h"

#include <QPoint>
#include <QRect>
#include <QString>
#include <QVariant>
#include <QDebug>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusConnectionInterface>

#include <MDebug>


namespace
{
    const QString DBusServiceName("com.meego.inputmethod.uiserver1");
    const QString DBusPath("/com/meego/inputmethod/uiserver1");
    const QString DBusInterface("com.meego.inputmethod.uiserver1");
}


QtDBusIMServerProxy::QtDBusIMServerProxy(const QString &dbusObjectPath)
    : iface(NULL),
      dbusObjectPath(dbusObjectPath)
{
    QDBusConnection connection = QDBusConnection::sessionBus();

    if (connection.isConnected() == false) {
        mDebug("MInputContext") << "Cannot connect to the DBus session bus";
    }

    QDBusConnectionInterface *connectionInterface = connection.interface();

    // start to follow server changes
    connect(connectionInterface, SIGNAL(serviceOwnerChanged(QString, QString, QString)),
            this, SLOT(serviceChangeHandler(QString, QString, QString)));

    // iface MUST NOT be sibling of object of class derived from QDBusAbstractAdaptor
    // due to bug in Qt
    iface = new QDBusInterface(DBusServiceName, DBusPath, DBusInterface, connection, 0);

    // The input method server might not be running when MInputContext is created.
    // The iface validity is rechecked at serviceChangeHandler().
    if (!iface->isValid()) {
        mDebug("MInputContext") << "MInputContext was unable to connect to input method server: "
                                << connection.lastError().message();
    } else {
        setContextObject(dbusObjectPath);
    }
}

QtDBusIMServerProxy::~QtDBusIMServerProxy()
{
}

// Auxiliary connection handling.............................................

void QtDBusIMServerProxy::serviceChangeHandler(const QString &name, const QString &/*oldOwner*/,
                                               const QString &newOwner)
{
    if (name != DBusServiceName) {
        return;
    }

    emit dbusDisconnected();

    if (!newOwner.isEmpty()) {
        if (!iface->isValid()) {
            if (!iface->isValid()) {
                // dbus interface don't seem to become valid if on construction the server
                // wasn't found. workaround this by creating the interface again
                mDebug("MInputContext") << "recreating dbus interface";
                delete iface;
                QDBusConnection connection = QDBusConnection::sessionBus();
                iface = new QDBusInterface(DBusServiceName, DBusPath, DBusInterface, connection, 0);
            }

            mDebug("MInputContext")
                << "InputContext: service owner changed. Registering callback again";
            setContextObject(dbusObjectPath);
        }
        emit dbusConnected();
    }
}


// Remote methods............................................................

void QtDBusIMServerProxy::setContextObject(const QString &dbusObjectPath)
{
    iface->call(QDBus::NoBlock, "setContextObject", dbusObjectPath);
}

void QtDBusIMServerProxy::activateContext()
{
    iface->call(QDBus::NoBlock, "activateContext");
}

void QtDBusIMServerProxy::showInputMethod()
{
    iface->call(QDBus::NoBlock, "showInputMethod");
}

void QtDBusIMServerProxy::hideInputMethod()
{
    iface->call(QDBus::NoBlock, "hideInputMethod");
}

void QtDBusIMServerProxy::mouseClickedOnPreedit(const QPoint &pos, const QRect &preeditRect)
{
    iface->call(QDBus::NoBlock, "mouseClickedOnPreedit", pos, preeditRect);
}

void QtDBusIMServerProxy::setPreedit(const QString &text)
{
    iface->call(QDBus::NoBlock, "setPreedit", text);
}

void QtDBusIMServerProxy::updateWidgetInformation(const QMap<QString, QVariant> &stateInformation,
                                                  bool focusChanged)
{
    iface->call(QDBus::NoBlock, "updateWidgetInformation", stateInformation, focusChanged);
}

void QtDBusIMServerProxy::reset()
{
    iface->call(QDBus::NoBlock, "reset");
}

void QtDBusIMServerProxy::appOrientationChanged(int angle)
{
    iface->call(QDBus::NoBlock, "appOrientationChanged", angle);
}

void QtDBusIMServerProxy::setCopyPasteState(bool copyAvailable, bool pasteAvailable)
{
    iface->call(QDBus::NoBlock, "setCopyPasteState", copyAvailable, pasteAvailable);
}

void QtDBusIMServerProxy::processKeyEvent(QEvent::Type keyType, Qt::Key keyCode,
                                          Qt::KeyboardModifiers modifiers,
                                          const QString &text, bool autoRepeat, int count,
                                          quint32 nativeScanCode, quint32 nativeModifiers)
{
    iface->call(QDBus::NoBlock, "processKeyEvent", static_cast<int>(keyType),
                static_cast<int>(keyCode), static_cast<int>(modifiers), text, autoRepeat, count,
                nativeScanCode, nativeModifiers);
}

void QtDBusIMServerProxy::registerToolbar(int id, const QString &fileName)
{
    iface->call(QDBus::NoBlock, "registerToolbar", id, fileName);
}

void QtDBusIMServerProxy::unregisterToolbar(int id)
{
    iface->call(QDBus::NoBlock, "unregisterToolbar", id);
}

void QtDBusIMServerProxy::setToolbarItemAttribute(int id, const QString &item,
                                                  const QString &attribute, const QVariant &value)
{
    iface->call(QDBus::NoBlock, "setToolbarItemAttribute", item, attribute, value);
}
