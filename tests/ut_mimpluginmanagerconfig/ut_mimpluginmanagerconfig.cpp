/* * This file is part of Maliit framework *
 *
 * Copyright (C) 2012 One Laptop per Child Association
 *
 * Contact: maliit-discuss@lists.maliit.org
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 */

#include "ut_mimpluginmanagerconfig.h"

#include "dummyimplugin.h"
#include "dummyimplugin3.h"
#include "dummyinputmethod.h"
#include "dummyinputmethod3.h"

#include "minputcontextconnection.h"
#include "mimsettingsqsettings.h"

#include "core-utils.h"

#include <QTest>
#include <QProcess>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QWidget>
#include <QRegExp>
#include <QCoreApplication>
#include <QPointer>
#include <QTimer>
#include <QEventLoop>
#include <QStringList>
#include <mimpluginmanager.h>
#include <mimpluginmanager_p.h>
#include <maliit/plugins/inputmethodplugin.h>

#include "mattributeextensionmanager.h"
#include "msharedattributeextensionmanager.h"

using namespace std::tr1;

typedef QSet<Maliit::HandlerState> HandlerStates;
Q_DECLARE_METATYPE(HandlerStates);
Q_DECLARE_METATYPE(Maliit::HandlerState);

namespace {
    const QString Organization = "maliit.org";
    const QString Application = "server-tests";

    const QString ConfigRoot = MALIIT_CONFIG_ROOT;
    const QString MImPluginPaths = ConfigRoot + "paths";

    const QString EnabledPluginsKey = MALIIT_CONFIG_ROOT"onscreen/enabled";
    const QString ActivePluginKey = MALIIT_CONFIG_ROOT"onscreen/active";

    const QString pluginName  = "DummyImPlugin";
    const QString pluginName2 = "DummyImPlugin2";
    const QString pluginName3 = "DummyImPlugin3";
    const QString pluginId = "libdummyimplugin.so";
    const QString pluginId2 = "libdummyimplugin2.so";
    const QString pluginId3 = "libdummyimplugin3.so";

    const QStringList DefaultEnabledPlugins = QStringList()
                                              << pluginId + ":" + "dummyimsv1"
                                              << pluginId + ":" + "dummyimsv2"
                                              << pluginId3 + ":" + "dummyim3sv1"
                                              << pluginId3 + ":" + "dummyim3sv2";
}


void Ut_MIMPluginManagerConfig::initTestCase()
{
    MImSettings::setPreferredSettingsType(MImSettings::TemporarySettings);
    MImSettings::setImplementationFactory(new MImSettingsQSettingsBackendFactory(Organization, Application));

    // Make sure we start with empty/non-existing config file:
    QSettings settings(Organization, Application);
    QFile file(settings.fileName());
    file.remove();
}
 
void Ut_MIMPluginManagerConfig::cleanupTestCase()
{
    // Make sure we remove the config file at the end, too:
    QSettings settings(Organization, Application);
    QFile file(settings.fileName());
    file.remove();
}

void Ut_MIMPluginManagerConfig::init()
{
    MImSettings pathConf(MImPluginPaths);
    pathConf.set(MaliitTestUtils::getTestPluginPath());
    connection = new MInputContextConnection;
    enabledPluginSettings = new MImSettings(EnabledPluginsKey);
    activePluginSettings = new MImSettings(ActivePluginKey);
}

void Ut_MIMPluginManagerConfig::cleanup()
{
    delete manager;
    delete connection;
    delete enabledPluginSettings;
    delete activePluginSettings;
    manager = 0;
    subject = 0;
}


// Test methods..............................................................

void Ut_MIMPluginManagerConfig::testNoActiveSubView()
{
    enabledPluginSettings->set(DefaultEnabledPlugins);
    activePluginSettings->unset();

    QSharedPointer<MInputContextConnection> icConnection(connection);
    manager = new MIMPluginManager(icConnection);
    subject = manager->d_ptr;

    // The first enabled subview should have been auto-selected as the
    // active subview
    MImOnScreenPlugins::SubView subView = subject->onScreenPlugins.activeSubView();
    QCOMPARE(subView.plugin + ":" + subView.id, DefaultEnabledPlugins.first());
}

void Ut_MIMPluginManagerConfig::testEmptyConfig()
{
    enabledPluginSettings->unset();
    activePluginSettings->unset();

    QSharedPointer<MInputContextConnection> icConnection(connection);
    manager = new MIMPluginManager(icConnection);
    subject = manager->d_ptr;

    // One subview should have been auto-activated and enabled.
    MImOnScreenPlugins& plugins = subject->onScreenPlugins;
    MImOnScreenPlugins::SubView active = plugins.activeSubView();
    QList<MImOnScreenPlugins::SubView> enabled = plugins.enabledSubViews(active.plugin);

    QCOMPARE(enabled.size(), 1);
    QCOMPARE(enabled.first(), active);
}

void Ut_MIMPluginManagerConfig::autoLanguageSubView()
{
    // Force autodetection of enabled plugins in DummyPlugin3
    enabledPluginSettings->unset();
    activePluginSettings->set(QStringList() << pluginId3 + ":");

    QSharedPointer<MInputContextConnection> icConnection(connection);
    manager = new MIMPluginManager(icConnection);
    subject = manager->d_ptr;

    MImOnScreenPlugins& plugins = subject->onScreenPlugins;
    QList<MImOnScreenPlugins::SubView> enabled = plugins.enabledSubViews(pluginId3);

    // Expect 3 results; ur_PK should have been skipped (no match)
    QCOMPARE(enabled.size(), 3);

    // First one should be en_gb - simple match
    QCOMPARE(enabled[0].id, QString("en_gb"));

    // Check that we matched after stripping the country code
    QCOMPARE(enabled[1].id, QString("es"));

    // Check that we matched after auto-adding the country code
    QCOMPARE(enabled[2].id, QString("fr_fr"));
}

int main(int argc, char **argv)
{
    // Provide our own main function so that we can override LANGUAGE
    // before Qt gets loaded.
    // FIXME: Find a cleaner way to set LANGUAGE env that allows us to use
    // QTEST_MAIN macro again.
    setenv("LANGUAGE", "en_GB.utf8:ur_PK.utf8:es_NI.utf8:fr", 1);

    Ut_MIMPluginManagerConfig test;

    // Qt5 crashes when not supplying argc & argv.
    // TODO: File bug report.
    QTest::qExec(&test, argc, argv);
    return 0;
}
