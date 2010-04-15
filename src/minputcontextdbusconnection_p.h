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

#ifndef MINPUTCONTEXTDBUSCONNECTION_P_H
#define MINPUTCONTEXTDBUSCONNECTION_P_H

#include <QDBusAbstractAdaptor>
#include <QMap>
#include <QDBusInterface>
#include <QString>
#include <QVariant>

class MInputContextDBusConnection;

class MInputContextDBusAdaptor: public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.maemo.duiinputmethodserver1")

public:
    MInputContextDBusAdaptor(MInputContextDBusConnection *host);
    virtual ~MInputContextDBusAdaptor();

public slots:
    void showInputMethod();
    void hideInputMethod();
    void mouseClickedOnPreedit(const QPoint &pos, const QRect &preeditRect);
    void setPreedit(const QString &text);
    void updateWidgetInformation(const QMap<QString, QVariant> &stateInformation);
    void reset();

    void setContextObject(QString callbackObject);
    void activateContext();
    void appOrientationChanged(int angle);
    void setCopyPasteState(bool copyAvailable, bool pasteAvailable);
    void processKeyEvent(int keyType, int keyCode, int modifiers, const QString &text,
                         bool autoRepeat, int count, int nativeScanCode, int nativeModifiers);

private:
    MInputContextDBusConnection *host;
};



class MInputContextDBusConnectionPrivate
{
public:
    MInputContextDBusConnectionPrivate();
    ~MInputContextDBusConnectionPrivate();

    QMap<QString, QDBusInterface *> clients; // map of service/interface object
    QDBusInterface *activeContext; // currently active context
    bool valid;
    QMap<QString, QVariant> widgetState;
    bool globalCorrectionEnabled;
    bool redirectionEnabled;
};

#endif
