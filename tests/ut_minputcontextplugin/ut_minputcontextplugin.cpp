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

#include "utils.h"
#include "ut_minputcontextplugin.h"
#include "minputcontextplugin.h"

#include <QInputContext>
#include <QApplication>

void Ut_MInputContextPlugin::initTestCase()
{
    // This is a hack to prevent Qt from loading the plugin from
    // /usr/lib/qt4/plugins/inputmethods/ when we are testing in a
    // sandbox.
    if (MaliitTestUtils::isTestingInSandbox())
        QCoreApplication::setLibraryPaths(QStringList("/tmp"));

    static int argc = 1;
    static char *argv[1] = { (char *) "ut_minputcontextplugin" };
    app = new QApplication(argc, argv);
}

void Ut_MInputContextPlugin::cleanupTestCase()
{
    delete app;
}

void Ut_MInputContextPlugin::init()
{
    subject = new MInputContextPlugin(0);
    QVERIFY(subject);
}

void Ut_MInputContextPlugin::cleanup()
{
    delete subject;
}

void Ut_MInputContextPlugin::testCreate()
{
    QStringList keys = subject->keys();
    QInputContext *ctx = NULL;

    Q_FOREACH(QString key, keys) {
        ctx = subject->create(key);
        QVERIFY(ctx);
        QCOMPARE(ctx->identifierName(), key);
        delete ctx;
        ctx = NULL;
    }

    // Null key should fail
    ctx = subject->create(QString());
    QVERIFY(!ctx);
}

void Ut_MInputContextPlugin::testOther()
{
    // This is a dont-crash test for simple API functions.

    QString key = subject->keys().first();

    QString description(subject->description(key));
    QString displayName(subject->displayName(key));
    QStringList languages(subject->languages(key));

    qDebug() << "description: " << description;
    qDebug() << "display name: " << displayName;
    Q_FOREACH(QString lang, languages) {
        qDebug() << "language: " << lang;
    }
}

QTEST_APPLESS_MAIN(Ut_MInputContextPlugin)
