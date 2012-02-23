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

#include <QObject>
#include <QEvent>
#include <Qt>
#include <QMap>

#include "mimserverconnection.h"
#include "inputcontextdbusaddress.h"

#include <tr1/memory>

class GlibDBusIMServerProxyPrivate;

class QDBusError;
class QDBusVariant;

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
    GlibDBusIMServerProxy(std::tr1::shared_ptr<Maliit::InputContext::DBus::Address> address, QObject *parent = 0);
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
    void openDBusConnection(const QString &addressString);
    void connectToDBusFailed(const QString &errorMessage);

private:
    void onDisconnection();
    static void onDisconnectionTrampoline(void *proxy, void* userData);

    const QScopedPointer<GlibDBusIMServerProxyPrivate> d_ptr;
    Q_DECLARE_PRIVATE(GlibDBusIMServerProxy)
};

#endif
