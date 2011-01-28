/* * This file is part of meego-keyboard *
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



#include "ut_mtoolbarmanager.h"
#include <mtoolbarmanager.h>
#include <mtoolbardata.h>
#include <mtoolbaritem.h>
#include <mtoolbarlayout.h>
#include <mtoolbarid.h>
#include <QCoreApplication>
#include <QDebug>
#include <QDir>

Q_DECLARE_METATYPE(MInputMethod::ActionType);

namespace {
    QString Toolbar1 = "/toolbar1.xml";
    QString Toolbar2 = "/toolbar2.xml";
    QString Toolbar3 = "/toolbar3.xml"; // this file does not exist
}

void Ut_MToolbarManager::initTestCase()
{
    // Avoid waiting if im server is not responding
    // MApplication::setLoadMInputContext(false);

    static char *argv[1] = {(char *) "ut_toolbarmanager"};
    static int argc = 1;
    app = new QCoreApplication(argc, argv);

    Toolbar1 = QCoreApplication::applicationDirPath() + Toolbar1;
    QVERIFY2(QFile(Toolbar1).exists(), "toolbar1.xml does not exist");
    Toolbar2 = QCoreApplication::applicationDirPath() + Toolbar2;
    QVERIFY2(QFile(Toolbar2).exists(), "toolbar2.xml does not exist");
    Toolbar3 = QCoreApplication::applicationDirPath() + Toolbar3;
    QVERIFY2(!QFile(Toolbar3).exists(), "toolbar3.xml should not exist");
}

void Ut_MToolbarManager::cleanupTestCase()
{
    delete app;
    app = 0;
}

void Ut_MToolbarManager::init()
{
    subject = new MToolbarManager;
}

void Ut_MToolbarManager::cleanup()
{
    delete subject;
    subject = 0;
}

void Ut_MToolbarManager::testLoadToolbar()
{
    QList<MToolbarId> toolbarIds;
    for (qlonglong i = 1; i <= 3; i ++) {
        MToolbarId id(i, "Ut_MToolbarManager");
        toolbarIds <<  id;
    }
    QStringList toolbars;
    toolbars << Toolbar1
             << Toolbar2
             << Toolbar3;

    int toolbarCount = 0;
    // register all toolbars
    for (int i = 0; i < toolbarIds.count(); i++) {
        subject->registerToolbar(toolbarIds.at(i), toolbars.at(i));
        if (QFile(toolbars.at(i)).exists())
            toolbarCount ++;
        QTest::qWait(50);
        QCOMPARE(subject->toolbarList().count(), toolbarCount);
    }

    for (int i = 0; i < toolbarIds.count(); i++) {
        QSharedPointer<MToolbarData> toolbar = subject->toolbarData(toolbarIds.at(i));
        if (QFile(toolbars.at(i)).exists())
            QVERIFY(!toolbar.isNull());
    }

    for (int i = toolbarIds.count() - 1; i >= 0; --i) {
        subject->unregisterToolbar(toolbarIds.at(i));
        QCOMPARE(subject->toolbars.count(), i);
        QVERIFY(!subject->toolbars.contains(toolbarIds.at(i)));
    }
}

void Ut_MToolbarManager::testSetItemAttribute()
{
    MToolbarId id1(1, "Ut_MToolbarManager");
    MToolbarId id2(2, "Ut_MToolbarManager");
    QList<MToolbarId> toolbarIds;
    toolbarIds <<  id1 << id2;
    QStringList toolbars;
    toolbars << Toolbar1
             << Toolbar1;

    int toolbarCount = 0;
    // register all toolbars
    for (int i = 0; i < toolbarIds.count(); i++) {
        subject->registerToolbar(toolbarIds.at(i), toolbars.at(i));
        toolbarCount ++;
        QTest::qWait(50);
        QCOMPARE(subject->toolbarList().count(), toolbarCount);
    }

    subject->setToolbarItemAttribute(id1, "test1", "text", QVariant(QString("some text")));

    QSharedPointer<MToolbarData> toolbar = subject->toolbarData(id1);
    QVERIFY(!toolbar.isNull());

    QSharedPointer<MToolbarItem> item = toolbar->item("test1");
    QVERIFY(!item.isNull());
    QCOMPARE(item->text(), QString("some text"));

    toolbar = subject->toolbarData(id2);
    QVERIFY(!toolbar.isNull());

    item = toolbar->item("test1");
    QVERIFY(!item.isNull());
    QVERIFY(item->text() != "some text");

    // pass incorrect parameters. Test should not crash here
    MToolbarId invalidId;
    subject->setToolbarItemAttribute(invalidId, "test1", "text", QVariant(QString("some text")));
    subject->setToolbarItemAttribute(id1, "invalid-item-name", "text", QVariant(QString("some text")));
}

QTEST_APPLESS_MAIN(Ut_MToolbarManager);

