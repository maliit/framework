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

#include "ft_exampleplugin.h"
#include "core-utils.h"
#include "gui-utils.h"

#include <minputmethodhost.h>
#include <maliit/plugins/inputmethodplugin.h>
#include <minputcontextconnection.h>

#include <QtCore>
#include <QtGui>

class MIndicatorServiceClient
{};

void Ft_ExamplePlugin::initTestCase()
{
}

void Ft_ExamplePlugin::cleanupTestCase()
{
}

void Ft_ExamplePlugin::init()
{}

void Ft_ExamplePlugin::cleanup()
{}


void Ft_ExamplePlugin::testFunction_data()
{
    QTest::addColumn<QString>("testPluginPath");
    QTest::newRow("Hello world")
        << "helloworld/libcxxhelloworldplugin.so";
    QTest::newRow("Override")
        << "override/libcxxoverrideplugin.so";
}

void Ft_ExamplePlugin::testFunction()
{
    QFETCH(QString, testPluginPath);

    MIndicatorServiceClient fakeService;

    const QDir pluginDir = MaliitTestUtils::isTestingInSandbox() ?
                QDir(IN_TREE_TEST_PLUGIN_DIR"/cxx") : QDir(MALIIT_TEST_PLUGINS_DIR"/examples/cxx");
    const QString pluginPath = pluginDir.absoluteFilePath(testPluginPath);
    const QString pluginId = QFileInfo(testPluginPath).baseName();
    QVERIFY(pluginDir.exists(pluginPath));

    QPluginLoader loader(pluginPath);
    QObject *pluginInstance = loader.instance();
    QVERIFY(pluginInstance != 0);

    Maliit::Plugins::InputMethodPlugin *plugin =  qobject_cast<Maliit::Plugins::InputMethodPlugin *>(pluginInstance);
    QVERIFY(plugin != 0);

    MaliitTestUtils::TestInputMethodHost host(fakeService, pluginId, plugin->name());
    plugin->createInputMethod(&host);

    QCOMPARE(host.last_commit, QString("Maliit"));
    QCOMPARE(host.send_commit_count, 1);
    QCOMPARE(host.last_preedit, QString("Mali"));
    QCOMPARE(host.send_preedit_count, 1);
}

QTEST_MAIN(Ft_ExamplePlugin)
