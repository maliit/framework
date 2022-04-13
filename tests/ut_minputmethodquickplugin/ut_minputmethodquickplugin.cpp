/* * This file is part of Maliit framework *
 *
 * Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 *
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

void Ut_MInputMethodQuickPlugin::testQmlSetup()
{
    QString testPlugin("helloworld.qml");
    const QDir pluginDir = MaliitTestUtils::isTestingInSandbox()
            ? QDir(MaliitTestUtils::getTestDataPath() + "/qmlplugin")
            : QDir(MALIIT_TEST_PLUGINS_DIR"/qml/helloworld");
    const QString pluginPath = pluginDir.absoluteFilePath(testPlugin);
    const QString pluginId = QFileInfo(testPlugin).baseName();
    QVERIFY(pluginDir.exists(pluginPath));

    Maliit::Plugins::InputMethodPlugin *plugin
            = new Maliit::InputMethodQuickPlugin(pluginPath,
                                                 QSharedPointer<Maliit::AbstractPlatform>(new Maliit::UnknownPlatform));

    MaliitTestUtils::TestInputMethodHost host(pluginId, plugin->name());
    Maliit::InputMethodQuick *testee = static_cast<Maliit::InputMethodQuick *>(
        plugin->createInputMethod(&host));

    QVERIFY(not testee->inputMethodArea().isEmpty());
    QCOMPARE(testee->inputMethodArea(),
             QRectF(0, qRound(testee->screenHeight() * 0.5),
                    testee->screenWidth(), qRound(testee->screenHeight() * 0.5)));

    QCOMPARE(host.lastCommit, QString("Maliit"));
    QCOMPARE(host.sendCommitCount, 1);
    QCOMPARE(host.lastPreedit, QString("Maliit"));
    QCOMPARE(host.sendPreeditCount, 1);
}

QTEST_MAIN(Ut_MInputMethodQuickPlugin)
