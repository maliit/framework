/* * This file is part of dui-im-framework *
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

#ifndef DUIINPUTCONTEXTDBUSCONNECTION_P_H
#define DUIINPUTCONTEXTDBUSCONNECTION_P_H

#include <QDBusAbstractAdaptor>
#include <QMap>
#include <QDBusInterface>
#include <QString>
#include <QVariant>

class DuiInputContextDBusConnection;

class DuiInputContextDBusAdaptor: public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.maemo.duiinputmethodserver1")

public:
    DuiInputContextDBusAdaptor(DuiInputContextDBusConnection *host);
    virtual ~DuiInputContextDBusAdaptor();

public slots:
    void showOnFocus(); // rename -> showInputMethod or inputEntryGainedFocus()
    void hideOnLostFocus();
    void mouseClickedOnPreedit(const QPoint &pos, const QRect &preeditRect);
    void setPreedit(const QString &text);
    void updateWidgetInformation(const QMap<QString, QVariant> &stateInformation);
    void reset();

    void setContextObject(QString callbackObject);
    void activateContext();
    void appOrientationChanged(int angle);
    void setCopyPasteButton(bool copyAvailable, bool pasteAvailable);
    void processKeyEvent(int keyType, int keyCode, int modifiers, const QString &text,
                         bool autoRepeat, int count, int nativeScanCode);

private:
    DuiInputContextDBusConnection *host;
};



class DuiInputContextDBusConnectionPrivate
{
public:
    DuiInputContextDBusConnectionPrivate();
    ~DuiInputContextDBusConnectionPrivate();

    QMap<QString, QDBusInterface *> clients; // map of service/interface object
    QDBusInterface *activeContext; // currently active context
    bool valid;
    QMap<QString, QVariant> widgetState;
    bool globalCorrectionEnabled;
};

#endif
