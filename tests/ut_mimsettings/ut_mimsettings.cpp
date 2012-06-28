/* * This file is part of Maliit framework *
 *
 * Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
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

#include "ut_mimsettings.h"

#include "mimsettings.h"
#include "mimsettingsqsettings.h"

namespace
{
    const QString ORGANIZATION = "maliit.org";
    const QString APPLICATION = "server";
}


void Ut_MImSettings::initTestCase()
{
    MImSettings::setImplementationFactory(new MImSettingsQSettingsBackendFactory);
}

void Ut_MImSettings::cleanupTestCase()
{
}

// the test code is generic, by writing a backend-specific
// init()/cleanup() pair it could be reused for multiple backends
void Ut_MImSettings::init()
{
    cleanup();

    QSettings settings(ORGANIZATION, APPLICATION);
    settings.beginGroup("ut_mimsettings");

    settings.beginGroup("group");
    settings.setValue("integer", 42);
    settings.setValue("string", "forty-two");
    settings.endGroup();

    settings.beginGroup("group2");
    settings.setValue("integer", 43);
    settings.setValue("string", "forty-three");
    settings.endGroup();
}

void Ut_MImSettings::cleanup()
{
    QSettings settings(ORGANIZATION, APPLICATION);
    settings.beginGroup("ut_mimsettings");

    Q_FOREACH (QString key, settings.childKeys()) {
        settings.remove(key);
    }

    Q_FOREACH (QString key, settings.childGroups()) {
        settings.remove(key);
    }
}


// Test methods..............................................................

void Ut_MImSettings::testValue()
{
    MImSettings integer("/ut_mimsettings/group/integer");
    MImSettings string("/ut_mimsettings/group/string");
    MImSettings empty("/ut_mimsettings/group/empty");

    // basic test for value
    QCOMPARE(integer.key(), QString("/ut_mimsettings/group/integer"));
    QCOMPARE(integer.value().toInt(), 42);

    QCOMPARE(string.key(), QString("/ut_mimsettings/group/string"));
    QCOMPARE(string.value().toString(), QString("forty-two"));

    // check default value handling
    QVERIFY(!empty.value().isValid());
    QCOMPARE(empty.value(42).toInt(), 42);
    QCOMPARE(empty.value("forty-two").toString(), QString("forty-two"));
    QVERIFY(!empty.value().isValid());

    QCOMPARE(integer.value(12).toInt(), 42);
    QCOMPARE(string.value(12).toString(), QString("forty-two"));
}

void Ut_MImSettings::testModifyValue()
{
    MImSettings integer("/ut_mimsettings/group/integer");
    MImSettings integer2("/ut_mimsettings/group/integer");
    MImSettings string("/ut_mimsettings/group/string");
    MImSettings string2("/ut_mimsettings/group/string");

    // basic set tests
    QCOMPARE(integer.value().toInt(), 42);
    integer.set(43);
    QCOMPARE(integer.value().toInt(), 43);
    QCOMPARE(integer2.value().toInt(), 43);

    QCOMPARE(string.value().toString(), QString("forty-two"));
    string.set("forty-three");
    QCOMPARE(string.value().toString(), QString("forty-three"));
    QCOMPARE(string2.value().toString(), QString("forty-three"));

    // basic unset tests
    integer.unset();
    QVERIFY(!integer.value().isValid());
    QVERIFY(!integer2.value().isValid());

    string.unset();
    QVERIFY(!string.value().isValid());
    QVERIFY(!string2.value().isValid());
}

void Ut_MImSettings::testModifyValueNotification()
{
    MImSettings integer("/ut_mimsettings/group/integer");
    MImSettings integer2("/ut_mimsettings/group/integer");
    MImSettings string("/ut_mimsettings/group/string");
    MImSettings string2("/ut_mimsettings/group/string");

    QSignalSpy spy_integer(&integer, SIGNAL(valueChanged()));
    QSignalSpy spy_integer2(&integer2, SIGNAL(valueChanged()));
    QSignalSpy spy_string(&string, SIGNAL(valueChanged()));
    QSignalSpy spy_string2(&string2, SIGNAL(valueChanged()));

    integer.set(43);

    QCOMPARE(spy_integer.count(), 1);
    QCOMPARE(spy_integer2.count(), 1);
    QCOMPARE(spy_string.count(), 0);
    QCOMPARE(spy_string2.count(), 0);

    integer.set(43);

    QCOMPARE(spy_integer.count(), 1);
    QCOMPARE(spy_integer2.count(), 1);
    QCOMPARE(spy_string.count(), 0);
    QCOMPARE(spy_string2.count(), 0);

    integer.set(42);

    QCOMPARE(spy_integer.count(), 2);
    QCOMPARE(spy_integer2.count(), 2);
    QCOMPARE(spy_string.count(), 0);
    QCOMPARE(spy_string2.count(), 0);

    integer2.unset();

    QCOMPARE(spy_integer.count(), 3);
    QCOMPARE(spy_integer2.count(), 3);
    QCOMPARE(spy_string.count(), 0);
    QCOMPARE(spy_string2.count(), 0);

    string.unset();

    QCOMPARE(spy_integer.count(), 3);
    QCOMPARE(spy_integer2.count(), 3);
    QCOMPARE(spy_string.count(), 1);
    QCOMPARE(spy_string2.count(), 1);

    string.unset();

    QCOMPARE(spy_integer.count(), 3);
    QCOMPARE(spy_integer2.count(), 3);
    QCOMPARE(spy_string.count(), 1);
    QCOMPARE(spy_string2.count(), 1);
}

void Ut_MImSettings::testUnsetValue()
{
    MImSettings integer("/ut_mimsettings/group/integer");
    MImSettings group("/ut_mimsettings/group");
    QSignalSpy spy_integer(&integer, SIGNAL(valueChanged()));

    // unset(), then set(Qvariant())
    integer.unset();

    QVERIFY(!integer.value().isValid());
    QCOMPARE(group.listEntries(),
             QList<QString>() << "/ut_mimsettings/group/string");
    QCOMPARE(spy_integer.count(), 1);

    integer.set(QVariant());

    QCOMPARE(spy_integer.count(), 1);

    // set(Qvariant()), then unset()
    integer.set(42);
    spy_integer.clear();

    integer.set(QVariant());

    QVERIFY(!integer.value().isValid());
    QCOMPARE(group.listEntries(),
             QList<QString>() << "/ut_mimsettings/group/string");
    QCOMPARE(spy_integer.count(), 1);

    integer.unset();

    QCOMPARE(spy_integer.count(), 1);
}

void Ut_MImSettings::testListDirs()
{
    QCOMPARE(MImSettings("/ut_mimsettings").listDirs(),
             QList<QString>()
                 << "/ut_mimsettings/group"
                 << "/ut_mimsettings/group2");
    QCOMPARE(MImSettings("/ut_mimsettings/group").listDirs(),
             QList<QString>());
    QCOMPARE(MImSettings("/ut_mimsettings/dummy").listDirs(),
             QList<QString>());
}

void Ut_MImSettings::testListEntries()
{
    QCOMPARE(MImSettings("/ut_mimsettings").listEntries(),
             QList<QString>());
    QCOMPARE(MImSettings("/ut_mimsettings/group").listEntries(),
             QList<QString>()
                 << "/ut_mimsettings/group/integer"
                 << "/ut_mimsettings/group/string");
    QCOMPARE(MImSettings("/ut_mimsettings/dummy").listEntries(),
             QList<QString>());
}

QTEST_MAIN(Ut_MImSettings)
