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

#ifndef MINPUTCONTEXTGLIBDBUSCONNECTION_H
#define MINPUTCONTEXTGLIBDBUSCONNECTION_H

#include "minputcontextconnection.h"

#include <QtCore>
#include <QtGui>

#include "serverdbusaddress.h"
#include <tr1/memory>

struct MDBusGlibICConnection;
struct MIMSDBusActivater;
struct DBusServer;

/*! \internal
 * \ingroup maliitserver
 * \brief Peer-to-peer DBus input context connection based on glib dbus bindings
 */
class MInputContextGlibDBusConnection : public MInputContextConnection
{
    Q_OBJECT

public:
    explicit MInputContextGlibDBusConnection(std::tr1::shared_ptr<Maliit::Server::DBus::Address> address, bool allowAnonymous);
    virtual ~MInputContextGlibDBusConnection();

    /* Public so they can be called from plain C callbacks */
    void handleNewConnection(MDBusGlibICConnection *connectionObj);
    void handleDisconnection(unsigned int connectionId);

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
    virtual void copy();
    virtual void paste();
    virtual void setSelection(int start, int length);
    virtual QString selection(bool &valid);
    virtual void setLanguage(const QString &language);
    virtual void sendActivationLostEvent();
    //! \reimp_end

public Q_SLOTS:
    //! \reimp
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
    virtual void pluginSettingsLoaded(int clientId, const QList<MImPluginSettingsInfo> &info);
    //! \reimp_end

private:
    MDBusGlibICConnection *activeContext();
    MDBusGlibICConnection *connectionObj(unsigned int connectionId);
    void handleNewDBusConnectionReady(MDBusGlibICConnection *connectionObj);
    void insertNewConnection(unsigned int connectionId, MDBusGlibICConnection *connectionObj);

    //! Helper method for setLanguage(QString) to use it for other than active connection.
    void setLanguage(MDBusGlibICConnection *targetIcConnection,
                     const QString &language);

private:
    const std::tr1::shared_ptr<Maliit::Server::DBus::Address> mAddress;
    bool mAllowAnonymous;
    DBusServer *server;

    /* Used to maintain a mapping between the connection identifiers
    and the object we actually use to handle communication for the given ID. */
    QMap<unsigned int,MDBusGlibICConnection *>mConnections;

    //! Cached values to be sent from server to new input contexts.
    QString lastLanguage;

    Q_DISABLE_COPY(MInputContextGlibDBusConnection)
};
//! \internal_end


#endif
