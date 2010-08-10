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

#ifndef QTDBUSIMSERVERPROXY_H
#define QTDBUSIMSERVERPROXY_H

#include <QObject>
#include <QEvent>
#include <Qt>
#include <QMap>

class QPoint;
class QRect;
class QString;
class QVariant;
class QRegion;
class QDBusInterface;

/* \brief Qt D-Bus implementation of a proxy through which input method server
 * methods can be called
 */
class QtDBusIMServerProxy : public QObject
{
    Q_OBJECT

public:
    QtDBusIMServerProxy(const QString &dbusObjectPath);
    virtual ~QtDBusIMServerProxy();

    void setContextObject(const QString &dbusObjectPath);
    void activateContext();

    void showInputMethod();

    void hideInputMethod();

    void mouseClickedOnPreedit(const QPoint &pos, const QRect &preeditRect);

    void setPreedit(const QString &text);

    void updateWidgetInformation(const QMap<QString, QVariant> &stateInformation,
                                 bool focusChanged);

    void reset();

    void appOrientationChanged(int angle);

    void setCopyPasteState(bool copyAvailable, bool pasteAvailable);

    void processKeyEvent(QEvent::Type keyType, Qt::Key keyCode,
                         Qt::KeyboardModifiers modifiers,
                         const QString &text, bool autoRepeat, int count,
                         quint32 nativeScanCode, quint32 nativeModifiers);

    void registerToolbar(int id, const QString &fileName);

    void unregisterToolbar(int id);

    void setToolbarItemAttribute(int id, const QString &item,
                                 const QString &attribute, const QVariant &value);

signals:
    void dbusDisconnected();
    void dbusConnected();

private slots:
    void serviceChangeHandler(const QString &name, const QString &oldOwner,
                              const QString &newOwner);

private:
    QDBusInterface *iface;
    QString dbusObjectPath;
};

#endif
