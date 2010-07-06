#include "ft_mimpluginmanager.h"
#include "mgconfitem_stub.h"
#include "mkeyboardstatetracker_stub.h"
#include "minputcontextconnection_stub.h"
#include "dummyimplugin.h"
#include "dummyinputmethod.h"

#include <QProcess>
#include <QGraphicsScene>
#include <QRegExp>
#include <QApplication>
#include <QPointer>
#include <mimpluginmanager.h>
#include <mimpluginmanager_p.h>

namespace
{
    const QString GlobalTestPluginPath("/usr/lib/meego-im-framework-tests/plugins");
    const QString TestPluginPathEnvVariable("TESTPLUGIN_PATH");

    const QString pluginName  = "DummyImPlugin";
    const QString pluginName3 = "DummyImPlugin3";

    const QString ConfigRoot        = "/meegotouch/inputmethods/";
    const QString MImPluginPaths    = ConfigRoot + "paths";
    const QString MImPluginActive   = ConfigRoot + "activeplugins";
    const QString MImPluginDisabled = ConfigRoot + "disabledpluginfiles";

    const QString PluginRoot         = "/meegotouch/inputmethods/plugins/";
    const QString MImStateToPlugin   = PluginRoot + "handler";
    const QString MImAccesoryEnabled = "/meegotouch/inputmethods/accessoryenabled";
}


void Ft_MIMPluginManager::initTestCase()
{
    static char *argv[1] = { (char *) "ut_mimpluginmanager" };
    static int argc = 1;

    app = new QApplication(argc, argv);

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
    MGConfItem(MImPluginPaths).set(pluginPath);
    MGConfItem(MImPluginDisabled).set(QStringList("libdummyimplugin2.so"));
    MGConfItem(MImPluginActive).set(QStringList("DummyImPlugin"));

    MGConfItem(MImStateToPlugin + "/0").set(pluginName);
    MGConfItem(MImStateToPlugin + "/1").set(pluginName);
    MGConfItem(MImStateToPlugin + "/2").set(pluginName3);

    gMKeyboardStateTrackerStub->setOpenState(false);
    MGConfItem(MImAccesoryEnabled).set(QVariant(false));

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
    QVERIFY(loadedPlugins.contains(pluginName));
    QVERIFY(loadedPlugins.contains(pluginName3));

    QStringList activePlugins = subject->activePluginsNames();
    QCOMPARE(activePlugins.count(), 1);
    QCOMPARE(activePlugins.first(), pluginName);
}


void Ft_MIMPluginManager::testSwitchPluginState()
{
    MGConfItem(MImAccesoryEnabled).set(QVariant(true));

    QStringList loadedPlugins = subject->loadedPluginsNames();
    QCOMPARE(loadedPlugins.count(), 2);
    QVERIFY(loadedPlugins.contains(pluginName));
    QVERIFY(loadedPlugins.contains(pluginName3));

    QStringList activePlugins = subject->activePluginsNames();
    QCOMPARE(activePlugins.count(), 1);
    QCOMPARE(activePlugins.first(), pluginName3);
}


void Ft_MIMPluginManager::testMultiplePlugins()
{
    gMKeyboardStateTrackerStub->setOpenState(true);
    MGConfItem(MImAccesoryEnabled).set(QVariant(true));

    QStringList loadedPlugins = subject->loadedPluginsNames();
    QCOMPARE(loadedPlugins.count(), 2);
    QVERIFY(loadedPlugins.contains(pluginName));
    QVERIFY(loadedPlugins.contains(pluginName3));

    QStringList activePlugins = subject->activePluginsNames();
    QCOMPARE(activePlugins.count(), 2);
    QVERIFY(activePlugins.contains(pluginName));
    QVERIFY(activePlugins.contains(pluginName3));
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

    inputMethod->switchMe(); //emit signal to switch plugin

    QStringList activePlugins = subject->activePluginsNames();
    QCOMPARE(activePlugins.count(), 1);
    QVERIFY(activePlugins.contains(pluginName3));
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

    inputMethod->switchMe(pluginName3); //emit signal to switch plugin

    QStringList activePlugins = subject->activePluginsNames();
    QCOMPARE(activePlugins.count(), 1);
    QVERIFY(activePlugins.contains(pluginName3));
    QVERIFY(inputMethod != 0);
}

QTEST_APPLESS_MAIN(Ft_MIMPluginManager)

