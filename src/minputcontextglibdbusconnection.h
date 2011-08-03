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

#ifndef MINPUTCONTEXTGLIBDBUSCONNECTION_H
#define MINPUTCONTEXTGLIBDBUSCONNECTION_H

#include "minputcontextconnection.h"
#include "mattributeextensionid.h"

#include <QWidget>
#include <QByteArray>
#include <QMap>
#include <QSet>
#include <QEvent>
#include <QString>
#include <QVariant>
#include <QWidget>

class QPoint;
class QRegion;
struct MDBusGlibICConnection;
struct MIMSDBusActivater;
struct DBusServer;

//! \internal
/*! \brief Peer-to-peer DBus input context connection based on glib dbus bindings
 */
class MInputContextGlibDBusConnection : public MInputContextConnection
{
    Q_OBJECT

public:
    MInputContextGlibDBusConnection();
    virtual ~MInputContextGlibDBusConnection();

    void handleNewDBusConnectionReady(MDBusGlibICConnection *connectionObj);
    void handleDBusDisconnection(MDBusGlibICConnection *connectionObj);

    //! \reimp
    virtual void sendPreeditString(const QString &string,
                                   const QList<MInputMethod::PreeditTextFormat> &preeditFormats,
                                   int replacementStart = 0, int replacementLength = 0,
                                   int cursorPos = -1);
    virtual void sendCommitString(const QString &string, int replaceStart = 0,
                                  int replaceLength = 0, int cursorPos = -1);
    virtual void sendKeyEvent(const QKeyEvent &keyEvent,
                              MInputMethod::EventRequestType requestType);
    virtual void notifyImInitiatedHiding();

    virtual void setGlobalCorrectionEnabled(bool);
    virtual QRect preeditRectangle(bool &valid);
    virtual void setRedirectKeys(bool enabled);
    virtual void setDetectableAutoRepeat(bool enabled);
    virtual void copy();
    virtual void paste();
    virtual void setSelection(int start, int length);
    virtual QString selection(bool &valid);
    virtual void setLanguage(const QString &language);
    //! \reimp_end

public Q_SLOTS:
    //! \reimp
    virtual void updateInputMethodArea(const QRegion &region);
    //! \reimp_end

public:
    //! sets the input to go to calling connection
    void activateContext(MDBusGlibICConnection *connection);

private:
    //! Helper method for setLanguage(QString) to use it for other than active connection.
    void setLanguage(MDBusGlibICConnection *targetIcConnection,
                     const QString &language);

private:
    MDBusGlibICConnection *activeContext;
    QByteArray socketAddress;
    DBusServer *server;

    //! Cached values to be sent from server to new input contexts.
    QString lastLanguage;

    Q_DISABLE_COPY(MInputContextGlibDBusConnection)
};
//! \internal_end


#endif
