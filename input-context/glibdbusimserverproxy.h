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

class QPoint;
class QRect;
class QString;
class QVariant;
class QRegion;

namespace Maliit
{
    namespace DBusGLib
    {
    // std::tr1::shared_ptr allows to specify a deleter. Hiding it behind a typedef,
    // as we do not really need all the shared pointer semantics here.
    typedef std::tr1::shared_ptr<DBusGConnection> ConnectionRef;
    }
}

/* \brief Glib D-Bus implementation of a proxy through which input method server
 * methods can be called
 */
class GlibDBusIMServerProxy : public QObject
{
    Q_OBJECT

public:
    GlibDBusIMServerProxy(GObject* inputContextAdaptor, const QString &icAdaptorPath);
    virtual ~GlibDBusIMServerProxy();

    void activateContext();

    void showInputMethod();

    void hideInputMethod();

    void mouseClickedOnPreedit(const QPoint &pos, const QRect &preeditRect);

    void setPreedit(const QString &text, int cursorPos);

    void updateWidgetInformation(const QMap<QString, QVariant> &stateInformation,
                                 bool focusChanged);

    void reset(bool requireSynchronization);
    bool pendingResets();

    void appOrientationAboutToChange(int angle);

    void appOrientationChanged(int angle);

    void setCopyPasteState(bool copyAvailable, bool pasteAvailable);

    void processKeyEvent(QEvent::Type keyType, Qt::Key keyCode,
                         Qt::KeyboardModifiers modifiers,
                         const QString &text, bool autoRepeat, int count,
                         quint32 nativeScanCode, quint32 nativeModifiers, unsigned long time);

    void registerAttributeExtension(int id, const QString &fileName);

    void unregisterAttributeExtension(int id);

    void setExtendedAttribute(int id, const QString &target, const QString &targetItem,
                              const QString &attribute, const QVariant &value);

signals:
    void dbusDisconnected();
    void dbusConnected();

private slots:
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
    QString icAdaptorPath;
    bool active;
    QSet<DBusGProxyCall *> pendingResetCalls;
};

#endif
