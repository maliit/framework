/* * This file is part of meego-keyboard *
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


#include "ut_mattributeextensionmanager.h"

#include "core-utils.h"

#include <mattributeextensionmanager.h>
#include <mattributeextensionid.h>
#include <QCoreApplication>
#include <QSignalSpy>
#include <QDebug>
#include <QDir>

namespace {
    const QString testDirectory = "/ut_mattributeextensionmanager";
}

void Ut_MAttributeExtensionManager::initTestCase()
{
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

QTEST_MAIN(Ut_MAttributeExtensionManager);
