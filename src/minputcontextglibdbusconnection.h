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

#include <QtCore>
#include <QtGui>

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

    /* Public so they can be called from plain C callbacks */
    void handleDisconnection(unsigned int connectionId);
    void insertNewConnection(unsigned int connectionId, MDBusGlibICConnection *connectionObj);

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
    virtual void sendActivationLostEvent();
    //! \reimp_end

public slots:
    // communication w/ minputcontext

    //! \reimp
    virtual void updateInputMethodArea(const QRegion &region);
    //! \reimp_end

private:
    MDBusGlibICConnection *activeContext();
    MDBusGlibICConnection *connectionObj(unsigned int connectionId);

private:
    QByteArray socketAddress;
    DBusServer *server;
    /* Used to maintain a mapping between the connection identifiers
    and the object we actually use to handle communication for the given ID. */
    QMap<unsigned int,MDBusGlibICConnection *>mConnections;

    Q_DISABLE_COPY(MInputContextGlibDBusConnection)
};
//! \internal_end


#endif
