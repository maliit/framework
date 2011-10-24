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
#include "ft_exampleplugin.h"

#include <minputmethodhost.h>
#include <minputmethodplugin.h>
#include <minputcontextconnection.h>

#include <QtCore>
#include <QtGui>

class MIndicatorServiceClient
{};

void Ft_ExamplePlugin::initTestCase()
{
    static char *argv[2] = { (char *) "Ft_ExamplePlugin",
                             (char *) "-software" };
    static int argc = 2;

    // Enforcing raster GS to make test reliable:
    QApplication::setGraphicsSystem("raster");

    app = new QApplication(argc, argv);
}

void Ft_ExamplePlugin::cleanupTestCase()
{
    delete app;
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
    MaliitTestUtils::TestInputMethodHost host(fakeService);

    const QDir pluginDir = MaliitTestUtils::isTestingInSandbox() ?
                QDir(IN_TREE_TEST_PLUGIN_DIR"/cxx") : QDir(MALIIT_TEST_PLUGINS_DIR"/examples/cxx");
    const QString pluginPath = pluginDir.absoluteFilePath(testPluginPath);
    QVERIFY(pluginDir.exists(pluginPath));

    QPluginLoader loader(pluginPath);
    QObject *pluginInstance = loader.instance();
    QVERIFY(pluginInstance != 0);

    MInputMethodPlugin *plugin =  qobject_cast<MInputMethodPlugin *>(pluginInstance);
    QVERIFY(plugin != 0);

    plugin->createInputMethod(&host, new QWidget);

    QCOMPARE(host.lastCommit, QString("Maliit"));
    QCOMPARE(host.sendCommitCount, 1);
    QCOMPARE(host.lastPreedit, QString("Mali"));
    QCOMPARE(host.sendPreeditCount, 1);
}

QTEST_APPLESS_MAIN(Ft_ExamplePlugin)
