/* * This file is part of Maliit framework *
 *
 * Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#include "ut_minputmethodquickplugin.h"
#include "core-utils.h"
#include "gui-utils.h"

#include <quick/inputmethodquickplugin.h>
#include <quick/inputmethodquick.h>
#include <minputmethodhost.h>
#include <QtCore>
#include <QtGui>

class MIndicatorServiceClient
{};

void Ut_MInputMethodQuickPlugin::initTestCase()
{
}

void Ut_MInputMethodQuickPlugin::cleanupTestCase()
{
}

void Ut_MInputMethodQuickPlugin::init()
{}

void Ut_MInputMethodQuickPlugin::cleanup()
{}

void Ut_MInputMethodQuickPlugin::testQmlSetup_data()
{
    QTest::addColumn<QString>("testPluginPath");
    QTest::newRow("Hello world")
        << "helloworld/helloworld.qml";
// TODO Adapt plugins for Qt 5
#if 0
    QTest::newRow("Cycle keys")
        << "cyclekeys/libqmlcyclekeysplugin.so";
    QTest::newRow("Override")
        << "override/libqmloverrideplugin.so";
#endif
}

/* This test currently tests both the qml example found in examples/
 * and the minputmethodquick interface, since the test is so simple.
 * If more tests are added, it might make sense to make these two
 * things tested in separate tests. */
void Ut_MInputMethodQuickPlugin::testQmlSetup()
{
    QFETCH(QString, testPluginPath);

    const QDir pluginDir = MaliitTestUtils::isTestingInSandbox() ?
                QDir(IN_TREE_TEST_PLUGIN_DIR"/qml") : QDir(MALIIT_TEST_PLUGINS_DIR"/examples/qml");
    const QString pluginPath = pluginDir.absoluteFilePath(testPluginPath);
    const QString pluginId = QFileInfo(testPluginPath).baseName();
    QVERIFY(pluginDir.exists(pluginPath));

    QObject *pluginInstance = 0;
    Maliit::Plugins::InputMethodPlugin *plugin = 0;

    if (pluginPath.endsWith(".qml")) {
        plugin = new Maliit::InputMethodQuickPlugin(pluginPath,
                                                    QSharedPointer<Maliit::AbstractPlatform>(new Maliit::UnknownPlatform));
    } else {
        QPluginLoader loader(pluginPath);
        pluginInstance = loader.instance();
        QVERIFY(pluginInstance != 0);
        plugin = qobject_cast<Maliit::Plugins::InputMethodPlugin *>(pluginInstance);
    }

    QVERIFY(plugin != 0);

    MaliitTestUtils::TestInputMethodHost host(pluginId, plugin->name());
    Maliit::InputMethodQuick *testee = static_cast<Maliit::InputMethodQuick *>(
        plugin->createInputMethod(&host));

    QVERIFY(not testee->inputMethodArea().isEmpty());
    QCOMPARE(testee->inputMethodArea(),
             QRectF(0, qRound(testee->screenHeight() * 0.5),
                    testee->screenWidth(), qRound(testee->screenHeight() * 0.5) - 1));

    QCOMPARE(host.lastCommit, QString("Maliit"));
    QCOMPARE(host.sendCommitCount, 1);
    QCOMPARE(host.lastPreedit, QString("Maliit"));
    QCOMPARE(host.sendPreeditCount, 1);
}

QTEST_MAIN(Ut_MInputMethodQuickPlugin)
