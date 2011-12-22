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

#ifndef GLIBDBUSIMSERVERPROXY_H
#define GLIBDBUSIMSERVERPROXY_H

#include <dbus/dbus-glib.h>
#include <QObject>
#include <QEvent>
#include <Qt>
#include <QMap>
#include <QSet>
#include <tr1/memory>

#include "mimserverconnection.h"
#include "minputcontext.h"
#include "inputcontextdbusaddress.h"

namespace Maliit
{
    namespace DBusGLib
    {
    // std::tr1::shared_ptr allows to specify a deleter. Hiding it behind a typedef,
    // as we do not really need all the shared pointer semantics here.
    typedef std::tr1::shared_ptr<DBusGConnection> ConnectionRef;
    }
}

/* \brief Glib D-Bus implementation of a connection with the input method server.
 *
 * The input context is exposed over DBus so that the input method server can communicate with it,
 * and methods can be called to communicate with the input method server.
 */

// TODO: rename to MImGlibDBusServerConnection, or similar
class GlibDBusIMServerProxy: public MImServerConnection
{
    Q_OBJECT

public:
    GlibDBusIMServerProxy(QObject *parent = 0);
    virtual ~GlibDBusIMServerProxy();

    //! reimpl
    virtual bool pendingResets();

    /* Outbound communication */
    virtual void activateContext();

    virtual void showInputMethod();

    virtual void hideInputMethod();

    virtual void mouseClickedOnPreedit(const QPoint &pos, const QRect &preeditRect);

    virtual void setPreedit(const QString &text, int cursorPos);

    virtual void updateWidgetInformation(const QMap<QString, QVariant> &stateInformation,
                                 bool focusChanged);

    virtual void reset(bool requireSynchronization);

    virtual void appOrientationAboutToChange(int angle);
    virtual void appOrientationAboutToChange(int angle, Qt::HANDLE pixmapHandle);

    virtual void appOrientationChanged(int angle);

    virtual void setCopyPasteState(bool copyAvailable, bool pasteAvailable);

    virtual void processKeyEvent(QEvent::Type keyType, Qt::Key keyCode,
                         Qt::KeyboardModifiers modifiers,
                         const QString &text, bool autoRepeat, int count,
                         quint32 nativeScanCode, quint32 nativeModifiers, unsigned long time);

    virtual void registerAttributeExtension(int id, const QString &fileName);

    virtual void unregisterAttributeExtension(int id);

    virtual void setExtendedAttribute(int id, const QString &target, const QString &targetItem,
                              const QString &attribute, const QVariant &value);
    //! reimpl end

private Q_SLOTS:
    void connectToDBus();

private:
    void setContextObject(const QString &dbusObjectPath);

    static void onDisconnectionTrampoline(DBusGProxy *proxy, gpointer userData);
    void onDisconnection();

    static void resetNotifyTrampoline(DBusGProxy *proxy, DBusGProxyCall *callId, gpointer userData);
    void resetNotify(DBusGProxy *proxy, DBusGProxyCall *callId);

    DBusGProxy *glibObjectProxy;
    Maliit::DBusGLib::ConnectionRef connection;
    GObject *inputContextAdaptor;
    bool active;
    QSet<DBusGProxyCall *> pendingResetCalls;
    const std::tr1::shared_ptr<Maliit::InputContext::DBus::Address> mAddress;
};

#endif
