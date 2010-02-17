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

#include "ut_duiinputcontextadaptor.h"
#include "duiinputcontextadaptor.h"
#include "duiinputcontext_stub.h"

#include <QApplication>

namespace
{
    const QString TestingInSandboxEnvVariable("TESTING_IN_SANDBOX");
}



void Ut_DuiInputContextAdaptor::initTestCase()
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
    static char *argv[] = { (char *)"ut_duiinputcontextadaptor" };
    app = new QApplication(argc, argv);
}

void Ut_DuiInputContextAdaptor::cleanupTestCase()
{
    delete app;
}

void Ut_DuiInputContextAdaptor::init()
{
    inputContext = new DuiInputContext();
    subject = new DuiInputContextAdaptor(inputContext);
    QVERIFY(subject);

    gDuiInputContextStub->stubReset();
}

void Ut_DuiInputContextAdaptor::cleanup()
{
    delete subject;
    delete inputContext;
}

void Ut_DuiInputContextAdaptor::testNoReplyPassthroughs()
{
    QString someString("hello");

    subject->imInitiatedHide();
    QCOMPARE(1, gDuiInputContextStub->stubCallCount("imInitiatedHide"));

    subject->commitString(someString);
    QCOMPARE(1, gDuiInputContextStub->stubCallCount("commitString"));

    subject->updatePreedit(someString, 0);
    QCOMPARE(1, gDuiInputContextStub->stubCallCount("updatePreedit"));

    subject->keyEvent(0, 0, 0, someString, false, 0);
    QCOMPARE(1, gDuiInputContextStub->stubCallCount("keyEvent"));

    subject->updateInputMethodArea(QList<QVariant>());
    QCOMPARE(1, gDuiInputContextStub->stubCallCount("updateInputMethodArea"));

    subject->setGlobalCorrectionEnabled(true);
    QCOMPARE(1, gDuiInputContextStub->stubCallCount("setGlobalCorrectionEnabled"));
}

void Ut_DuiInputContextAdaptor::testReplyMethods()
{
    // valid is out parameter for value validity
    bool validResult = false;

    DuiInputContext::WidgetInfo winfo;

    for (int i = 0; i < 3; i++) {

        if (i == 0) {
            winfo.valid = false;
            // other members ignored
        } else if (i == 1) {
            winfo.valid = true;
            // set all booleans to false, content type to FreeText
            winfo.predictionEnabled = false;
            winfo.correctionEnabled = false;
            winfo.autoCapitalizationEnabled = false;
            winfo.contentType = Dui::FreeTextContentType;
            winfo.inputMethodMode = Dui::InputMethodModeNormal;
            winfo.preeditRectangle = QRect();
        } else {
            winfo.valid = true;
            // set all booleans to true, content type to Number
            winfo.predictionEnabled = true;
            winfo.correctionEnabled = true;
            winfo.autoCapitalizationEnabled = true;
            winfo.contentType = Dui::NumberContentType;
            winfo.inputMethodMode = Dui::InputMethodModeDirect;
            winfo.preeditRectangle = QRect(0, 0, 1, 1);
        }

        gDuiInputContextStub->stubSetReturnValue("getFocusWidgetInfo", winfo);
        gDuiInputContextStub->stubSetReturnValue("surroundingText", winfo.valid);

        Dui::TextContentType type = static_cast<Dui::TextContentType>(subject->contentType(validResult));
        QCOMPARE(winfo.valid, validResult);
        if (validResult)
            QCOMPARE(winfo.contentType, type);

        bool enabled = subject->correctionEnabled(validResult);
        QCOMPARE(winfo.valid, validResult);
        if (validResult)
            QCOMPARE(winfo.correctionEnabled, enabled);

        enabled = subject->predictionEnabled(validResult);
        QCOMPARE(winfo.valid, validResult);
        if (validResult)
            QCOMPARE(winfo.predictionEnabled, enabled);

        enabled = subject->autoCapitalizationEnabled(validResult);
        QCOMPARE(winfo.valid, validResult);
        if (validResult)
            QCOMPARE(winfo.autoCapitalizationEnabled, enabled);

        QString text;
        int cursorPosition;
        validResult = subject->surroundingText(text, cursorPosition);
        QCOMPARE(winfo.valid, validResult);
        Dui::InputMethodMode inputMode = static_cast<Dui::InputMethodMode>(
                                             subject->inputMethodMode(validResult));
        QCOMPARE(winfo.valid, validResult);
        if (validResult)
            QCOMPARE(winfo.inputMethodMode, inputMode);

        QRect preeditRect = subject->preeditRectangle(validResult);
        QCOMPARE(winfo.valid, validResult);
        if (validResult)
            QCOMPARE(winfo.preeditRectangle, preeditRect);
    }
}

QTEST_APPLESS_MAIN(Ut_DuiInputContextAdaptor)
