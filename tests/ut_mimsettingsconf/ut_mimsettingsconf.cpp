#include "ut_mimsettingsconf.h"
#include "mimsettingsconf.h"
#include "mimsettings_stub.h"
#include "fakegconf.h"
#include "mimapplication.h"

#include <QProcess>
#include <QRegExp>
#include <QCoreApplication>
#include <mimpluginmanager.h>
#include <minputmethodplugin.h>

Q_DECLARE_METATYPE(MInputMethod::HandlerState);

namespace
{
    const QString GlobalTestPluginPath(MALIIT_TEST_PLUGINS_DIR);
    const QString TestPluginPathEnvVariable("TESTPLUGIN_PATH");

    const QString ConfigRoot          = "/meegotouch/inputmethods/";
    const QString MImPluginPaths    = ConfigRoot + "paths";
    const QString MImPluginDisabled = ConfigRoot + "disabledpluginfiles";

    const QString PluginRoot          = "/meegotouch/inputmethods/plugins/";

    const QString EnabledPluginsKey = "/meegotouch/inputmethods/onscreen/enabled";
    const QString ActivePluginKey = "/meegotouch/inputmethods/onscreen/active";

    const QString pluginName  = "DummyImPlugin";
    const QString pluginName2 = "DummyImPlugin2";
    const QString pluginName3 = "DummyImPlugin3";
    const QString pluginId  = "libdummyimplugin.so";
    const QString pluginId2 = "libdummyimplugin2.so";
    const QString pluginId3 = "libdummyimplugin3.so";
}


void Ut_MIMSettingsConf::initTestCase()
{
    static char *argv[1] = { (char *) "ut_mimsettingsconf" };
    static int argc = 1;

    app = new MIMApplication(argc, argv);

    // Use either global test plugin directory or TESTPLUGIN_PATH, if it is
    // set (to local sandbox's plugin directory by makefile, at least).
    pluginPath = GlobalTestPluginPath;

    const QStringList env(QProcess::systemEnvironment());
    int index = env.indexOf(QRegExp('^' + TestPluginPathEnvVariable + "=.*", Qt::CaseInsensitive));
    if (index != -1) {
        QString pathCandidate = env.at(index);
        pathCandidate = pathCandidate.remove(
                            QRegExp('^' + TestPluginPathEnvVariable + '=', Qt::CaseInsensitive));
        if (!pathCandidate.isEmpty()) {
            pluginPath = pathCandidate;
        } else {
            qDebug() << "Invalid " << TestPluginPathEnvVariable << " environment variable.\n";
            QFAIL("Attempt to execute test incorrectly.");
        }
    }
    QVERIFY2(QDir(pluginPath).exists(), "Test plugin directory does not exist.");

    MImSettings pathConf(MImPluginPaths);
    pathConf.set(pluginPath);
    MImSettings blackListConf(MImPluginDisabled);

    QStringList blackList;
    blackList << "libdummyimplugin2.so";
    //ignore the meego-keyboard
    blackList << "libmeego-keyboard.so";
    blackListConf.set(blackList);

    MImSettings enabledPluginsSettings(EnabledPluginsKey);
    QStringList enabledPlugins;
    enabledPlugins << pluginId << "dummyimsv1";
    enabledPlugins << pluginId << "dummyimsv2";
    enabledPlugins << pluginId3 << "dummyim3sv1";
    enabledPlugins << pluginId3 << "dummyim3sv2";
    enabledPluginsSettings.set(enabledPlugins);

    MImSettings activePluginSettings(ActivePluginKey);
    QStringList activePlugin;
    activePlugin << pluginId << "dummyimsv1";
    activePluginSettings.set(activePlugin);

    manager = new MIMPluginManager();
    if (!manager->isDBusConnectionValid()) {
        QSKIP("MIMPluginManager dbus connection is not valid. Possibly other program using it running.",
              SkipAll);
    }
}

void Ut_MIMSettingsConf::cleanupTestCase()
{
    delete manager;
    delete app;
}

void Ut_MIMSettingsConf::init()
{
    MImSettingsConf::createInstance();
    handleMessages();
    QCOMPARE(manager->activePluginsName(MInputMethod::OnScreen), pluginId);
    QCOMPARE(manager->activeSubView(MInputMethod::OnScreen), QString("dummyimsv1"));
}

void Ut_MIMSettingsConf::cleanup()
{
    MImSettingsConf::destroyInstance();
}


// Test methods..............................................................

void Ut_MIMSettingsConf::testPlugins()
{
    QCOMPARE(MImSettingsConf::instance().plugins().size(), 2);

    QStringList expectedPlugins;
    expectedPlugins << pluginName << pluginName3;
    foreach (const MInputMethodPlugin *plugin, MImSettingsConf::instance().plugins()) {
        QVERIFY(expectedPlugins.contains(plugin->name()));
    }
}

void Ut_MIMSettingsConf::testSubViews()
{
    QList<MImSettingsConf::MImSubView> subViews = MImSettingsConf::instance().subViews();
    // only has subviews for OnScreen
    QCOMPARE(subViews.count(), 4);
}

void Ut_MIMSettingsConf::handleMessages()
{
    while (app->hasPendingEvents()) {
        app->processEvents();
    }
}

QTEST_APPLESS_MAIN(Ut_MIMSettingsConf)
