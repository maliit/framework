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

#include "bbt_connection.h"
#include "core-utils.h"

// Direct connection backend
#include <mimdirectserverconnection.h>
#include <miminputcontextdirectconnection.h>

// GlibDBus connection backend
#include <connectionfactory.h>

#include <QtCore>
#include <limits>

namespace {
    const char * const CONNECTION_TYPE_DIRECT = "direct: ";
    const char * const CONNECTION_TYPE_GLIB_DBUS = "glib dbus: ";

    /* Return a list of integers that might be worthwhile to test. */
    QList<int> getInterestingIntegers()
    {
        QList<int> interestingIntegers;
        const int minInt = std::numeric_limits<int>::min();
        const int maxInt = std::numeric_limits<int>::max();
        interestingIntegers << minInt << -1 << 0 << 1 << maxInt;
        return interestingIntegers;
    }
}


void Bbt_Connection::initTestCase()
{
    // TODO: also test CONNECTION_TYPE_GLIB_DBUS;
    mConnectionTypes << CONNECTION_TYPE_DIRECT;
}

void Bbt_Connection::cleanupTestCase()
{

}

void Bbt_Connection::init()
{
    // Direct connection backend
    MImInputContextDirectConnection *directIcConn = new MImInputContextDirectConnection();
    MImDirectServerConnection *directServerConn = new MImDirectServerConnection();
    directServerConn->connectTo(directIcConn);
    mIcConnections.insert(CONNECTION_TYPE_DIRECT, directIcConn);
    mServerConnections.insert(CONNECTION_TYPE_DIRECT, directServerConn);
    directServerConn->activateContext();

    // Glib DBus backend
    // FIXME: currently cannot be tested due to blocking call for getting the dbus address
    /*
    mIcConnections.insert(CONNECTION_TYPE_GLIB_DBUS, new MInputContextGlibDBusConnection());
    MImServerConnection *glibDbusServerConn = new GlibDBusIMServerProxy();
    mServerConnections.insert(CONNECTION_TYPE_GLIB_DBUS, glibDbusServerConn);
    glibDbusServerConn->activateContext();
    */
}

void Bbt_Connection::cleanup()
{
    qDeleteAll(mServerConnections);
    qDeleteAll(mIcConnections);
}

/* Populate QTest data.
Adds a QString "connectionType" column with the different connection types.
Used in test functions which do not want to have different input data. */
void Bbt_Connection::setupConnectionsDataVoid()
{
    QTest::addColumn<QString>("connectionType");

    Q_FOREACH (const QString &connType, mConnectionTypes) {
        QTest::newRow(connType.toUtf8()) << connType;
    }
}

/* Populate QTest data.
Adds a QString "connectionType" column with the different connection types and
a bool "enabled" column with true and false.
Used in test functions which want to test connections with boolean input data. */
void Bbt_Connection::setupConnectionsDataBool()
{
    QTest::addColumn<QString>("connectionType");
    QTest::addColumn<bool>("enabled");

    Q_FOREACH (const QString &connType, mConnectionTypes) {
        QTest::newRow((connType + QString("test true")).toUtf8())
                << connType << true;
        QTest::newRow((connType + QString("test false")).toUtf8())
                << connType << false;
    }
}

/* Return the MImServerConnection that should be used in a test.
 * A QString "connectionType" column must be provided in the QTest data. */
MImServerConnection *
Bbt_Connection::serverConnection()
{
    QFETCH(QString, connectionType);
    return mServerConnections.value(connectionType);
}

/* Return the MInputContextConnection that should be used in a test.
 * A QString "connectionType" column must be provided in the QTest data. */
MInputContextConnection *
Bbt_Connection::icConnection()
{
    QFETCH(QString, connectionType);
    return mIcConnections.value(connectionType);
}

/* Test that copy() on server side reaches input context side */
void Bbt_Connection::testCopy_data()
{
    setupConnectionsDataVoid();
}
void Bbt_Connection::testCopy()
{
    QSignalSpy signalSpy(serverConnection(), SIGNAL(copy()));
    icConnection()->copy();

    MaliitTestUtils::waitAndProcessEvents(10);

    QCOMPARE(signalSpy.count(), 1);
}

/* Test that paste() on server side reaches input context side */
void Bbt_Connection::testPaste_data()
{
    setupConnectionsDataVoid();
}
void Bbt_Connection::testPaste()
{
    QSignalSpy signalSpy(serverConnection(), SIGNAL(paste()));
    icConnection()->paste();

    MaliitTestUtils::waitAndProcessEvents(10);

    QCOMPARE(signalSpy.count(), 1);
}

/* Test that notififyImInitiatedHiding() on server side reaches input context side */
void Bbt_Connection::testImInitiatedHide_data()
{
    setupConnectionsDataVoid();
}
void Bbt_Connection::testImInitiatedHide()
{
    QSignalSpy signalSpy(serverConnection(), SIGNAL(imInitiatedHide()));
    icConnection()->notifyImInitiatedHiding();

    MaliitTestUtils::waitAndProcessEvents(10);

    QCOMPARE(signalSpy.count(), 1);
}

/* Test that setRedirectKeys() on server side reaches input context side */
void Bbt_Connection::testSetRedirectKeys_data()
{
    setupConnectionsDataBool();
}
void Bbt_Connection::testSetRedirectKeys()
{
    const bool enabled = true;
    QSignalSpy signalSpy(serverConnection(), SIGNAL(setRedirectKeys(bool)));
    icConnection()->setRedirectKeys(enabled);

    MaliitTestUtils::waitAndProcessEvents(10);

    QCOMPARE(signalSpy.count(), 1);
    QCOMPARE(signalSpy.first().at(0).toBool(), enabled);
}

/* Test that setDetectableAutoRepeat() on server side reaches input context side */
void Bbt_Connection::testSetDetectableAutoRepeat_data()
{
    setupConnectionsDataBool();
}
void Bbt_Connection::testSetDetectableAutoRepeat()
{
    const bool enabled = true;
    QSignalSpy signalSpy(serverConnection(), SIGNAL(setDetectableAutoRepeat(bool)));
    icConnection()->setDetectableAutoRepeat(enabled);

    MaliitTestUtils::waitAndProcessEvents(10);

    QCOMPARE(signalSpy.count(), 1);
    QCOMPARE(signalSpy.first().at(0).toBool(), enabled);
}

/* Test that setDetectableAutoRepeat() on server side reaches input context side */
void Bbt_Connection::testSetGlobalCorrectionEnabled_data()
{
    setupConnectionsDataBool();
}
void Bbt_Connection::testSetGlobalCorrectionEnabled()
{
    QFETCH(bool, enabled);
    QSignalSpy signalSpy(serverConnection(), SIGNAL(setGlobalCorrectionEnabled(bool)));
    icConnection()->setGlobalCorrectionEnabled(enabled);

    MaliitTestUtils::waitAndProcessEvents(10);

    QCOMPARE(signalSpy.count(), 1);
    QCOMPARE(signalSpy.first().at(0).toBool(), enabled);
}

/* Test that setSelection(int start, int length); */
void Bbt_Connection::testSetSelection_data()
{
    QTest::addColumn<QString>("connectionType");
    QTest::addColumn<int>("start");
    QTest::addColumn<int>("length");

    /* Test some different data for each parameter for each of the connections. */
    Q_FOREACH (const QString &connType, mConnectionTypes) {

        Q_FOREACH (int integer, getInterestingIntegers()) {
            QTest::newRow((connType + QString("start=") + QString::number(integer)).toUtf8())
                    << connType << integer << 0;
        }

        Q_FOREACH (int integer, getInterestingIntegers()) {
            QTest::newRow((connType + QString("length=") + QString::number(integer)).toUtf8())
                    << connType << 0 << integer;
        }
    }
}
void Bbt_Connection::testSetSelection()
{
    QFETCH(int, start);
    QFETCH(int, length);
    QSignalSpy signalSpy(serverConnection(), SIGNAL(setSelection(int,int)));
    icConnection()->setSelection(start, length);

    MaliitTestUtils::waitAndProcessEvents(10);

    QCOMPARE(signalSpy.count(), 1);
    QCOMPARE(signalSpy.first().at(0).toInt(), start);
    QCOMPARE(signalSpy.first().at(1).toInt(), length);
}


/* Tests that sendCommitString on server side reaches input context side. */
void Bbt_Connection::testCommitString_data()
{
    QTest::addColumn<QString>("connectionType");
    QTest::addColumn<QString>("commitString");
    QTest::addColumn<int>("replaceStart");
    QTest::addColumn<int>("replaceLength");
    QTest::addColumn<int>("cursorPos");

    /* Test some different data for each parameter for each of the connections. */
    Q_FOREACH (const QString &connType, mConnectionTypes) {

        Q_FOREACH (int integer, getInterestingIntegers()) {
            QTest::newRow((connType + QString("replaceStart=") + QString::number(integer)).toUtf8())
                    << connType << QString("") << integer << 0 << 0;
        }

        Q_FOREACH (int integer, getInterestingIntegers()) {
            QTest::newRow((connType + QString("replaceStart=") + QString::number(integer)).toUtf8())
                    << connType << QString("") << integer << 0 << 0;
        }

        Q_FOREACH (int integer, getInterestingIntegers()) {
            QTest::newRow((connType + QString("replaceEnd=") + QString::number(integer)).toUtf8())
                    << connType << QString("") << 0 << integer << 0;
        }

        Q_FOREACH (int integer, getInterestingIntegers()) {
            QTest::newRow((connType + QString("cursorPos=") + QString::number(integer)).toUtf8())
                    << connType << QString("") << 0 << 0 << integer;
        }
    }
}
void Bbt_Connection::testCommitString()
{
    QFETCH(QString, commitString);
    QFETCH(int, replaceStart);
    QFETCH(int, replaceLength);
    QFETCH(int, cursorPos);

    QSignalSpy signalSpy(serverConnection(), SIGNAL(commitString(QString,int,int,int)));

    icConnection()->sendCommitString(commitString, replaceStart, replaceLength, cursorPos);

    MaliitTestUtils::waitAndProcessEvents(10);

    QCOMPARE(signalSpy.count(), 1);
    QCOMPARE(signalSpy.first().at(0).toString(), commitString);
    QCOMPARE(signalSpy.first().at(1).toInt(), replaceStart);
    QCOMPARE(signalSpy.first().at(2).toInt(), replaceLength);
    QCOMPARE(signalSpy.first().at(3).toInt(), cursorPos);
}

QTEST_MAIN(Bbt_Connection)
