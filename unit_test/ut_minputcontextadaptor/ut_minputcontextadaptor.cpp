/* * This file is part of m-im-framework *
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

#include "ut_minputcontextadaptor.h"
#include "minputcontextadaptor.h"
#include "minputcontext_stub.h"

#include <QApplication>

namespace
{
    const QString TestingInSandboxEnvVariable("TESTING_IN_SANDBOX");
}



void Ut_MInputContextAdaptor::initTestCase()
{
    // This is a hack to prevent Qt from loading the plugin from
    // /usr/lib/qt4/plugins/inputmethods/ when we are testing in a
    // sandbox.
    bool testingInSandbox = false;
    const QStringList env(QProcess::systemEnvironment());
    int index = env.indexOf(QRegExp('^' + TestingInSandboxEnvVariable + "=.*", Qt::CaseInsensitive));
    if (index != -1) {
        QString statusCandidate = env.at(index);
        statusCandidate = statusCandidate.remove(
                              QRegExp('^' + TestingInSandboxEnvVariable + '=', Qt::CaseInsensitive));
        bool statusOk = false;
        int status = statusCandidate.toInt(&statusOk);
        if (statusOk && (status == 0 || status == 1)) {
            testingInSandbox = (status == 1);
        } else {
            qDebug() << "Invalid " << TestingInSandboxEnvVariable << " environment variable.\n";
            QFAIL("Attempt to execute test incorrectly.");
        }
    }
    if (testingInSandbox)
        QCoreApplication::setLibraryPaths(QStringList("/tmp"));


    int argc = 1;
    static char *argv[] = { (char *)"ut_minputcontextadaptor" };
    app = new QApplication(argc, argv);
}

void Ut_MInputContextAdaptor::cleanupTestCase()
{
    delete app;
}

void Ut_MInputContextAdaptor::init()
{
    inputContext = new MInputContext();
    subject = new MInputContextAdaptor(inputContext);
    QVERIFY(subject);

    gMInputContextStub->stubReset();
}

void Ut_MInputContextAdaptor::cleanup()
{
    delete subject;
    delete inputContext;
}

void Ut_MInputContextAdaptor::testNoReplyPassthroughs()
{
    QString someString("hello");

    subject->imInitiatedHide();
    QCOMPARE(1, gMInputContextStub->stubCallCount("imInitiatedHide"));

    subject->commitString(someString);
    QCOMPARE(1, gMInputContextStub->stubCallCount("commitString"));

    subject->updatePreedit(someString, 0);
    QCOMPARE(1, gMInputContextStub->stubCallCount("updatePreedit"));

    subject->keyEvent(0, 0, 0, someString, false, 0);
    QCOMPARE(1, gMInputContextStub->stubCallCount("keyEvent"));

    subject->updateInputMethodArea(QList<QVariant>());
    QCOMPARE(1, gMInputContextStub->stubCallCount("updateInputMethodArea"));

    subject->setGlobalCorrectionEnabled(true);
    QCOMPARE(1, gMInputContextStub->stubCallCount("setGlobalCorrectionEnabled"));
}

QTEST_APPLESS_MAIN(Ut_MInputContextAdaptor)
