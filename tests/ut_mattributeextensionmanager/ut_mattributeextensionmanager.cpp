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


#include "ut_mattributeextensionmanager.h"

#include "core-utils.h"

#include <mattributeextensionmanager.h>
#include <mtoolbardata.h>
#include <mtoolbaritem.h>
#include <mtoolbarlayout.h>
#include <mattributeextensionid.h>
#include <QCoreApplication>
#include <QSignalSpy>
#include <QDebug>
#include <QDir>



Q_DECLARE_METATYPE(MInputMethod::ActionType);

namespace {
    const QString testDirectory = "/ut_mattributeextensionmanager";
    QString Toolbar1 = "/toolbar1.xml";
    QString Toolbar2 = "/toolbar2.xml";
    QString Toolbar3 = "/toolbar3.xml"; // this file does not exist
}

void Ut_MAttributeExtensionManager::initTestCase()
{
    Toolbar1 = MaliitTestUtils::getTestDataPath() + testDirectory + Toolbar1;
    QVERIFY2(QFile(Toolbar1).exists(), "toolbar1.xml does not exist");
    Toolbar2 = MaliitTestUtils::getTestDataPath() + testDirectory + Toolbar2;
    QVERIFY2(QFile(Toolbar2).exists(), "toolbar2.xml does not exist");
    Toolbar3 = MaliitTestUtils::getTestDataPath() + testDirectory + Toolbar3;
    QVERIFY2(!QFile(Toolbar3).exists(), "toolbar3.xml should not exist");
}

void Ut_MAttributeExtensionManager::cleanupTestCase()
{
}

void Ut_MAttributeExtensionManager::init()
{
    subject = new MAttributeExtensionManager;
}

void Ut_MAttributeExtensionManager::cleanup()
{
    delete subject;
    subject = 0;
}

void Ut_MAttributeExtensionManager::testLoadToolbar()
{
    QList<MAttributeExtensionId> toolbarIds;
    for (qlonglong i = 1; i <= 3; i ++) {
        MAttributeExtensionId id(i, "Ut_MAttributeExtensionManager");
        toolbarIds <<  id;
    }
    QStringList toolbars;
    toolbars << Toolbar1
             << Toolbar2
             << Toolbar3;

    int toolbarCount = 0;
    // register all toolbars
    for (int i = 0; i < toolbarIds.count(); i++) {
        subject->registerAttributeExtension(toolbarIds.at(i), toolbars.at(i));
        if (QFile(toolbars.at(i)).exists()) {
            QVERIFY(subject->toolbarData(toolbarIds.at(i)));
            toolbarCount ++;
        }
    }

    for (int i = 0; i < toolbarIds.count(); i++) {
        QSharedPointer<MToolbarData> toolbar = subject->toolbarData(toolbarIds.at(i));
        if (QFile(toolbars.at(i)).exists())
            QVERIFY(!toolbar.isNull());
    }

    for (int i = toolbarIds.count() - 1; i >= 0; --i) {
        subject->unregisterAttributeExtension(toolbarIds.at(i));
        QCOMPARE(subject->attributeExtensionIdList().count(), i);
        QVERIFY(!subject->attributeExtensionIdList().contains(toolbarIds.at(i)));
    }
}

void Ut_MAttributeExtensionManager::testSetItemAttribute()
{
    MAttributeExtensionId id1(1, "Ut_MAttributeExtensionManager");
    MAttributeExtensionId id2(2, "Ut_MAttributeExtensionManager");
    QList<MAttributeExtensionId> toolbarIds;
    toolbarIds <<  id1 << id2;
    QStringList toolbars;
    toolbars << Toolbar1
             << Toolbar1;

    int toolbarCount = 0;
    // register all toolbars
    for (int i = 0; i < toolbarIds.count(); i++) {
        subject->registerAttributeExtension(toolbarIds.at(i), toolbars.at(i));
        toolbarCount ++;
        QTest::qWait(50);
        QCOMPARE(subject->attributeExtensionIdList().count(), toolbarCount);
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
    MAttributeExtensionId invalidId;
    subject->setToolbarItemAttribute(invalidId, "test1", "text", QVariant(QString("some text")));
    subject->setToolbarItemAttribute(id1, "invalid-item-name", "text", QVariant(QString("some text")));
}

void Ut_MAttributeExtensionManager::testSetExtendedAttribute()
{
    MAttributeExtensionId id1(1, "Ut_MAttributeExtensionManager");
    MAttributeExtensionId id2(2, "Ut_MAttributeExtensionManager");

    QList<MAttributeExtensionId> idList;
    idList <<  id1 << id2;

    int attributteExtentionCount = 0;
    // register all extended attributes
    for (int i = 0; i < idList.count(); i++) {
        subject->registerAttributeExtension(idList.at(i), "");
        attributteExtentionCount ++;
        QTest::qWait(50);
        QCOMPARE(subject->attributeExtensionIdList().count(), attributteExtentionCount);
    }

    QSignalSpy spy(subject, SIGNAL(keyOverrideCreated()));
    QVERIFY(spy.isValid());
    for (int i = 0; i < idList.count(); i++) {
        QCOMPARE(subject->keyOverrides(idList.at(i)).count(), 0);

        // set extended attribute not registered before.
        subject->setExtendedAttribute(idList.at(i),
                                      "/keys",
                                      "testKey",
                                      "label",
                                      QVariant("testLabel"));

        // new key overrides will be created
        QCOMPARE(spy.count(), 1);
        spy.clear();
        QCOMPARE(subject->keyOverrides(idList.at(i)).count(), 1);
        QVERIFY(subject->keyOverrides(idList.at(i)).value("testKey"));
        QCOMPARE(subject->keyOverrides(idList.at(i)).value("testKey")->label(), QString("testLabel"));
    }

    spy.clear();
    // item attribute is modified in one, the same item attribute must not change in the other one.
    subject->setExtendedAttribute(idList.at(0),
                                  "/keys",
                                  "testKey",
                                  "icon",
                                  QVariant("testIcon"));
    QCOMPARE(subject->keyOverrides(idList.at(0)).count(), 1);
    QVERIFY(subject->keyOverrides(idList.at(0)).value("testKey"));
    QCOMPARE(subject->keyOverrides(idList.at(0)).value("testKey")->icon(), QString("testIcon"));
    QVERIFY(subject->keyOverrides(idList.at(1)).value("testKey")->icon().isEmpty());
}

void Ut_MAttributeExtensionManager::testSendExtendedAttributeBack()
{
    const MAttributeExtensionId id(1, "Ut_MAttributeExtensionManager");
    const QString name = QString::fromLatin1("toggle");
    const QString attribute = QString::fromLatin1("pressed");

    // register toolbar
    subject->registerAttributeExtension(id, Toolbar2);

    QSharedPointer<MToolbarData> toolbar = subject->toolbarData(id);
    QVERIFY(!toolbar.isNull());

    QSharedPointer<MToolbarItem> item = toolbar->item(name);
    QVERIFY(!item.isNull());
    QVERIFY(item->toggle());
    QVERIFY(!item->pressed());
    QCOMPARE(item->text(), QString("toggle-button"));

    QSharedPointer<MToolbarItem> idle = toolbar->item("idle");
    QVERIFY(!idle.isNull());
    QVERIFY(idle->toggle());
    QVERIFY(!idle->pressed());
    QCOMPARE(idle->text(), QString("idle-button"));

    QSignalSpy spy(subject, SIGNAL(notifyExtensionAttributeChanged(int,QString,QString,QString,QVariant)));

    // signal should not be emitted if uiserver changes attribute which has not
    // been changed by application calling setToolbarItemAttribute
    item->setPressed(false);
    QVERIFY(spy.isEmpty());

    item->setPressed(true);
    QVERIFY(spy.isEmpty());

    // now application wants to know what happens with this toolbar item
    subject->setToolbarItemAttribute(id, name, attribute, QVariant(false));
    QVERIFY(spy.isEmpty());
    QVERIFY(!item->pressed());

    item->setPressed(true);
    QCOMPARE(spy.count(), 1);
    spy.clear();

    // other attributes and items should not cause signal emission
    item->setText("other");
    QVERIFY(spy.isEmpty());
    idle->setText("label");
    QVERIFY(spy.isEmpty());
    idle->setPressed(true);
    QVERIFY(spy.isEmpty());

    // and now we are not interested about it anymore
    qDebug() << "now we are not interested about it again";
    subject->unregisterAttributeExtension(id);
    QVERIFY(subject->toolbarItemFilters.isEmpty());

    item->setPressed(false);
    QVERIFY(spy.isEmpty());
}

QTEST_MAIN(Ut_MAttributeExtensionManager);
