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

#include "dbusserverconnection.h"

#include "minputmethodcontext1interface_adaptor.h"
#include "minputmethodserver1interface_interface.h"

#include <QDBusConnection>
#include <QDebug>

namespace
{
    const char * const IMServerPath("/com/meego/inputmethod/uiserver1");
    const char * const IMServerConnection("Maliit::IMServerConnection");
    const char * const InputContextAdaptorPath("/com/meego/inputmethod/inputcontext");
    const char * const DBusLocalPath("/org/freedesktop/DBus/Local");
    const char * const DBusLocalInterface("org.freedesktop.DBus.Local");
    const char * const DisconnectedSignal("Disconnected");
    const int ConnectionRetryInterval(6*1000); // in ms
}

DBusServerConnection::DBusServerConnection(const QSharedPointer<Maliit::InputContext::DBus::Address> &address) :
    MImServerConnection(0)
  , mAddress(address)
  , mProxy(0)
  , mActive(true)
  , pendingResetCalls()
{
    new Inputcontext1Adaptor(this);

    connect(mAddress.data(), SIGNAL(addressRecieved(QString)),
            this, SLOT(openDBusConnection(QString)));
    connect(mAddress.data(), SIGNAL(addressFetchError(QString)),
            this, SLOT(connectToDBusFailed(QString)));

    QTimer::singleShot(0, this, SLOT(connectToDBus()));
}

DBusServerConnection::~DBusServerConnection()
{
    mActive = false;
    Q_FOREACH (QDBusPendingCallWatcher *watcher, pendingResetCalls) {
        disconnect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                   this, SLOT(resetCallFinished(QDBusPendingCallWatcher*)));
    }
}

void DBusServerConnection::connectToDBus()
{
    mAddress->get();
}

void DBusServerConnection::openDBusConnection(const QString &addressString)
{
    if (addressString.isEmpty()) {
        QTimer::singleShot(ConnectionRetryInterval, this, SLOT(connectToDBus()));
        return;
    }

    QDBusConnection connection = QDBusConnection::connectToPeer(addressString, QString::fromLatin1(IMServerConnection));
    if (!connection.isConnected()) {
        QTimer::singleShot(ConnectionRetryInterval, this, SLOT(connectToDBus()));
        return;
    }

    mProxy = new ComMeegoInputmethodUiserver1Interface(QString(), QString::fromLatin1(IMServerPath), connection, this);

    connection.connect(QString(), QString::fromLatin1(DBusLocalPath), QString::fromLatin1(DBusLocalInterface),
                       QString::fromLatin1(DisconnectedSignal),
                       this, SLOT(onDisconnection()));

    connection.registerObject(QString::fromLatin1(InputContextAdaptorPath), this);

    Q_EMIT connected();
}

void DBusServerConnection::connectToDBusFailed(const QString &)
{
    QTimer::singleShot(ConnectionRetryInterval, this, SLOT(connectToDBus()));
}

void DBusServerConnection::onDisconnection()
{
    delete mProxy;
    mProxy = 0;
    QDBusConnection::disconnectFromPeer(QString::fromLatin1(IMServerConnection));
    Q_EMIT disconnected();

    if (mActive)
        QTimer::singleShot(ConnectionRetryInterval, this, SLOT(connectToDBus()));
}

void DBusServerConnection::resetCallFinished(QDBusPendingCallWatcher *watcher)
{
    pendingResetCalls.remove(watcher);
    watcher->deleteLater();
}

bool DBusServerConnection::pendingResets()
{
    return !pendingResetCalls.empty();
}

void DBusServerConnection::activateContext()
{
    if (!mProxy)
        return;

    mProxy->activateContext();
}

void DBusServerConnection::showInputMethod()
{
    if (!mProxy)
        return;

    mProxy->showInputMethod();
}

void DBusServerConnection::hideInputMethod()
{
    if (!mProxy)
        return;

    mProxy->hideInputMethod();
}

void DBusServerConnection::mouseClickedOnPreedit(const QPoint &pos, const QRect &preeditRect)
{
    if (!mProxy)
        return;

    mProxy->mouseClickedOnPreedit(pos.x(), pos.y(), preeditRect.x(), preeditRect.y(), preeditRect.width(), preeditRect.height());
}

void DBusServerConnection::setPreedit(const QString &text, int cursorPos)
{
    if (!mProxy)
        return;

    mProxy->setPreedit(text, cursorPos);
}

void DBusServerConnection::updateWidgetInformation(const QMap<QString, QVariant> &stateInformation,
                                                   bool focusChanged)
{
    if (!mProxy)
        return;

    mProxy->updateWidgetInformation(stateInformation, focusChanged);
}

void DBusServerConnection::reset(bool requireSynchronization)
{
    if (!mProxy)
        return;

    QDBusPendingCall resetCall = mProxy->reset();
    if (requireSynchronization) {
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(resetCall, this);
        pendingResetCalls.insert(watcher);
        QObject::connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                         this, SLOT(resetCallFinished(QDBusPendingCallWatcher*)));
    }
}

void DBusServerConnection::appOrientationAboutToChange(int angle)
{
    if (!mProxy)
        return;

    mProxy->appOrientationAboutToChange(angle);
}

void DBusServerConnection::appOrientationChanged(int angle)
{
    if (!mProxy)
        return;

    mProxy->appOrientationChanged(angle);
}

void DBusServerConnection::setCopyPasteState(bool copyAvailable, bool pasteAvailable)
{
    if (!mProxy)
        return;

    mProxy->setCopyPasteState(copyAvailable, pasteAvailable);
}

void DBusServerConnection::processKeyEvent(QEvent::Type keyType, Qt::Key keyCode,
                                           Qt::KeyboardModifiers modifiers,
                                           const QString &text, bool autoRepeat, int count,
                                           quint32 nativeScanCode, quint32 nativeModifiers, unsigned long time)
{
    if (!mProxy)
        return;

    mProxy->processKeyEvent(keyType, keyCode, modifiers, text, autoRepeat, count, nativeScanCode, nativeModifiers, time);
}

void DBusServerConnection::registerAttributeExtension(int id, const QString &fileName)
{
    if (!mProxy)
        return;

    mProxy->registerAttributeExtension(id, fileName);
}

void DBusServerConnection::unregisterAttributeExtension(int id)
{
    if (!mProxy)
        return;

    mProxy->unregisterAttributeExtension(id);
}

void DBusServerConnection::setExtendedAttribute(int id, const QString &target, const QString &targetItem,
                                                const QString &attribute, const QVariant &value)
{
    if (!mProxy)
        return;

    mProxy->setExtendedAttribute(id, target, targetItem, attribute, QDBusVariant(value));
}

void DBusServerConnection::keyEvent(int type, int key, int modifiers, const QString &text, bool autoRepeat, int count, uchar requestType)
{
    keyEvent(type, key, modifiers, text, autoRepeat, count, static_cast<MInputMethod::EventRequestType>(requestType));
}

void DBusServerConnection::notifyExtendedAttributeChanged(int id, const QString &target, const QString &targetItem, const QString &attribute, const QDBusVariant &value)
{
    extendedAttributeChanged(id, target, targetItem, attribute, value.variant());
}

bool DBusServerConnection::preeditRectangle(int &x, int &y, int &width, int &height) const
{
    bool valid;
    QRect result;
    getPreeditRectangle(result, valid);
    x = result.x();
    y = result.y();
    width = result.width();
    height = result.height();
    return valid;
}

bool DBusServerConnection::selection(QString &selection) const
{
    bool valid;
    getSelection(selection, valid);
    return valid;
}

void DBusServerConnection::updateInputMethodArea(int x, int y, int width, int height)
{
    updateInputMethodArea(QRect(x, y, width, height));
}
