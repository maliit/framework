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

#ifndef BBT_CONNECTION_H
#define BBT_CONNECTION_H

#include <QtTest/QtTest>
#include <QtCore>

#include <minputcontextconnection.h>
#include <mimserverconnection.h>

/* Bt_Connection
  This class tests the connection between the input method server,
  and the input method context.

  There are multiple implementations of the imServer<->inputContext
  communication, and they are all tested here.
  */

class Bbt_Connection : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void init();
    void cleanup();

    void testInvokeAction_data();
    void testInvokeAction();

    void testImInitiatedHide_data();
    void testImInitiatedHide();

    void testSetRedirectKeys_data();
    void testSetRedirectKeys();

    void testSetGlobalCorrectionEnabled_data();
    void testSetGlobalCorrectionEnabled();

    void testSetDetectableAutoRepeat_data();
    void testSetDetectableAutoRepeat();

    void testSetSelection_data();
    void testSetSelection();

    void testCommitString();
    void testCommitString_data();

private:
    void setupConnectionsDataVoid();
    void setupConnectionsDataBool();

    MImServerConnection *serverConnection();
    MInputContextConnection *icConnection();

private:
    QMap<QString, MImServerConnection *> mServerConnections;
    QMap<QString, MInputContextConnection *> mIcConnections;
    QList<QString> mConnectionTypes;
};

#endif
