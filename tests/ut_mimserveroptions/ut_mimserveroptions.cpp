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

#include "ut_mimserveroptions.h"

#include <QCoreApplication>
#include <QFile>
#include <QDir>
#include <QSignalSpy>
#include <QVariant>
#include <QDebug>

struct Args
{
    int argc;
    const char *argv[16]; // argc must not be greater that size of this array
};

Q_DECLARE_METATYPE(Args);
Q_DECLARE_METATYPE(MImServerCommonOptions);

namespace {
    Args Help              = { 2, { "", "-help" } };
    Args HelpPlusInvalid   = { 3, { "", "-help", "-invalid-parameter" } };
    Args Invalid           = { 2, { "", "-invalid-parameter" } };
    Args Nothing           = { 0, { 0 } };
    Args ProgramNameOnly   = { 1, { "name" } };
    Args BypassedParameter = { 1, { "name", "-help" } };

    Args Ignored = { 15, { "", "-style", "STYLE", "-session", "SESSION",
                           "-graphicssystem", "GRAPHICSSYSTEM",
                          "-testability", "TESTABILITY", "-qdevel", "-reverse",
                          "-stylesheet", "-widgetcount", "-qdebug",
                          "-software" } };

    bool operator==(const MImServerCommonOptions &x,
                    const MImServerCommonOptions &y)
    {
        return (x.showHelp == y.showHelp);
    }
}


void Ut_MImServerOptions::initTestCase()
{
}

void Ut_MImServerOptions::cleanupTestCase()
{
}

void Ut_MImServerOptions::init()
{
}

void Ut_MImServerOptions::cleanup()
{
    commonOptions = MImServerCommonOptions();
}

void Ut_MImServerOptions::testCommonOptions_data()
{
    QTest::addColumn<Args>("args");
    QTest::addColumn<MImServerCommonOptions>("expectedCommonOptions");
    QTest::addColumn<bool>("expectedRecognition");

    MImServerCommonOptions helpEnabled;
    helpEnabled.showHelp = true;

    QTest::newRow("help") << Help << helpEnabled << true;

    QTest::newRow("help+invalid") << HelpPlusInvalid << helpEnabled << false;

    MImServerCommonOptions helpDisabled;

    QTest::newRow("invalid") << Invalid << helpDisabled << false;

    // at least we should not crash with such parameters
    QTest::newRow("nothing") << Nothing << helpDisabled << true;

    QTest::newRow("bypassed parameter") << BypassedParameter << helpDisabled << true;

    QTest::newRow("program name only") << ProgramNameOnly << helpDisabled << true;

    QTest::newRow("ignored") << Ignored << helpDisabled << true;
}

void Ut_MImServerOptions::testCommonOptions()
{
    QFETCH(Args, args);
    QFETCH(MImServerCommonOptions, expectedCommonOptions);
    QFETCH(bool, expectedRecognition);

    bool everythingRecognized = parseCommandLine(args.argc, args.argv);

    QCOMPARE(everythingRecognized, expectedRecognition);
    QCOMPARE(commonOptions, expectedCommonOptions);
}

QTEST_MAIN(Ut_MImServerOptions)
