/* * This file is part of Maliit framework *
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 *
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 */

#ifndef DBUSINPUTCONTEXTCONNECTION_H
#define DBUSINPUTCONTEXTCONNECTION_H

#include "minputcontextconnection.h"

#include "serverdbusaddress.h"

#include <QDBusContext>
#include <QDBusVariant>
#include <QHash>

class ComMeegoInputmethodInputcontext1Interface;

class DBusInputContextConnection : public MInputContextConnection,
                                   protected QDBusContext
{
    Q_OBJECT
public:
    explicit DBusInputContextConnection(const QSharedPointer<Maliit::Server::DBus::Address> &address);
    ~DBusInputContextConnection();

    //! \reimp
    virtual void sendPreeditString(const QString &string,
                                   const QList<Maliit::PreeditTextFormat> &preeditFormats,
                                   int replacementStart = 0, int replacementLength = 0,
                                   int cursorPos = -1);
    virtual void sendCommitString(const QString &string, int replaceStart = 0,
                                  int replaceLength = 0, int cursorPos = -1);
    virtual void sendKeyEvent(const QKeyEvent &keyEvent,
                              Maliit::EventRequestType requestType);
    virtual void notifyImInitiatedHiding();

    virtual void setGlobalCorrectionEnabled(bool);
    virtual QRect preeditRectangle(bool &valid);
    virtual void setRedirectKeys(bool enabled);
    virtual void setDetectableAutoRepeat(bool enabled);
    virtual void invokeAction(const QString &action,
                            const QKeySequence &sequence);
    virtual void setSelection(int start, int length);
    virtual QString selection(bool &valid);
    virtual void setLanguage(const QString &language);
    virtual void sendActivationLostEvent();
    virtual void updateInputMethodArea(const QRegion &region);
    virtual void notifyExtendedAttributeChanged(int id,
                                                const QString &target,
                                                const QString &targetItem,
                                                const QString &attribute,
                                                const QVariant &value);
    virtual void notifyExtendedAttributeChanged(const QList<int> &clientIds,
                                                int id,
                                                const QString &target,
                                                const QString &targetItem,
                                                const QString &attribute,
                                                const QVariant &value);
    //! \reimp_end

    void activateContext();
    void showInputMethod();
    void hideInputMethod();
    void mouseClickedOnPreedit(int posX, int posY, int preeditRectX, int preeditRectY, int preeditRectWidth, int preeditRectHeight);
    void setPreedit(const QString &text, int cursorPos);
    void updateWidgetInformation(const QVariantMap &stateInformation, bool focusChanged);
    void reset();
    void appOrientationAboutToChange(int angle);
    void appOrientationChanged(int angle);
    void setCopyPasteState(bool copyAvailable, bool pasteAvailable);
    void processKeyEvent(int keyType, int keyCode, int modifiers, const QString &text, bool autoRepeat, int count, uint nativeScanCode, uint nativeModifiers, uint time);
    void registerAttributeExtension(int id, const QString &fileName);
    void unregisterAttributeExtension(int id);
    void setExtendedAttribute(int id, const QString &target, const QString &targetItem, const QString &attribute, const QDBusVariant &value);

private Q_SLOTS:
    void newConnection(const QDBusConnection &connection);
    void onDisconnection();

private:
    unsigned int connectionNumber();

    const QSharedPointer<Maliit::Server::DBus::Address> mAddress;
    QScopedPointer<QDBusServer> mServer;
    QHash<QString, unsigned int> mConnectionNumbers;
    QHash<unsigned int, ComMeegoInputmethodInputcontext1Interface *> mProxys;
    QHash<unsigned int, QString> mConnections;

    QString lastLanguage;
};

#endif // DBUSINPUTCONTEXTCONNECTION_H
