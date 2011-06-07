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
#include "mimapplication.h"

#include <minputmethodhost.h>
#include <minputmethodplugin.h>
#include <QtCore>
#include <QtGui>

class MIndicatorServiceClient
{};

class TestInputMethodHost
    : public MInputMethodHost
{
public:
    QString lastCommit;
    int sendCommitCount;

    QString lastPreedit;
    int sendPreeditCount;

    TestInputMethodHost(MIndicatorServiceClient &client)
        : MInputMethodHost(0, 0, client, 0)
        , sendCommitCount(0)
        , sendPreeditCount(0)
    {}

    void sendCommitString(const QString &string,
                          int, int, int)
    {
        lastCommit = string;
        ++sendCommitCount;
    }

    void sendPreeditString(const QString &string,
                           const QList<MInputMethod::PreeditTextFormat> &,
                           int , int , int)
    {
        lastPreedit = string;
        ++sendPreeditCount;
    }
};

void Ft_ExamplePlugin::initTestCase()
{
    static char *argv[2] = { (char *) "Ft_ExamplePlugin",
                             (char *) "-software" };
    static int argc = 2;

    // Enforcing raster GS to make test reliable:
    QApplication::setGraphicsSystem("raster");

    app = new MIMApplication(argc, argv);
}

void Ft_ExamplePlugin::cleanupTestCase()
{
    delete app;
}

void Ft_ExamplePlugin::init()
{}

void Ft_ExamplePlugin::cleanup()
{}

void Ft_ExamplePlugin::testFunction()
{
    MIndicatorServiceClient fakeService;
    TestInputMethodHost host(fakeService);

    const QDir pluginDir = MaliitTestUtils::isTestingInSandbox() ? QDir(IN_TREE_TEST_PLUGIN_DIR) : QDir(MALIIT_TEST_PLUGINS_DIR);
    const QString pluginPath = pluginDir.absoluteFilePath("libexampleplugin.so");
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
