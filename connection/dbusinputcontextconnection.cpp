/* * This file is part of maliit-framework *
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

#include "dbusinputcontextconnection.h"

#include "minputmethodserver1interface_adaptor.h"
#include "minputmethodcontext1interface_interface.h"
#include "dbuscustomarguments.h"

#include <QDBusConnection>
#include <QDBusServer>

#include <QKeyEvent>

namespace
{
    const char * const DBusPath = "/com/meego/inputmethod/uiserver1";

    const char * const DBusClientPath = "/com/meego/inputmethod/inputcontext";
    const char * const DBusClientInterface = "com.meego.inputmethod.inputcontext1";

    const char * const DBusLocalPath("/org/freedesktop/DBus/Local");
    const char * const DBusLocalInterface("org.freedesktop.DBus.Local");
    const char * const DisconnectedSignal("Disconnected");
}

DBusInputContextConnection::DBusInputContextConnection(const QSharedPointer<Maliit::Server::DBus::Address> &address)
    : MInputContextConnection(0)
    , mAddress(address)
    , mServer(mAddress->connect())
    , mConnectionNumbers()
    , mProxys()
    , lastLanguage()
{
    connect(mServer.data(), SIGNAL(newConnection(QDBusConnection)), this, SLOT(newConnection(QDBusConnection)));

    qDBusRegisterMetaType<MImPluginSettingsEntry>();
    qDBusRegisterMetaType<MImPluginSettingsInfo>();
    qDBusRegisterMetaType<QList<MImPluginSettingsInfo> >();

    new Uiserver1Adaptor(this);
}

DBusInputContextConnection::~DBusInputContextConnection()
{
}

void
DBusInputContextConnection::newConnection(const QDBusConnection &connection)
{
    ComMeegoInputmethodInputcontext1Interface *proxy = new ComMeegoInputmethodInputcontext1Interface(QString(), QString::fromLatin1(DBusClientPath), connection, this);

    static unsigned int connectionCounter = 1; // Start at 1 so 0 can be used as a sentinel value
    unsigned int connectionNumber = connectionCounter++;

    mConnectionNumbers.insert(connection.name(), connectionNumber);
    mProxys.insert(connectionNumber, proxy);

    QDBusConnection c(connection);

    c.connect(QString(), QString::fromLatin1(DBusLocalPath), QString::fromLatin1(DBusLocalInterface),
              QString::fromLatin1(DisconnectedSignal),
              this, SLOT(onDisconnection()));

    c.registerObject(QString::fromLatin1(DBusPath), this);

    proxy->setLanguage(lastLanguage);
}

void
DBusInputContextConnection::onDisconnection()
{
    const QString &name = connection().name();
    unsigned int connectionNumber = mConnectionNumbers.take(name);
    ComMeegoInputmethodInputcontext1Interface *proxy = mProxys.take(connectionNumber);
    delete proxy;
}

void
DBusInputContextConnection::sendPreeditString(const QString &string,
                                              const QList<Maliit::PreeditTextFormat> &preeditFormats,
                                              int replacementStart, int replacementLength,
                                              int cursorPos)
{
    if (activeConnection) {
        MInputContextConnection::sendPreeditString(string, preeditFormats, replacementStart, replacementLength, cursorPos);

        ComMeegoInputmethodInputcontext1Interface *proxy = mProxys.value(activeConnection);
        if (proxy) {
            proxy->updatePreedit(string, preeditFormats, replacementStart, replacementLength, cursorPos);
        }
    }
}

void
DBusInputContextConnection::sendCommitString(const QString &string, int replaceStart,
                                             int replaceLength, int cursorPos)
{
    if (activeConnection) {
        MInputContextConnection::sendCommitString(string, replaceStart, replaceLength, cursorPos);

        ComMeegoInputmethodInputcontext1Interface *proxy = mProxys.value(activeConnection);
        if (proxy) {
            proxy->commitString(string, replaceStart, replaceLength, cursorPos);
        }
    }
}

void
DBusInputContextConnection::sendKeyEvent(const QKeyEvent &keyEvent,
                                         Maliit::EventRequestType requestType)
{
    if (activeConnection) {
        MInputContextConnection::sendKeyEvent(keyEvent, requestType);

        ComMeegoInputmethodInputcontext1Interface *proxy = mProxys.value(activeConnection);
        if (proxy) {
            proxy->keyEvent(keyEvent.type(), keyEvent.key(), keyEvent.modifiers(),
                            keyEvent.text(), keyEvent.isAutoRepeat(), keyEvent.count(), requestType);
        }
    }
}

void
DBusInputContextConnection::notifyImInitiatedHiding()
{
    ComMeegoInputmethodInputcontext1Interface *proxy = mProxys.value(activeConnection);
    if (proxy) {
        proxy->imInitiatedHide();
    }
}

void
DBusInputContextConnection::setGlobalCorrectionEnabled(bool enabled)
{
    ComMeegoInputmethodInputcontext1Interface *proxy = mProxys.value(activeConnection);
    if ((enabled != globalCorrectionEnabled()) && proxy) {
        proxy->setGlobalCorrectionEnabled(enabled);
        MInputContextConnection::setGlobalCorrectionEnabled(enabled);
    }
}

QRect
DBusInputContextConnection::preeditRectangle(bool &valid)
{
    ComMeegoInputmethodInputcontext1Interface *proxy = mProxys.value(activeConnection);
    if (proxy) {
        int x, y, width, height;
        if (proxy->preeditRectangle(x, y, width, height)) {
            valid = true;
            return QRect(x, y, width, height);
        }
    }
    valid = false;
    return QRect();
}

void
DBusInputContextConnection::setRedirectKeys(bool enabled)
{
    ComMeegoInputmethodInputcontext1Interface *proxy = mProxys.value(activeConnection);
    if ((enabled != redirectKeysEnabled()) && proxy) {
        proxy->setRedirectKeys(enabled);
        MInputContextConnection::setRedirectKeys(enabled);
    }
}

void
DBusInputContextConnection::setDetectableAutoRepeat(bool enabled)
{
    ComMeegoInputmethodInputcontext1Interface *proxy = mProxys.value(activeConnection);
    if ((enabled != detectableAutoRepeat()) && proxy) {
        proxy->setDetectableAutoRepeat(enabled);
        MInputContextConnection::setDetectableAutoRepeat(enabled);
    }
}

void
DBusInputContextConnection::copy()
{
    ComMeegoInputmethodInputcontext1Interface *proxy = mProxys.value(activeConnection);
    if (proxy) {
        proxy->copy();
    }
}

void
DBusInputContextConnection::paste()
{
    ComMeegoInputmethodInputcontext1Interface *proxy = mProxys.value(activeConnection);
    if (proxy) {
        proxy->paste();
    }
}

void
DBusInputContextConnection::setSelection(int start, int length)
{
    ComMeegoInputmethodInputcontext1Interface *proxy = mProxys.value(activeConnection);
    if (proxy) {
        proxy->setSelection(start, length);
    }
}

QString
DBusInputContextConnection::selection(bool &valid)
{
    ComMeegoInputmethodInputcontext1Interface *proxy = mProxys.value(activeConnection);
    if (proxy) {
        QString selectionText;
        if (proxy->selection(selectionText)) {
            valid = true;
            return selectionText;
        }
    }
    valid = false;
    return QString();
}

void
DBusInputContextConnection::setLanguage(const QString &language)
{
    lastLanguage = language;
    ComMeegoInputmethodInputcontext1Interface *proxy = mProxys.value(activeConnection);
    if (proxy) {
        proxy->setLanguage(language);
    }
}

void
DBusInputContextConnection::sendActivationLostEvent()
{
    ComMeegoInputmethodInputcontext1Interface *proxy = mProxys.value(activeConnection);
    if (proxy) {
        proxy->activationLostEvent();
    }
}

void
DBusInputContextConnection::updateInputMethodArea(const QRegion &region)
{
    ComMeegoInputmethodInputcontext1Interface *proxy = mProxys.value(activeConnection);
    if (proxy) {
        QRect rect = region.boundingRect();
        proxy->updateInputMethodArea(rect.x(), rect.y(), rect.width(), rect.height());
    }
}

void
DBusInputContextConnection::notifyExtendedAttributeChanged(int id,
                                                           const QString &target,
                                                           const QString &targetItem,
                                                           const QString &attribute,
                                                           const QVariant &value)
{
    ComMeegoInputmethodInputcontext1Interface *proxy = mProxys.value(activeConnection);
    if (proxy) {
        proxy->notifyExtendedAttributeChanged(id, target, targetItem, attribute, QDBusVariant(value));
    }
}

void
DBusInputContextConnection::notifyExtendedAttributeChanged(const QList<int> &clientIds,
                                                           int id,
                                                           const QString &target,
                                                           const QString &targetItem,
                                                           const QString &attribute,
                                                           const QVariant &value)
{
    Q_FOREACH (int clientId, clientIds) {
        ComMeegoInputmethodInputcontext1Interface *proxy = mProxys.value(clientId);
        if (proxy) {
            proxy->notifyExtendedAttributeChanged(id, target, targetItem, attribute, QDBusVariant(value));
        }
    }
}

void
DBusInputContextConnection::pluginSettingsLoaded(int clientId, const QList<MImPluginSettingsInfo> &info)
{
    ComMeegoInputmethodInputcontext1Interface *proxy = mProxys.value(clientId);
    if (proxy) {
        proxy->pluginSettingsLoaded(info);
    }
}


unsigned int
DBusInputContextConnection::connectionNumber()
{
    return mConnectionNumbers.value(connection().name());
}

void DBusInputContextConnection::activateContext()
{
    MInputContextConnection::activateContext(connectionNumber());
}

void DBusInputContextConnection::showInputMethod()
{
    MInputContextConnection::showInputMethod(connectionNumber());
}

void DBusInputContextConnection::hideInputMethod()
{
    MInputContextConnection::hideInputMethod(connectionNumber());
}

void DBusInputContextConnection::mouseClickedOnPreedit(int posX, int posY, int preeditRectX, int preeditRectY, int preeditRectWidth, int preeditRectHeight)
{
    MInputContextConnection::mouseClickedOnPreedit(connectionNumber(), QPoint(posX, posY), QRect(preeditRectX, preeditRectY, preeditRectWidth, preeditRectHeight));
}

void DBusInputContextConnection::setPreedit(const QString &text, int cursorPos)
{
    MInputContextConnection::setPreedit(connectionNumber(), text, cursorPos);
}

void DBusInputContextConnection::updateWidgetInformation(const QVariantMap &stateInformation, bool focusChanged)
{
    MInputContextConnection::updateWidgetInformation(connectionNumber(), stateInformation, focusChanged);
}

void DBusInputContextConnection::reset()
{
    MInputContextConnection::reset(connectionNumber());
}

void DBusInputContextConnection::appOrientationAboutToChange(int angle)
{
    MInputContextConnection::receivedAppOrientationAboutToChange(connectionNumber(), angle);
}

void DBusInputContextConnection::appOrientationChanged(int angle)
{
    MInputContextConnection::receivedAppOrientationChanged(connectionNumber(), angle);
}

void DBusInputContextConnection::setCopyPasteState(bool copyAvailable, bool pasteAvailable)
{
    MInputContextConnection::setCopyPasteState(connectionNumber(), copyAvailable, pasteAvailable);
}

void DBusInputContextConnection::processKeyEvent(int keyType, int keyCode, int modifiers, const QString &text, bool autoRepeat, int count, uint nativeScanCode, uint nativeModifiers, uint time)
{
    MInputContextConnection::processKeyEvent(connectionNumber(), static_cast<QEvent::Type>(keyType), static_cast<Qt::Key>(keyCode), static_cast<Qt::KeyboardModifier>(modifiers), text, autoRepeat, count, nativeScanCode, nativeModifiers, time);
}

void DBusInputContextConnection::registerAttributeExtension(int id, const QString &fileName)
{
    MInputContextConnection::registerAttributeExtension(connectionNumber(), id, fileName);
}

void DBusInputContextConnection::unregisterAttributeExtension(int id)
{
    MInputContextConnection::unregisterAttributeExtension(connectionNumber(), id);
}

void DBusInputContextConnection::setExtendedAttribute(int id, const QString &target, const QString &targetItem, const QString &attribute, const QDBusVariant &value)
{
    MInputContextConnection::setExtendedAttribute(connectionNumber(), id, target, targetItem, attribute, value.variant());
}

void DBusInputContextConnection::loadPluginSettings(const QString &descriptionLanguage)
{
    MInputContextConnection::loadPluginSettings(connectionNumber(), descriptionLanguage);
}
