#include "ft_mimpluginmanager.h"
#include "mgconfitem_stub.h"
#include "mkeyboardstatetracker_stub.h"
#include "minputcontextconnection_stub.h"

#include <QProcess>
#include <QGraphicsScene>
#include <QRegExp>
#include <QApplication>
#include <mimpluginmanager.h>
#include <mimpluginmanager_p.h>

namespace
{
    const QString GlobalTestPluginPath("/usr/lib/m-im-framework-tests/plugins");
    const QString TestPluginPathEnvVariable("TESTPLUGIN_PATH");

    const QString pluginName  = "DummyImPlugin";
    const QString pluginName3 = "DummyImPlugin3";

    const QString ConfigRoot          = "/Dui/InputMethods/";
    const QString MImPluginPaths    = ConfigRoot + "Paths";
    const QString MImPluginActive   = ConfigRoot + "ActivePlugins";
    const QString MImPluginDisabled = ConfigRoot + "DisabledPluginFiles";

    const QString PluginRoot          = "/Dui/InputMethods/Plugins/";
    const QString MImStateToPlugin  = PluginRoot + "Handler";
    const QString MImAccesoryEnabled  = "/Dui/InputMethods/AccessoryEnabled";
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

    QStringList activeInputMethods = subject->activeInputMethodsNames();
    QCOMPARE(activePlugins, activeInputMethods);
}


void Ft_MIMPluginManager::testSwitchPlugin()
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

void Ft_MIMPluginManager::testFreeInputMethod()
{
    subject->setDeleteIMTimeout(300);

    MGConfItem(MImAccesoryEnabled).set(QVariant(true));

    QStringList activeInputMethods = subject->activeInputMethodsNames();
    QCOMPARE(activeInputMethods.count(), 2);

    QTest::qWait(500); //wait intil timer expiration

    activeInputMethods = subject->activeInputMethodsNames();
    QCOMPARE(activeInputMethods.count(), 1);
    QCOMPARE(activeInputMethods.first(), pluginName3);
}

QTEST_APPLESS_MAIN(Ft_MIMPluginManager)

