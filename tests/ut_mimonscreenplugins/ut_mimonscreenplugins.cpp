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

#include "ut_mimonscreenplugins.h"

#include "mimonscreenplugins.h"
#include "mimsettings.h"
#include "mimsettingsqsettings.h"

#include <QtCore>

namespace
{
    const QString ORGANIZATION = "maliit.org";
    const QString APPLICATION = "server-tests";
}

void Ut_MImOnScreenPlugins::initTestCase()
{
    MImSettings::setImplementationFactory(new MImSettingsQSettingsBackendFactory(ORGANIZATION, APPLICATION));
}

void Ut_MImOnScreenPlugins::cleanupTestCase()
{}

void Ut_MImOnScreenPlugins::init()
{}

void Ut_MImOnScreenPlugins::cleanup()
{
    QSettings settings(ORGANIZATION, APPLICATION);
    settings.clear();
}

void Ut_MImOnScreenPlugins::testActiveAndEnabledSubviews_data()
{
    QTest::addColumn<QString>("active_key");
    QTest::addColumn<QString>("enabled_key");
    QTest::addColumn<QString>("initially_active");
    QTest::addColumn<QStringList>("initially_enabled");
    QTest::addColumn<QString>("expected_active_plugin");
    QTest::addColumn<QString>("expected_active_id");
    QTest::addColumn<int>("expected_enabled_count");
    QTest::addColumn<int>("expected_active_index");

    QTest::newRow("empty user configuration")
        << "maliit/onscreen/active" << "maliit/onscreen/enabled"
        << QString() << QStringList()
        << "libmaliit-keyboard-plugin.so" << "en_gb"
        << 1 << 0;
}

void Ut_MImOnScreenPlugins::testActiveAndEnabledSubviews()
{
    QFETCH(QString, active_key);
    QFETCH(QString, enabled_key);
    QFETCH(QString, initially_active);
    QFETCH(QStringList, initially_enabled);
    QFETCH(QString, expected_active_plugin);
    QFETCH(QString, expected_active_id);
    QFETCH(int, expected_enabled_count);
    QFETCH(int, expected_active_index);

    QSettings settings(ORGANIZATION, APPLICATION);

    if (not initially_active.isEmpty()) {
        settings.setValue(active_key, initially_active);
    }

    if (not initially_enabled.isEmpty()) {
        settings.setValue(enabled_key, initially_enabled);
    }

    MImOnScreenPlugins plugins;
    MImOnScreenPlugins::SubView active = plugins.activeSubView();
    QCOMPARE(active.plugin, expected_active_plugin);
    QCOMPARE(active.id, expected_active_id);

    QList<MImOnScreenPlugins::SubView> enabled = plugins.enabledSubViews(active.plugin);
    QCOMPARE(enabled.size(), expected_enabled_count);
    QCOMPARE(enabled.at(expected_active_index), active);
}

QTEST_MAIN(Ut_MImOnScreenPlugins)
