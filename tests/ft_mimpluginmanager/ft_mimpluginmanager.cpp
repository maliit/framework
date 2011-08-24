#include "ft_mimpluginmanager.h"
#include "mimsettings_stub.h"
#include "mkeyboardstatetracker_stub.h"
#include "minputcontextconnection_stub.h"
#include "dummyimplugin.h"
#include "dummyinputmethod.h"
#include "mimapplication.h"

#include <QProcess>
#include <QGraphicsScene>
#include <QRegExp>
#include <QApplication>
#include <QPointer>
#include <mimpluginmanager.h>
#include <mimpluginmanager_p.h>

namespace
{
    const QString GlobalTestPluginPath(MALIIT_TEST_PLUGINS_DIR);
    const QString TestPluginPathEnvVariable("TESTPLUGIN_PATH");

    const QString pluginName  = "DummyImPlugin";
    const QString pluginName3 = "DummyImPlugin3";
    const QString pluginId  = "libdummyimplugin.so";
    const QString pluginId3 = "libdummyimplugin3.so";

    const QString EnabledPluginsKey = MALIIT_CONFIG_ROOT"onscreen/enabled";
    const QString ActivePluginKey =   MALIIT_CONFIG_ROOT"onscreen/active";

    const QString ConfigRoot        = MALIIT_CONFIG_ROOT;
    const QString MImPluginPaths    = ConfigRoot + "paths";
    const QString MImPluginActive   = ConfigRoot + "activeplugins";
    const QString MImPluginDisabled = ConfigRoot + "disabledpluginfiles";

    const QString PluginRoot         = MALIIT_CONFIG_ROOT"plugins/";
    const QString MImAccesoryEnabled = MALIIT_CONFIG_ROOT"accessoryenabled";

    const MImPluginDescription* findPluginDescriptions(const QList<MImPluginDescription> &list, const QString &pluginName)
    {
        foreach (const MImPluginDescription &desc, list) {
            if (desc.name() == pluginName) {
                return &desc;
            }
        }

        return 0;
    }
}


// Initialization............................................................

void Ft_MIMPluginManager::initTestCase()
{
    static char *argv[1] = { (char *) "ut_mimpluginmanager" };
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
}

void Ft_MIMPluginManager::cleanupTestCase()
{
    delete app;
}

void Ft_MIMPluginManager::init()
{
    MImSettings(MImPluginPaths).set(pluginPath);
    MImSettings(MImPluginDisabled).set(QStringList("libdummyimplugin2.so"));
    MImSettings(MImPluginActive).set(QStringList("DummyImPlugin"));

    MImSettings(PluginRoot + "hardware").set(pluginId);
    MImSettings(PluginRoot + "accessory").set(pluginId3);

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

    gMKeyboardStateTrackerStub->setOpenState(false);
    MImSettings(MImAccesoryEnabled).set(QVariant(false));

    subject = new MIMPluginManager();
}

void Ft_MIMPluginManager::cleanup()
{
    delete subject;
}


// Test methods..............................................................

void Ft_MIMPluginManager::testLoadPlugins()
{
    QStringList loadedPlugins = subject->loadedPluginsNames();
    QCOMPARE(loadedPlugins.count(), 2);
    QVERIFY(loadedPlugins.contains(pluginId));
    QVERIFY(loadedPlugins.contains(pluginId3));

    QStringList activePlugins = subject->activePluginsNames();
    QCOMPARE(activePlugins.count(), 1);
    QCOMPARE(activePlugins.first(), pluginId);
}


void Ft_MIMPluginManager::testSwitchPluginState()
{
    MImSettings(MImAccesoryEnabled).set(QVariant(true));

    QStringList loadedPlugins = subject->loadedPluginsNames();
    QCOMPARE(loadedPlugins.count(), 2);
    QVERIFY(loadedPlugins.contains(pluginId));
    QVERIFY(loadedPlugins.contains(pluginId3));

    QStringList activePlugins = subject->activePluginsNames();
    QCOMPARE(activePlugins.count(), 1);
    QCOMPARE(activePlugins.first(), pluginId3);
}


void Ft_MIMPluginManager::testMultiplePlugins()
{
    //QSKIP("This test fails to activate > 1 plugins, for unknown reasons.", SkipAll);

    gMKeyboardStateTrackerStub->setOpenState(true);
    MImSettings(MImAccesoryEnabled).set(QVariant(true));

    QStringList loadedPlugins = subject->loadedPluginsNames();
    QCOMPARE(loadedPlugins.count(), 2);
    QVERIFY(loadedPlugins.contains(pluginId));
    QVERIFY(loadedPlugins.contains(pluginId3));

    QStringList activePlugins = subject->activePluginsNames();
    QCOMPARE(activePlugins.count(), 2);
    QVERIFY(activePlugins.contains(pluginId));
    QVERIFY(activePlugins.contains(pluginId3));
}

void Ft_MIMPluginManager::testSwitchPluginBySignal()
{
    DummyImPlugin *plugin = 0;
    QPointer<DummyInputMethod> inputMethod = 0;

    for (MIMPluginManagerPrivate::Plugins::iterator iterator(subject->d_ptr->plugins.begin());
            iterator != subject->d_ptr->plugins.end();
            ++iterator) {
        if (pluginName == iterator.key()->name()) {
            plugin = dynamic_cast<DummyImPlugin *>(iterator.key());
        }
    }

    QVERIFY(plugin != 0);
    inputMethod = dynamic_cast<DummyInputMethod *>(subject->d_ptr->plugins[plugin].inputMethod);
    QVERIFY(inputMethod != 0);

    inputMethod->switchMe();

    QStringList activePlugins = subject->activePluginsNames();
    QCOMPARE(activePlugins.count(), 1);
    QVERIFY(activePlugins.contains(pluginId3));
    QVERIFY(inputMethod != 0);
}

void Ft_MIMPluginManager::testSwitchToSpecifiedPlugin()
{
    DummyImPlugin *plugin = 0;
    QPointer<DummyInputMethod> inputMethod = 0;

    for (MIMPluginManagerPrivate::Plugins::iterator iterator(subject->d_ptr->plugins.begin());
            iterator != subject->d_ptr->plugins.end();
            ++iterator) {
        if (pluginName == iterator.key()->name()) {
            plugin = dynamic_cast<DummyImPlugin *>(iterator.key());
        }
    }

    QVERIFY(plugin != 0);
    inputMethod = dynamic_cast<DummyInputMethod *>(subject->d_ptr->plugins[plugin].inputMethod);
    QVERIFY(inputMethod != 0);

    inputMethod->switchMe(pluginId3);

    QStringList activePlugins = subject->activePluginsNames();
    QCOMPARE(activePlugins.count(), 1);
    QVERIFY(activePlugins.contains(pluginId3));
    QVERIFY(inputMethod != 0);
}

void Ft_MIMPluginManager::testPluginDescriptions()
{
    QSignalSpy spy(subject, SIGNAL(pluginsChanged()));
    QVERIFY(spy.isValid());

    MImSettings enabledPluginsSettings(EnabledPluginsKey);
    QStringList enabledPlugins;
    const MImPluginDescription *description = 0;

    enabledPlugins << pluginId << "dummyimsv1";
    enabledPluginsSettings.set(enabledPlugins);
    QCOMPARE(spy.count(), 1);

    description = findPluginDescriptions(subject->pluginDescriptions(MInputMethod::OnScreen), pluginName);
    QVERIFY(description);
    QVERIFY(description->enabled());
    description = findPluginDescriptions(subject->pluginDescriptions(MInputMethod::OnScreen), pluginName3);
    QVERIFY(description);
    QVERIFY(!description->enabled());
    description = 0;

    enabledPlugins << pluginId << "dummyimsv2";
    enabledPluginsSettings.set(enabledPlugins);
    QCOMPARE(spy.count(), 2);

    description = findPluginDescriptions(subject->pluginDescriptions(MInputMethod::OnScreen), pluginName);
    QVERIFY(description);
    QVERIFY(description->enabled());
    description = findPluginDescriptions(subject->pluginDescriptions(MInputMethod::OnScreen), pluginName3);
    QVERIFY(description);
    QVERIFY(!description->enabled());
    description = 0;

    enabledPlugins << pluginId3 << "dummyim3sv2";
    enabledPluginsSettings.set(enabledPlugins);
    QCOMPARE(spy.count(), 3);

    description = findPluginDescriptions(subject->pluginDescriptions(MInputMethod::OnScreen), pluginName);
    QVERIFY(description);
    QVERIFY(description->enabled());
    description = findPluginDescriptions(subject->pluginDescriptions(MInputMethod::OnScreen), pluginName3);
    QVERIFY(description);
    QVERIFY(description->enabled());
    description = 0;

    //at least we should not crash here
    enabledPluginsSettings.set(QStringList());
}

QTEST_APPLESS_MAIN(Ft_MIMPluginManager)

