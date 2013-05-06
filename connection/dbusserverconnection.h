/* * This file is part of Maliit framework *
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 *
 * Contact: maliit-discuss@lists.maliit.org
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 */

#ifndef DBUSSERVERCONNECTION_H
#define DBUSSERVERCONNECTION_H

#include "mimserverconnection.h"

#include "inputcontextdbusaddress.h"

#include <QDBusVariant>
#include <QDBusPendingCallWatcher>

class ComMeegoInputmethodUiserver1Interface;

class DBusServerConnection : public MImServerConnection
{
    Q_OBJECT

public:
    explicit DBusServerConnection(const QSharedPointer<Maliit::InputContext::DBus::Address> &address);
    ~DBusServerConnection();

    //! reimpl
    virtual bool pendingResets();
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
    virtual void loadPluginSettings(const QString &descriptionLanguage);
    //! reimpl end

    //! forwarding methods for InputContextAdaptor
    using MImServerConnection::keyEvent;
    void keyEvent(int type, int key, int modifiers, const QString &text, bool autoRepeat,
                  int count, uchar requestType);

    void notifyExtendedAttributeChanged(int id,
                                        const QString &target,
                                        const QString &targetItem,
                                        const QString &attribute,
                                        const QDBusVariant &value);
    void pluginSettingsLoaded(const QList<MImPluginSettingsInfo> &info);

    bool preeditRectangle(int &x, int &y, int &width, int &height) const;
    bool selection(QString &selection) const;

    using MImServerConnection::updateInputMethodArea;
    void updateInputMethodArea(int x, int y, int width, int height);

private Q_SLOTS:
    void connectToDBus();
    void openDBusConnection(const QString &addressString);
    void connectToDBusFailed(const QString &errorMessage);
    void onDisconnection();
    void resetCallFinished(QDBusPendingCallWatcher*);

private:
    QSharedPointer<Maliit::InputContext::DBus::Address> mAddress;
    ComMeegoInputmethodUiserver1Interface *mProxy;
    bool mActive;
    QSet<QDBusPendingCallWatcher*> pendingResetCalls;
};

#endif // DBUSSERVERCONNECTION_H
