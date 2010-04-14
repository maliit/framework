#include "ut_mimpluginmanager.h"
#include "mgconfitem_stub.h"
#include "minputcontextconnection_stub.h"
#include "dummyimplugin.h"
#include "dummyimplugin3.h"
#include "dummyinputmethod.h"
#include "dummyinputmethod3.h"

#include <QProcess>
#include <QGraphicsScene>
#include <QRegExp>
#include <QCoreApplication>
#include <mimpluginmanager.h>
#include <mimpluginmanager_p.h>
#include <minputmethodplugin.h>

typedef QSet<MIMHandlerState> HandlerStates;
Q_DECLARE_METATYPE(HandlerStates);

namespace
{
    const QString GlobalTestPluginPath("/usr/lib/m-im-framework-tests/plugins");
    const QString TestPluginPathEnvVariable("TESTPLUGIN_PATH");

    const QString pluginName  = "DummyImPlugin";
    const QString pluginName3 = "DummyImPlugin3";
}


void Ut_MIMPluginManager::initTestCase()
{
    char *argv[1] = { (char *) "ut_mimpluginloader" };
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
}

void Ut_MIMPluginManager::cleanupTestCase()
{
    delete app;
}

void Ut_MIMPluginManager::init()
{
    subject = new MIMPluginManagerPrivate(new MInputContextConnectionStub, 0);

    subject->paths     << pluginPath;
    subject->blacklist << "libdummyimplugin2.so";
    subject->active    << "DummyImPlugin";

    subject->loadPlugins();
}

void Ut_MIMPluginManager::cleanup()
{
    delete subject;
}


// Test methods..............................................................

void Ut_MIMPluginManager::testLoadPlugins()
{
    MInputMethodPlugin *plugin = 0;
    MInputMethodPlugin *plugin3 = 0;

    // Initial load based on settings -> DummyImPlugin loaded and activated,
    // DummyImPlugin3 loaded, DummyPlugin2 not loaded (skipped).  Also,
    // DummyPlugin ignored (it is currently left loaded but even though that
    // might better be fixed, it is not relevant to this test) and
    // libinvalidplugin not loaded.  The only "test" for these two is that the
    // test does not crash.  (One may also observe the warning/debug messages
    // concerning loading of those two plugins.)
    foreach(MInputMethodPlugin * plugin, subject->plugins.keys()) {
        qDebug() << plugin->name();
    }
    QCOMPARE(subject->plugins.size(), 2);
    QVERIFY(subject->activePlugins.size() == 1);
    plugin = *subject->activePlugins.begin();
    QCOMPARE(plugin->name(), pluginName);
    bool dummyImPluginFound = false;
    bool dummyImPlugin3Found = false;
    foreach(MInputMethodPlugin * plugin, subject->plugins.keys()) {
        if (plugin->name() == "DummyImPlugin") {
            dummyImPluginFound = true;
        } else if (plugin->name() == "DummyImPlugin3") {
            dummyImPlugin3Found = true;
            plugin3 = plugin;
        }
    }
    QVERIFY(dummyImPluginFound);
    QVERIFY(dummyImPlugin3Found);

    // Try to activate DummyImPlugin again -> ignored
    subject->activatePlugin(plugin);
    QCOMPARE(subject->plugins.size(), 2);
    QVERIFY(subject->activePlugins.size() == 1);
    plugin = *subject->activePlugins.begin();
    QCOMPARE(plugin->name(), pluginName);

    // Try to activate DummyImPlugin3 -> activated
    subject->activatePlugin(plugin3);
    QCOMPARE(subject->plugins.size(), 2);
    QVERIFY(subject->activePlugins.size() == 2);
    dummyImPluginFound = false;
    dummyImPlugin3Found = false;
    foreach(plugin, subject->plugins.keys()) {
        if (plugin->name() == "DummyImPlugin") {
            dummyImPluginFound = true;
        } else if (plugin->name() == "DummyImPlugin3") {
            dummyImPlugin3Found = true;
        }
    }
    QVERIFY(dummyImPluginFound);
    QVERIFY(dummyImPlugin3Found);

    // Try to activate a plugin that is not loaded -> ignored
    subject->activatePlugin("ThisPluginDoesNotExist");
    QCOMPARE(subject->plugins.size(), 2);
    QVERIFY(subject->activePlugins.size() == 2);
    dummyImPluginFound = false;
    dummyImPlugin3Found = false;
    foreach(plugin, subject->plugins.keys()) {
        if (plugin->name() == "DummyImPlugin") {
            dummyImPluginFound = true;
        } else if (plugin->name() == "DummyImPlugin3") {
            dummyImPlugin3Found = true;
        }
    }
    QVERIFY(dummyImPluginFound);
    QVERIFY(dummyImPlugin3Found);
}


void Ut_MIMPluginManager::testAddHandlerMap()
{
    MInputMethodPlugin *plugin = 0;
    subject->activatePlugin(pluginName3);

    subject->addHandlerMap(OnScreen, pluginName);
    subject->addHandlerMap(Hardware, pluginName);
    subject->addHandlerMap(Accessory, pluginName3);

    plugin = subject->handlerToPlugin[OnScreen];
    QVERIFY(plugin != 0);
    QCOMPARE(plugin->name(), pluginName);

    plugin = subject->handlerToPlugin[Hardware];
    QVERIFY(plugin != 0);
    QCOMPARE(plugin->name(), pluginName);

    plugin = subject->handlerToPlugin[Accessory];
    QVERIFY(plugin != 0);
    QCOMPARE(plugin->name(), pluginName3);
}


void Ut_MIMPluginManager::testConvertAndFilterHandlers_data()
{
    QTest::addColumn<QStringList>("names");
    QTest::addColumn<HandlerStates>("expectedStates");

    for (int n = 0; n <= Accessory; ++n) {
        QTest::newRow("single state")
                << (QStringList() << QString::number(n))
                << (HandlerStates() << MIMHandlerState(n));
    }

    QTest::newRow("0 1")
            << (QStringList() << "0" << "1")
            << (HandlerStates() << Hardware);

    QTest::newRow("1 2")
            << (QStringList() << "1" << "2")
            << (HandlerStates() << Accessory << Hardware);

    QTest::newRow("1 1")
            << (QStringList() << "1" << "1")
            << (HandlerStates() << Hardware);
}


void Ut_MIMPluginManager::testConvertAndFilterHandlers()
{
    QFETCH(QStringList, names);
    QFETCH(HandlerStates, expectedStates);
    HandlerStates states;

    qDebug() << "Handlers' names:" << names << "expected result:" << expectedStates;

    subject->convertAndFilterHandlers(names, &states);
    QCOMPARE(states, expectedStates);
}


void Ut_MIMPluginManager::testSwitchPlugin()
{
    QSet<MIMHandlerState> actualState;
    DummyImPlugin  *plugin  = 0;
    DummyImPlugin3 *plugin3 = 0;
    MInputMethodBase *inputMethodBase = 0;
    DummyInputMethod  *inputMethod  = 0;
    DummyInputMethod3 *inputMethod3 = 0;

    subject->addHandlerMap(OnScreen, pluginName);
    subject->addHandlerMap(Hardware, pluginName);
    subject->addHandlerMap(Accessory, pluginName3);

    actualState << OnScreen;
    QVERIFY(subject->activePlugins.size() == 1);
    subject->setActiveHandlers(actualState);
    QCOMPARE(subject->activePlugins.size(), 1);
    plugin = dynamic_cast<DummyImPlugin *>(*subject->activePlugins.begin());
    QVERIFY(plugin != 0);
    QCOMPARE(plugin->name(), pluginName);
    inputMethodBase = subject->plugins[plugin];
    QVERIFY(inputMethodBase != 0);
    inputMethod = dynamic_cast<DummyInputMethod *>(inputMethodBase);
    QVERIFY(inputMethod != 0);
    QCOMPARE(inputMethod->setStateCount, 1);
    inputMethod->setStateCount = 0;
    QCOMPARE(inputMethod->setStateParam.size(), 1);
    QCOMPARE(inputMethod->setStateParam.first(), OnScreen);

    actualState.clear();
    actualState << Hardware;
    subject->setActiveHandlers(actualState);
    QCOMPARE(subject->activePlugins.size(), 1);
    plugin = dynamic_cast<DummyImPlugin *>(*subject->activePlugins.begin());
    QVERIFY(plugin != 0);
    QCOMPARE(plugin->name(), pluginName);
    inputMethodBase = subject->plugins[plugin];
    QVERIFY(inputMethodBase != 0);
    inputMethod = dynamic_cast<DummyInputMethod *>(inputMethodBase);
    QVERIFY(inputMethod != 0);
    QCOMPARE(inputMethod->setStateCount, 1);
    inputMethod->setStateCount = 0;
    QCOMPARE(inputMethod->setStateParam.size(), 1);
    QCOMPARE(inputMethod->setStateParam.first(), Hardware);

    actualState.clear();
    actualState << Accessory;
    subject->setActiveHandlers(actualState);
    QCOMPARE(subject->activePlugins.size(), 1);
    plugin3 = dynamic_cast<DummyImPlugin3 *>(*subject->activePlugins.begin());
    QVERIFY(plugin3 != 0);
    QCOMPARE(plugin3->name(), pluginName3);
    inputMethodBase = subject->plugins[*subject->activePlugins.begin()];
    QVERIFY(inputMethodBase != 0);
    inputMethod3 = dynamic_cast<DummyInputMethod3 *>(inputMethodBase);
    QVERIFY(inputMethod3 != 0);
    QCOMPARE(inputMethod3->setStateCount, 1);
    inputMethod3->setStateCount = 0;
    QCOMPARE(inputMethod3->setStateParam.size(), 1);
    QCOMPARE(inputMethod3->setStateParam.first(), Accessory);
}


void Ut_MIMPluginManager::testMultilePlugins()
{
    QSet<MIMHandlerState> actualState;
    DummyImPlugin  *plugin  = 0;
    DummyImPlugin3 *plugin3 = 0;
    int pluginCount = 0;
    int plugin3Count = 0;
    MInputMethodBase *inputMethodBase = 0;
    DummyInputMethod  *inputMethod  = 0;
    DummyInputMethod3 *inputMethod3 = 0;

    subject->addHandlerMap(OnScreen, pluginName);
    subject->addHandlerMap(Hardware, pluginName);
    subject->addHandlerMap(Accessory, pluginName3);

    actualState << Accessory << Hardware;
    subject->setActiveHandlers(actualState);
    QCOMPARE(subject->activePlugins.size(), 2);
    foreach(MInputMethodPlugin * p, subject->activePlugins) {
        plugin3 = dynamic_cast<DummyImPlugin3 *>(p);
        if (plugin3 != 0) {
            ++plugin3Count;
            QCOMPARE(plugin3->name(), pluginName3);
            inputMethodBase = subject->plugins[p];
            QVERIFY(inputMethodBase != 0);
            inputMethod3 = dynamic_cast<DummyInputMethod3 *>(inputMethodBase);
            QVERIFY(inputMethod3 != 0);
            QCOMPARE(inputMethod3->setStateCount, 1);
            inputMethod3->setStateCount = 0;
            QCOMPARE(inputMethod3->setStateParam.size(), 1);
            QCOMPARE(inputMethod3->setStateParam.first(), Accessory);
        }
        plugin = dynamic_cast<DummyImPlugin *>(p);
        if (plugin != 0) {
            ++pluginCount;
            QCOMPARE(plugin->name(), pluginName);
            inputMethodBase = subject->plugins[p];
            QVERIFY(inputMethodBase != 0);
            inputMethod = dynamic_cast<DummyInputMethod *>(inputMethodBase);
            QVERIFY(inputMethod != 0);
            QCOMPARE(inputMethod->setStateCount, 1);
            inputMethod->setStateCount = 0;
            QCOMPARE(inputMethod->setStateParam.size(), 1);
            QCOMPARE(inputMethod->setStateParam.first(), Hardware);
        }
    }
    QCOMPARE(pluginCount, 1);
    QCOMPARE(plugin3Count, 1);
}

void Ut_MIMPluginManager::testFreeInputMethod()
{
    QMap<MInputMethodPlugin *, MInputMethodBase *>::iterator iterator;
    QSet<MIMHandlerState> actualState;

    subject->addHandlerMap(OnScreen, pluginName);
    subject->addHandlerMap(Hardware, pluginName);
    subject->addHandlerMap(Accessory, pluginName3);

    actualState << OnScreen;
    subject->setActiveHandlers(actualState);
    actualState.clear();
    actualState << Accessory;
    subject->setActiveHandlers(actualState);

    subject->deleteInactiveIM();

    for (iterator = subject->plugins.begin(); iterator != subject->plugins.end(); ++iterator) {
        if (subject->activePlugins.contains(iterator.key())) {
            QVERIFY(iterator.value() != 0);
        } else {
            QVERIFY(iterator.value() == 0);
        }
    }
}

QTEST_APPLESS_MAIN(Ut_MIMPluginManager)
