/* * This file is part of meego-im-framework *
 *
 * Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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
#include "ut_minputmethodquickplugin.h"
#include "mimapplication.h"

#include <minputmethodquickplugin.h>
#include <minputmethodquick.h>
#include <minputmethodhost.h>
#include <QtCore>
#include <QtGui>

class MIndicatorServiceClient
{};

void Ut_MInputMethodQuickPlugin::initTestCase()
{
    static char *argv[2] = { (char *) "Ut_MInputMethodQuickPlugin",
                             (char *) "-software" };
    static int argc = 2;

    // Enforcing raster GS to make test reliable:
    QApplication::setGraphicsSystem("raster");

    app = new MIMApplication(argc, argv);
}

void Ut_MInputMethodQuickPlugin::cleanupTestCase()
{
    delete app;
}

void Ut_MInputMethodQuickPlugin::init()
{}

void Ut_MInputMethodQuickPlugin::cleanup()
{}

void Ut_MInputMethodQuickPlugin::testQmlSetup_data()
{
    QTest::addColumn<QString>("testPluginPath");
    QTest::newRow("Hello world")
        << "helloworld/libqmlhelloworldplugin.so";
    QTest::newRow("Cycle keys")
        << "cyclekeys/libqmlcyclekeysplugin.so";
    QTest::newRow("Override")
        << "override/libqmloverrideplugin.so";
}

/* This test currently tests both the qml example found in examples/
 * and the minputmethodquick interface, since the test is so simple.
 * If more tests are added, it might make sense to make these two
 * things tested in separate tests. */
void Ut_MInputMethodQuickPlugin::testQmlSetup()
{
    MIndicatorServiceClient fakeService;
    MaliitTestUtils::TestInputMethodHost host(fakeService);
    QFETCH(QString, testPluginPath);

    const QDir pluginDir = MaliitTestUtils::isTestingInSandbox() ?
                QDir(IN_TREE_TEST_PLUGIN_DIR"/qml") : QDir(MALIIT_TEST_PLUGINS_DIR"/examples/qml");
    const QString pluginPath = pluginDir.absoluteFilePath(testPluginPath);
    QVERIFY(pluginDir.exists(pluginPath));

    QPluginLoader loader(pluginPath);
    QObject *pluginInstance = loader.instance();
    QVERIFY(pluginInstance != 0);

    MInputMethodPlugin *plugin =  qobject_cast<MInputMethodPlugin *>(pluginInstance);
    QVERIFY(plugin != 0);

    MInputMethodQuick *testee = static_cast<MInputMethodQuick *>(
        plugin->createInputMethod(&host, new QWidget));

    QVERIFY(not testee->inputMethodArea().isEmpty());
    QCOMPARE(testee->inputMethodArea(), QRect(0, testee->screenHeight() * 0.5,
                                              testee->screenWidth(), testee->screenHeight() * 0.5));

    QCOMPARE(host.lastCommit, QString("Maliit"));
    QCOMPARE(host.sendCommitCount, 1);
    QCOMPARE(host.lastPreedit, QString("Maliit"));
    QCOMPARE(host.sendPreeditCount, 1);
}

QTEST_APPLESS_MAIN(Ut_MInputMethodQuickPlugin)
