#include "ft_duiimpluginmanager.h"
#include "duigconfitem_stub.h"
#include "duiinputcontextconnection_stub.h"

#include <QProcess>
#include <QGraphicsScene>
#include <QRegExp>
#include <QApplication>
#include <duiimpluginmanager.h>
#include <duiimpluginmanager_p.h>

namespace
{
    const QString GlobalTestPluginPath("/usr/lib/dui-im-framework-tests/plugins");
    const QString TestPluginPathEnvVariable("TESTPLUGIN_PATH");

    const QString pluginName  = "DummyImPlugin";
    const QString pluginName3 = "DummyImPlugin3";

    const QString ConfigRoot          = "/Dui/InputMethods/";
    const QString DuiImPluginPaths    = ConfigRoot + "Paths";
    const QString DuiImPluginActive   = ConfigRoot + "ActivePlugins";
    const QString DuiImPluginDisabled = ConfigRoot + "DisabledPluginFiles";

    const QString PluginRoot          = "/Dui/InputMethods/Plugins/";
    const QString DuiImStateToPlugin  = PluginRoot + "Handler";
    const QString DuiImActualState    = PluginRoot + "ActualHandler";
}


void Ft_DuiIMPluginManager::initTestCase()
{
    static char *argv[1] = { (char *) "ut_duiimpluginmanager" };
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

void Ft_DuiIMPluginManager::cleanupTestCase()
{
    delete app;
}

void Ft_DuiIMPluginManager::init()
{
    DuiGConfItem(DuiImPluginPaths).set(pluginPath);
    DuiGConfItem(DuiImPluginDisabled).set(QStringList("libdummyimplugin2.so"));
    DuiGConfItem(DuiImPluginActive).set(QStringList("DummyImPlugin"));

    DuiGConfItem(DuiImStateToPlugin + "/0").set(pluginName);
    DuiGConfItem(DuiImStateToPlugin + "/1").set(pluginName);
    DuiGConfItem(DuiImStateToPlugin + "/2").set(pluginName3);

    DuiGConfItem(DuiImActualState).set(QStringList("0"));

    subject = new DuiIMPluginManager();
}

void Ft_DuiIMPluginManager::cleanup()
{
    delete subject;
}


// Test methods..............................................................

void Ft_DuiIMPluginManager::testLoadPlugins()
{
    QStringList loadedPlugins = subject->loadedPluginsNames();
    QCOMPARE(loadedPlugins.count(), 2);
    QVERIFY(loadedPlugins.contains(pluginName));
    QVERIFY(loadedPlugins.contains(pluginName3));

    QStringList activePlugins = subject->activePluginsNames();
    QCOMPARE(activePlugins.count(), 1);
    QCOMPARE(activePlugins.first(), pluginName);

    QStringList activeInputMethods = subject->activeInputMethodsNames();
    QCOMPARE(activePlugins, activeInputMethods);
}


void Ft_DuiIMPluginManager::testSwitchPlugin()
{
    DuiGConfItem(DuiImActualState).set(QStringList("2"));

    QStringList loadedPlugins = subject->loadedPluginsNames();
    QCOMPARE(loadedPlugins.count(), 2);
    QVERIFY(loadedPlugins.contains(pluginName));
    QVERIFY(loadedPlugins.contains(pluginName3));

    QStringList activePlugins = subject->activePluginsNames();
    QCOMPARE(activePlugins.count(), 1);
    QCOMPARE(activePlugins.first(), pluginName3);
}


void Ft_DuiIMPluginManager::testMultilePlugins()
{
    QStringList states;
    states << "1" << "2";
    DuiGConfItem(DuiImActualState).set(states);

    QStringList loadedPlugins = subject->loadedPluginsNames();
    QCOMPARE(loadedPlugins.count(), 2);
    QVERIFY(loadedPlugins.contains(pluginName));
    QVERIFY(loadedPlugins.contains(pluginName3));

    QStringList activePlugins = subject->activePluginsNames();
    QCOMPARE(activePlugins.count(), 2);
    QVERIFY(activePlugins.contains(pluginName));
    QVERIFY(activePlugins.contains(pluginName3));
}

void Ft_DuiIMPluginManager::testFreeInputMethod()
{
    subject->setDeleteIMTimeout(300);

    DuiGConfItem(DuiImActualState).set(QStringList("2"));

    QStringList activeInputMethods = subject->activeInputMethodsNames();
    QCOMPARE(activeInputMethods.count(), 2);

    QTest::qWait(500); //wait intil timer expiration

    activeInputMethods = subject->activeInputMethodsNames();
    QCOMPARE(activeInputMethods.count(), 1);
    QCOMPARE(activeInputMethods.first(), pluginName3);
}

QTEST_APPLESS_MAIN(Ft_DuiIMPluginManager)

