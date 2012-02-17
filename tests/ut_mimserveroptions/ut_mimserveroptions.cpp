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
Q_DECLARE_METATYPE(MImServerXOptions);

namespace {
    Args Help              = { 2, { "", "-help" } };
    Args HelpPlusInvalid   = { 3, { "", "-help", "-invalid-parameter" } };
    Args Invalid           = { 2, { "", "-invalid-parameter" } };
    Args Nothing           = { 0, { 0 } };
    Args ProgramNameOnly   = { 1, { "name" } };
    Args BypassedParameter = { 1, { "name", "-help" } };

    Args XOptions1 = { 2, { "", "-manual-redirection" } };
    Args XOptions2 = { 3, { "", "-manual-redirection", "-bypass-wm-hint" } };

    Args XOptions3 = { 4, { "", "-manual-redirection", "-bypass-wm-hint",
                            "-use-self-composition"} };

    Args XOptions4 = { 5, { "", "-manual-redirection", "-bypass-wm-hint",
                            "-use-self-composition", "-unconditional-show"} };

    Args Ignored = { 16, { "", "-style", "STYLE", "-session", "SESSION",
                           "-graphicssystem", "GRAPHICSSYSTEM",
                          "-testability", "TESTABILITY", "-qdevel", "-reverse",
                          "-stylesheet", "-widgetcount", "-qdebug",
                          "-software", "-remote-theme" } };

    bool operator==(const MImServerCommonOptions &x,
                    const MImServerCommonOptions &y)
    {
        return (x.showHelp == y.showHelp);
    }

    bool operator==(const MImServerXOptions &x,
                    const MImServerXOptions &y)
    {
        return x.bypassWMHint == y.bypassWMHint
               && x.manualRedirection == y.manualRedirection
               && x.selfComposited == y.selfComposited
               && x.unconditionalShow == y.unconditionalShow;
    }
}


void Ut_MImServerOptions::initTestCase()
{
    char *argv[1] = { (char *) "ut_mimserveroptions" };
    int argc = 1;

    app = new QCoreApplication(argc, argv);
}

void Ut_MImServerOptions::cleanupTestCase()
{
    delete app;
}

void Ut_MImServerOptions::init()
{
}

void Ut_MImServerOptions::cleanup()
{
    commonOptions = MImServerCommonOptions();
    xOptions = MImServerXOptions();
}

void Ut_MImServerOptions::testCommonOptions_data()
{
    QTest::addColumn<Args>("args");
    QTest::addColumn<MImServerCommonOptions>("expectedCommonOptions");
    QTest::addColumn<MImServerXOptions>("expectedXOptions");
    QTest::addColumn<bool>("expectedRecognition");

    MImServerCommonOptions helpEnabled;
    helpEnabled.showHelp = true;

    MImServerXOptions allXDisabled;

    QTest::newRow("help") << Help << helpEnabled << allXDisabled << true;

    QTest::newRow("help+invalid") << HelpPlusInvalid << helpEnabled << allXDisabled << false;

    MImServerCommonOptions helpDisabled;

    QTest::newRow("invalid") << Invalid << helpDisabled << allXDisabled << false;

    // at least we should not crash with such parameters
    QTest::newRow("nothing") << Nothing << helpDisabled << allXDisabled << true;

    QTest::newRow("bypassed parameter") << BypassedParameter << helpDisabled << allXDisabled << true;

    QTest::newRow("program name only") << ProgramNameOnly << helpDisabled << allXDisabled << true;

    QTest::newRow("ignored") << Ignored << helpDisabled << allXDisabled << true;

    MImServerXOptions xOptions(allXDisabled);

    xOptions.manualRedirection = true;
    QTest::newRow("x options 1") << XOptions1 << helpDisabled << xOptions << true;

    xOptions.bypassWMHint = true;
    QTest::newRow("x options 2") << XOptions2 << helpDisabled << xOptions << true;

    xOptions.selfComposited = true;
    QTest::newRow("x options 3") << XOptions3 << helpDisabled << xOptions << true;

    xOptions.unconditionalShow = true;
    QTest::newRow("x options 4") << XOptions4 << helpDisabled << xOptions << true;
}

void Ut_MImServerOptions::testCommonOptions()
{
    QFETCH(Args, args);
    QFETCH(MImServerCommonOptions, expectedCommonOptions);
    QFETCH(MImServerXOptions, expectedXOptions);
    QFETCH(bool, expectedRecognition);

    bool everythingRecognized = parseCommandLine(args.argc, args.argv);

    QCOMPARE(everythingRecognized, expectedRecognition);
    QCOMPARE(commonOptions, expectedCommonOptions);
    QCOMPARE(xOptions, expectedXOptions);
}

QTEST_APPLESS_MAIN(Ut_MImServerOptions)

