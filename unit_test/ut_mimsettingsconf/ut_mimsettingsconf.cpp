#include "ut_mimsettingsconf.h"
#include "mimsettingsconf.h"
#include "mgconfitem_stub.h"
#include "fakegconf.h"

#include <QProcess>
#include <QRegExp>
#include <QCoreApplication>
#include <mimpluginmanager.h>
#include <minputmethodplugin.h>

Q_DECLARE_METATYPE(MIMHandlerState);

namespace
{
    const QString GlobalTestPluginPath("/usr/lib/meego-im-framework-tests/plugins");
    const QString TestPluginPathEnvVariable("TESTPLUGIN_PATH");

    const QString ConfigRoot          = "/meegotouch/inputmethods/";
    const QString MImPluginPaths    = ConfigRoot + "paths";
    const QString MImPluginDisabled = ConfigRoot + "disabledpluginfiles";

    const QString PluginRoot          = "/meegotouch/inputmethods/plugins/";
    const QString MImHandlerToPlugin  = PluginRoot + "handler";

    const QString pluginName  = "DummyImPlugin";
    const QString pluginName2 = "DummyImPlugin2";
    const QString pluginName3 = "DummyImPlugin3";
}


void Ut_MIMSettingsConf::initTestCase()
{
    char *argv[1] = { (char *) "ut_mimsettingsconf" };
    int argc = 1;

    app = new QCoreApplication(argc, argv);

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

    MGConfItem pathConf(MImPluginPaths);
    pathConf.set(pluginPath);
    MGConfItem blackListConf(MImPluginDisabled);

    QStringList blackList;
    blackList << "libdummyimplugin2.so";
    //ignore the meego-keyboard
    blackList << "libmeego-keyboard.so";
    blackListConf.set(blackList);
    MGConfItem handlerItem(QString("%1/%2").arg(MImHandlerToPlugin).arg(OnScreen));
    handlerItem.set(pluginName);

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
    MImSettingsConf::instance().setActivePlugin(pluginName);
    MImSettingsConf::instance().setActiveSubView(QString("dummyimsv1"));
    handleMessages();
    QCOMPARE(manager->activePluginsName(OnScreen), pluginName);
    QCOMPARE(manager->activeSubView(OnScreen), QString("dummyimsv1"));
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

void Ut_MIMSettingsConf::testSetActivePlugin()
{
    QCOMPARE(MImSettingsConf::instance().plugins().size(), 2);
    QCOMPARE(manager->activePluginsName(OnScreen), pluginName);

    MImSettingsConf::instance().setActivePlugin(pluginName3);
    handleMessages();
    MGConfItem handlerItem(QString("%1/%2").arg(MImHandlerToPlugin).arg(OnScreen));
    QCOMPARE(handlerItem.value().toString(), pluginName3);
    QVERIFY(manager->activePluginsNames().size() == 1);
    QCOMPARE(manager->activePluginsName(OnScreen), pluginName3);

    MImSettingsConf::instance().setActivePlugin(pluginName, QString("dummyimsv2"));
    handleMessages();
    QCOMPARE(handlerItem.value().toString(), pluginName);
    QVERIFY(manager->activePluginsNames().size() == 1);
    QCOMPARE(manager->activePluginsName(OnScreen), pluginName);
    QCOMPARE(manager->activeSubView(OnScreen), QString("dummyimsv2"));

}

void Ut_MIMSettingsConf::testSubViews()
{
    QList<MImSettingsConf::MImSubView> subViews = MImSettingsConf::instance().subViews();
    // only has subviews for OnScreen
    QCOMPARE(subViews.count(), 4);
}

void Ut_MIMSettingsConf::testActiveSubView()
{
    MImSettingsConf::MImSubView activeSubView = MImSettingsConf::instance().activeSubView();
    QCOMPARE(activeSubView.subViewId, QString("dummyimsv1"));

    MImSettingsConf::instance().setActiveSubView(QString("dummyimsv2"));
    handleMessages();
    activeSubView = MImSettingsConf::instance().activeSubView();
    QCOMPARE(activeSubView.subViewId, QString("dummyimsv2"));

    MImSettingsConf::instance().setActivePlugin(pluginName3);
    handleMessages();
    activeSubView = MImSettingsConf::instance().activeSubView();
    QCOMPARE(activeSubView.subViewId, QString("dummyim3sv1"));

    MImSettingsConf::instance().setActiveSubView(QString("dummyim3sv2"));
    handleMessages();
    activeSubView = MImSettingsConf::instance().activeSubView();
    QCOMPARE(activeSubView.subViewId, QString("dummyim3sv2"));
}

void Ut_MIMSettingsConf::handleMessages()
{
    while (app->hasPendingEvents()) {
        app->processEvents();
    }
}

QTEST_APPLESS_MAIN(Ut_MIMSettingsConf)
