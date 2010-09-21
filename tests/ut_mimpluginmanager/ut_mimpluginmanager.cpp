#include "ut_mimpluginmanager.h"
#include "mgconfitem_stub.h"
#include "minputcontextconnection_stub.h"
#include "dummyimplugin.h"
#include "dummyimplugin3.h"
#include "dummyinputmethod.h"
#include "dummyinputmethod3.h"
#include "fakegconf.h"

#include <QProcess>
#include <QGraphicsScene>
#include <QRegExp>
#include <QCoreApplication>
#include <QPointer>
#include <QDBusInterface>
#include <QDBusReply>
#include <mimpluginmanager.h>
#include <mimpluginmanager_p.h>
#include <minputmethodplugin.h>

#include "mtoolbarmanager.h"

typedef QSet<MIMHandlerState> HandlerStates;
Q_DECLARE_METATYPE(HandlerStates);
Q_DECLARE_METATYPE(MIMHandlerState);

namespace
{
    const QString GlobalTestPluginPath("/usr/lib/meego-im-framework-tests/plugins");
    const QString TestPluginPathEnvVariable("TESTPLUGIN_PATH");

    const QString ConfigRoot          = "/meegotouch/inputmethods/";
    const QString MImPluginPaths    = ConfigRoot + "paths";
    const QString MImPluginDisabled = ConfigRoot + "disabledpluginfiles";

    const QString PluginRoot          = "/meegotouch/inputmethods/plugins/";

    const QString pluginName  = "DummyImPlugin";
    const QString pluginName2 = "DummyImPlugin2";
    const QString pluginName3 = "DummyImPlugin3";

    const char * const DBusMIMPluginManagerServiceName = "com.maemo.inputmethodpluginmanager1";
    const char * const DBusMIMPluginManagerPath = "/com/maemo/inputmethodpluginmanager1";
    const char * const DBusMIMPluginManagerInterface = "com.maemo.inputmethodpluginmanager1";

    QString Toolbar1 = "/toolbar1.xml";
    QString Toolbar2 = "/toolbar2.xml";
}


void Ut_MIMPluginManager::initTestCase()
{
    static char *argv[1] = { (char *) "ut_mimpluginloader" };
    static int argc = 1;

    app = new QCoreApplication(argc, argv);

    Toolbar1 = QCoreApplication::applicationDirPath() + Toolbar1;
    QVERIFY2(QFile(Toolbar1).exists(), "toolbar1.xml does not exist");
    Toolbar2 = QCoreApplication::applicationDirPath() + Toolbar2;
    QVERIFY2(QFile(Toolbar2).exists(), "toolbar2.xml does not exist");

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
    MGConfItem pathConf(MImPluginPaths);
    pathConf.set(pluginPath);
    MGConfItem blackListConf(MImPluginDisabled);

    QStringList blackList;
    blackList << "libdummyimplugin2.so";
    //ignore the meego-keyboard
    blackList << "libmeego-keyboard.so";
    blackListConf.set(blackList);
    MGConfItem handlerItem(PluginRoot + "onscreen");
    handlerItem.set(pluginName);

    manager = new MIMPluginManager();
    subject = manager->d_ptr;

    QVERIFY(subject->activePlugins.size() == 1);
    MInputMethodPlugin *plugin = 0;
    plugin = *subject->activePlugins.begin();
    QVERIFY(plugin != 0);
    QCOMPARE(plugin->name(), pluginName);
    DummyInputMethod  *inputMethod  = 0;
    MInputMethodBase *inputMethodBase = 0;
    inputMethodBase = subject->plugins[plugin].inputMethod;
    QVERIFY(inputMethodBase != 0);
    inputMethod = dynamic_cast<DummyInputMethod *>(inputMethodBase);
    QVERIFY(inputMethod != 0);
    inputMethod->setStateCount = 0;

    // init dbus client
    if (!QDBusConnection::sessionBus().isConnected()) {
        QFAIL("Cannot connect to the DBus session bus");
    }
    QDBusConnection connection = QDBusConnection::sessionBus();
    m_clientInterface = new QDBusInterface(DBusMIMPluginManagerServiceName, DBusMIMPluginManagerPath,
                                           DBusMIMPluginManagerInterface, connection, 0);

    if (!m_clientInterface->isValid()) {
        QFAIL(qPrintable(QDBusConnection::sessionBus().lastError().message()));
    }
}

void Ut_MIMPluginManager::cleanup()
{
    delete m_clientInterface;
    m_clientInterface = 0;
    delete manager;
    manager = 0;
    subject = 0;
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
}


void Ut_MIMPluginManager::testAddHandlerMap()
{
    MInputMethodPlugin *plugin = 0;

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


void Ut_MIMPluginManager::testSwitchPluginState()
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
    inputMethodBase = subject->plugins[plugin].inputMethod;
    QVERIFY(inputMethodBase != 0);
    inputMethod = dynamic_cast<DummyInputMethod *>(inputMethodBase);
    QVERIFY(inputMethod != 0);
    QCOMPARE(inputMethod->setStateCount, 1);
    inputMethod->setStateCount = 0;
    QCOMPARE(inputMethod->setStateParam.size(), 1);
    QCOMPARE(*inputMethod->setStateParam.begin(), OnScreen);

    actualState.clear();
    actualState << Hardware;
    subject->setActiveHandlers(actualState);
    QCOMPARE(subject->activePlugins.size(), 1);
    plugin = dynamic_cast<DummyImPlugin *>(*subject->activePlugins.begin());
    QVERIFY(plugin != 0);
    QCOMPARE(plugin->name(), pluginName);
    inputMethodBase = subject->plugins[plugin].inputMethod;
    QVERIFY(inputMethodBase != 0);
    inputMethod = dynamic_cast<DummyInputMethod *>(inputMethodBase);
    QVERIFY(inputMethod != 0);
    QCOMPARE(inputMethod->setStateCount, 1);
    inputMethod->setStateCount = 0;
    QCOMPARE(inputMethod->setStateParam.size(), 1);
    QCOMPARE(*inputMethod->setStateParam.begin(), Hardware);

    actualState.clear();
    actualState << Accessory;
    subject->setActiveHandlers(actualState);
    QCOMPARE(subject->activePlugins.size(), 1);
    plugin3 = dynamic_cast<DummyImPlugin3 *>(*subject->activePlugins.begin());
    QVERIFY(plugin3 != 0);
    QCOMPARE(plugin3->name(), pluginName3);
    inputMethodBase = subject->plugins[plugin3].inputMethod;
    QVERIFY(inputMethodBase != 0);
    inputMethod3 = dynamic_cast<DummyInputMethod3 *>(inputMethodBase);
    QVERIFY(inputMethod3 != 0);
    QCOMPARE(inputMethod3->setStateCount, 1);
    inputMethod3->setStateCount = 0;
    QCOMPARE(inputMethod3->setStateParam.size(), 1);
    QCOMPARE(*inputMethod3->setStateParam.begin(), Accessory);
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
            inputMethodBase = subject->plugins[p].inputMethod;
            QVERIFY(inputMethodBase != 0);
            inputMethod3 = dynamic_cast<DummyInputMethod3 *>(inputMethodBase);
            QVERIFY(inputMethod3 != 0);
            QCOMPARE(inputMethod3->setStateCount, 1);
            inputMethod3->setStateCount = 0;
            QCOMPARE(inputMethod3->setStateParam.size(), 1);
            QCOMPARE(*inputMethod3->setStateParam.begin(), Accessory);
        }
        plugin = dynamic_cast<DummyImPlugin *>(p);
        if (plugin != 0) {
            ++pluginCount;
            QCOMPARE(plugin->name(), pluginName);
            inputMethodBase = subject->plugins[p].inputMethod;
            QVERIFY(inputMethodBase != 0);
            inputMethod = dynamic_cast<DummyInputMethod *>(inputMethodBase);
            QVERIFY(inputMethod != 0);
            QCOMPARE(inputMethod->setStateCount, 1);
            inputMethod->setStateCount = 0;
            QCOMPARE(inputMethod->setStateParam.size(), 1);
            QCOMPARE(*inputMethod->setStateParam.begin(), Hardware);
        }
    }
    QCOMPARE(pluginCount, 1);
    QCOMPARE(plugin3Count, 1);
}

void Ut_MIMPluginManager::testExistInputMethod()
{
    MIMPluginManagerPrivate::Plugins::iterator iterator;
    QSet<MIMHandlerState> actualState;

    subject->addHandlerMap(OnScreen, pluginName);
    subject->addHandlerMap(Hardware, pluginName);
    subject->addHandlerMap(Accessory, pluginName3);

    actualState << OnScreen;
    subject->setActiveHandlers(actualState);
    actualState.clear();
    actualState << Accessory;
    subject->setActiveHandlers(actualState);

    for (iterator = subject->plugins.begin(); iterator != subject->plugins.end(); ++iterator) {
        // no matter the plugin is active or not, the inputmethodbase object is not empty.
        if (!iterator.key()->supportedStates().isEmpty()) {
            QVERIFY(iterator.value().inputMethod != 0);
        }
    }
}

void Ut_MIMPluginManager::testPluginSwitcher_data()
{
    QTest::addColumn<MIMHandlerState>("state");

    QTest::newRow("OnScreen")  << OnScreen;
    QTest::newRow("Hardware")  << Hardware;
    QTest::newRow("Accessory") << Accessory;
}

void Ut_MIMPluginManager::testPluginSwitcher()
{
    QFETCH(MIMHandlerState, state);
    QSet<MIMHandlerState> actualState;
    DummyImPlugin  *plugin  = 0;
    DummyImPlugin3 *plugin3 = 0;
    MInputMethodBase *inputMethodBase = 0;
    QPointer<DummyInputMethod > inputMethod  = 0;
    QPointer<DummyInputMethod3> inputMethod3 = 0;

    subject->addHandlerMap(OnScreen, pluginName);
    subject->addHandlerMap(Hardware, pluginName);
    subject->addHandlerMap(Accessory, pluginName);

    // find for loaded plugins
    for (MIMPluginManagerPrivate::Plugins::iterator iterator(subject->plugins.begin());
         iterator != subject->plugins.end();
         ++iterator) {
        if (iterator.key() == 0) {
            continue;
        }
        if (pluginName == iterator.key()->name()) {
            plugin  = dynamic_cast<DummyImPlugin  *>(iterator.key());
        } else if (pluginName3 == iterator.key()->name()) {
            plugin3 = dynamic_cast<DummyImPlugin3 *>(iterator.key());
        }
    }

    QVERIFY(plugin  != 0);
    QVERIFY(plugin3 != 0);
    QVERIFY(subject->plugins.contains(plugin));
    inputMethod  = dynamic_cast<DummyInputMethod *>(subject->plugins[plugin].inputMethod);

    actualState << state;
    QVERIFY(subject->activePlugins.size() == 1);
    subject->setActiveHandlers(actualState);

    // nothing should be changed
    subject->switchPlugin(M::SwitchUndefined, inputMethod);
    QVERIFY(inputMethod != 0);
    QCOMPARE(inputMethod->switchContextCallCount, 0);
    QCOMPARE(subject->plugins[plugin].lastSwitchDirection, M::SwitchUndefined);
    QCOMPARE(subject->activePlugins.count(), 1);
    QVERIFY(plugin == *subject->activePlugins.begin());

    // switch forward
    subject->switchPlugin(M::SwitchForward, inputMethod);
    QCOMPARE(subject->plugins[plugin].lastSwitchDirection, M::SwitchForward);

    QCOMPARE(subject->activePlugins.count(), 1);
    QVERIFY(plugin3 == *subject->activePlugins.begin());
    inputMethodBase = subject->plugins[plugin3].inputMethod;
    QVERIFY(inputMethodBase != 0);
    inputMethod3 = dynamic_cast<DummyInputMethod3 *>(inputMethodBase);
    QVERIFY(inputMethod3 != 0);
    QCOMPARE(inputMethod3->switchContextCallCount, 0);
    QCOMPARE(inputMethod3->setStateCount, 1);
    inputMethod3->setStateCount = 0;
    QCOMPARE(inputMethod3->setStateParam.size(), 1);
    QCOMPARE(*inputMethod3->setStateParam.begin(), state);
    checkHandlerMap(state, pluginName3);

    //switch backward
    inputMethod->setStateCount = 0;
    subject->switchPlugin(M::SwitchBackward, inputMethod3);
    QCOMPARE(subject->plugins[plugin3].lastSwitchDirection, M::SwitchBackward);

    QCOMPARE(subject->activePlugins.count(), 1);
    QVERIFY(plugin == *subject->activePlugins.begin());
    inputMethodBase = subject->plugins[plugin].inputMethod;
    QVERIFY(inputMethodBase != 0);
    inputMethod = dynamic_cast<DummyInputMethod *>(inputMethodBase);
    QVERIFY(inputMethod != 0);
    QCOMPARE(inputMethod->switchContextCallCount, 0);
    QCOMPARE(inputMethod->setStateCount, 1);
    inputMethod->setStateCount = 0;
    QCOMPARE(inputMethod->setStateParam.size(), 1);
    QCOMPARE(*inputMethod->setStateParam.begin(), state);
    checkHandlerMap(state, pluginName);

    // ... again
    subject->switchPlugin(M::SwitchBackward, inputMethod);
    QCOMPARE(subject->plugins[plugin].lastSwitchDirection, M::SwitchBackward);

    QCOMPARE(subject->activePlugins.count(), 1);
    QVERIFY(plugin3 == *subject->activePlugins.begin());
    inputMethodBase = subject->plugins[plugin3].inputMethod;
    QVERIFY(inputMethodBase != 0);
    inputMethod3 = dynamic_cast<DummyInputMethod3 *>(inputMethodBase);
    QVERIFY(inputMethod3 != 0);
    QCOMPARE(inputMethod3->switchContextCallCount, 1);
    inputMethod3->switchContextCallCount = 0;
    QCOMPARE(inputMethod3->directionParam, M::SwitchBackward);
    QVERIFY(inputMethod3->enableAnimationParam == false);
    QCOMPARE(inputMethod3->setStateCount, 1);
    inputMethod3->setStateCount = 0;
    QCOMPARE(inputMethod3->setStateParam.size(), 1);
    QCOMPARE(*inputMethod3->setStateParam.begin(), state);
    checkHandlerMap(state, pluginName3);

    // try to switch to plugin which could not support the same state
    // nothing should be changed
    plugin->allowedStates.clear();
    subject->switchPlugin(M::SwitchBackward, inputMethod3);
    plugin->allowedStates << OnScreen << Hardware << Accessory; // restore default configuration
    QCOMPARE(subject->plugins[plugin3].lastSwitchDirection, M::SwitchBackward);

    QCOMPARE(subject->activePlugins.count(), 1);
    QVERIFY(plugin3 == *subject->activePlugins.begin());
    QVERIFY(inputMethod3 != 0);
    inputMethodBase = inputMethod3;
    QVERIFY(inputMethodBase == subject->plugins[plugin3].inputMethod);
    QCOMPARE(inputMethod3->switchContextCallCount, 0);
    QCOMPARE(inputMethod3->setStateCount, 0);
    inputMethod3->setStateCount = 0;
    checkHandlerMap(state, pluginName3);

    //test toolbar status when switch plugin
    MToolbarId toolbarId1(1, "toolbarIdTest1");
    MToolbarManager::instance().registerToolbar(toolbarId1, Toolbar1);
    QSharedPointer<const MToolbarData> toolbarData1 =
        MToolbarManager::instance().toolbarData(toolbarId1);
    MToolbarId toolbarId2(2, "toolbarIdTest2");
    MToolbarManager::instance().registerToolbar(toolbarId2, Toolbar2);
    QSharedPointer<const MToolbarData> toolbarData2 =
        MToolbarManager::instance().toolbarData(toolbarId2);

    QVERIFY(toolbarData1.data());
    QVERIFY(toolbarData2.data());
    QVERIFY(toolbarData1.data() != toolbarData2.data());

    manager->setToolbar(toolbarId1);
    subject->setActivePlugin(plugin->name(), OnScreen);
    subject->switchPlugin(M::SwitchForward, inputMethod);
    QVERIFY(inputMethod->toolbarParam == toolbarData1);
    QVERIFY(inputMethod3->toolbarParam == toolbarData1);

    manager->setToolbar(toolbarId2);
    subject->setActivePlugin(plugin3->name(), OnScreen);
    subject->switchPlugin(M::SwitchBackward, inputMethod3);
    QVERIFY(inputMethod->toolbarParam == toolbarData2);
    QVERIFY(inputMethod3->toolbarParam == toolbarData2);

    MToolbarManager::instance().unregisterToolbar(toolbarId1);
    MToolbarManager::instance().unregisterToolbar(toolbarId2);
}

void Ut_MIMPluginManager::checkHandlerMap(int handler, const QString &name)
{
    const QString key = QString(PluginRoot + subject->inputSourceName(static_cast<MIMHandlerState>(handler)));
    MGConfItem gconf(key);
    QCOMPARE(gconf.value().toString(), name);
}

void Ut_MIMPluginManager::testSwitchToSpecifiedPlugin()
{
    const MIMHandlerState state = OnScreen;
    QSet<MIMHandlerState> actualState;
    DummyImPlugin  *plugin  = 0;
    DummyImPlugin3 *plugin3 = 0;
    MInputMethodBase *inputMethodBase = 0;
    QPointer<DummyInputMethod > inputMethod  = 0;
    QPointer<DummyInputMethod3> inputMethod3 = 0;

    subject->addHandlerMap(OnScreen, pluginName);
    subject->addHandlerMap(Hardware, pluginName);
    subject->addHandlerMap(Accessory, pluginName);

    // find for loaded plugins
    for (MIMPluginManagerPrivate::Plugins::iterator iterator(subject->plugins.begin());
         iterator != subject->plugins.end();
         ++iterator) {
        if (pluginName == iterator.key()->name()) {
            plugin  = dynamic_cast<DummyImPlugin  *>(iterator.key());
        } else if (pluginName3 == iterator.key()->name()) {
            plugin3 = dynamic_cast<DummyImPlugin3 *>(iterator.key());
        }
    }

    QVERIFY(plugin  != 0);
    QVERIFY(plugin3 != 0);
    inputMethod  = dynamic_cast<DummyInputMethod  *>(subject->plugins[plugin].inputMethod);

    actualState << state;
    QVERIFY(subject->activePlugins.size() == 1);
    subject->setActiveHandlers(actualState);

    // nothing should be changed
    subject->switchPlugin(pluginName, inputMethod);
    QVERIFY(inputMethod != 0);
    QCOMPARE(inputMethod->switchContextCallCount, 0);
    QCOMPARE(subject->plugins[plugin].lastSwitchDirection, M::SwitchUndefined);
    QCOMPARE(subject->activePlugins.count(), 1);
    QVERIFY(plugin == *subject->activePlugins.begin());
    foreach (MInputMethodPlugin *handler, subject->handlerToPlugin.values()) {
        QVERIFY(handler == plugin);
    }

    // switch to another plugin
    subject->switchPlugin(pluginName3, inputMethod);
    QCOMPARE(subject->plugins[plugin].lastSwitchDirection, M::SwitchUndefined);
    QVERIFY(inputMethod != 0);

    QCOMPARE(subject->activePlugins.count(), 1);
    QVERIFY(plugin3 == *subject->activePlugins.begin());
    inputMethodBase = subject->plugins[plugin3].inputMethod;
    QVERIFY(inputMethodBase != 0);
    inputMethod3 = dynamic_cast<DummyInputMethod3 *>(inputMethodBase);
    QVERIFY(inputMethod3 != 0);
    QCOMPARE(inputMethod3->switchContextCallCount, 1);
    QCOMPARE(inputMethod3->directionParam, M::SwitchUndefined);
    QCOMPARE(inputMethod3->setStateCount, 1);
    inputMethod3->setStateCount = 0;
    QCOMPARE(inputMethod3->setStateParam.size(), 1);
    QCOMPARE(*inputMethod3->setStateParam.begin(), state);
    foreach (MInputMethodPlugin *handler, subject->handlerToPlugin.values()) {
        QVERIFY(handler == plugin3);
    }
}

void Ut_MIMPluginManager::testSetActivePlugin()
{
    QVERIFY(subject->activePlugins.size() == 1);
    MInputMethodPlugin *plugin = 0;
    plugin = *subject->activePlugins.begin();
    QVERIFY(plugin != 0);
    QCOMPARE(plugin->name(), pluginName);

    subject->setActivePlugin(pluginName3, OnScreen);

    // check gconf item
    MGConfItem handlerItem(PluginRoot + "onscreen");
    QCOMPARE(handlerItem.value().toString(), pluginName3);

    QVERIFY(subject->activePlugins.size() == 1);
    plugin = *subject->activePlugins.begin();
    QVERIFY(plugin != 0);
    QCOMPARE(plugin->name(), pluginName3);
}

void Ut_MIMPluginManager::testSubViews()
{
    QList<MInputMethodBase::MInputMethodSubView> subViews;
    foreach (MInputMethodPlugin *plugin, subject->plugins.keys()) {
        subViews += subject->plugins[plugin].inputMethod->subViews(OnScreen);
    }
    // only has subviews for OnScreen
    QCOMPARE(subViews.count(), 4);

    subViews.clear();
    foreach (MInputMethodPlugin *plugin, subject->plugins.keys()) {
        subViews += subject->plugins[plugin].inputMethod->subViews(Hardware);
    }
    // doesn't have subviews for Hardware
    QCOMPARE(subViews.count(), 0);

}

void Ut_MIMPluginManager::testActiveSubView()
{
    QVERIFY(subject->activePlugins.size() == 1);
    MInputMethodPlugin *plugin = 0;
    plugin = *subject->activePlugins.begin();
    QVERIFY(plugin != 0);
    QCOMPARE(plugin->name(), pluginName);
    QCOMPARE(subject->activeSubView(OnScreen), QString("dummyimsv1"));
    subject->_q_setActiveSubView(QString("dummyimsv2"), OnScreen);
    QCOMPARE(subject->activeSubView(OnScreen), QString("dummyimsv2"));

    subject->setActivePlugin(pluginName3, OnScreen);
    QVERIFY(subject->activePlugins.size() == 1);
    plugin = *subject->activePlugins.begin();
    QVERIFY(plugin != 0);
    QCOMPARE(plugin->name(), pluginName3);
    QCOMPARE(subject->activeSubView(OnScreen), QString("dummyim3sv1"));

    subject->_q_setActiveSubView(QString("dummyim3sv2"), OnScreen);
    QCOMPARE(subject->activeSubView(OnScreen), QString("dummyim3sv2"));
}

void Ut_MIMPluginManager::testDBusQueryCalls()
{
    if (!manager->isDBusConnectionValid()) {
        QSKIP("MIMPluginManager dbus connection is not valid. Possibly other program using it running.",
              SkipSingle);
    }
    QVERIFY(subject->activePlugins.size() == 1);
    MInputMethodPlugin *plugin = 0;
    plugin = *subject->activePlugins.begin();
    QVERIFY(plugin != 0);
    QCOMPARE(plugin->name(), pluginName);
    QCOMPARE(subject->activeSubView(OnScreen), QString("dummyimsv1"));

    QVERIFY(m_clientInterface);
    QDBusReply<QStringList> AvailablePluginReply = m_clientInterface->call("queryAvailablePlugins", OnScreen);
    QVERIFY(AvailablePluginReply.isValid());
    QCOMPARE(AvailablePluginReply.value().count(), 2);
    QVERIFY(AvailablePluginReply.value().contains(pluginName));
    QVERIFY(AvailablePluginReply.value().contains(pluginName3));

    QDBusReply<QString> ActivePluginReply = m_clientInterface->call("queryActivePlugin", OnScreen);
    QVERIFY(ActivePluginReply.isValid());
    QCOMPARE(ActivePluginReply.value(), pluginName);

    QDBusReply< QMap<QString, QVariant> > activeSubViewReply = m_clientInterface->call("queryActiveSubView",
                                                                                       OnScreen);
    QVERIFY(activeSubViewReply.isValid());
    QVERIFY(activeSubViewReply.value().count() > 0);
    QCOMPARE(activeSubViewReply.value().values().at(0).toString(), pluginName);
    QCOMPARE(activeSubViewReply.value().keys().at(0), QString("dummyimsv1"));

    QDBusReply< QMap<QString, QVariant> > subViewsReply = m_clientInterface->call("queryAvailableSubViews",
                                                                                  pluginName,
                                                                                  OnScreen);
    QVERIFY(subViewsReply.isValid());
    QCOMPARE(subViewsReply.value().count(), 2);
}

void Ut_MIMPluginManager::testDBusSetCalls()
{
    if (!manager->isDBusConnectionValid()) {
        QSKIP("MIMPluginManager dbus connection is not valid. Possibly other program using it running.",
              SkipSingle);
    }
    QVERIFY(subject->activePlugins.size() == 1);
    MInputMethodPlugin *plugin = 0;
    plugin = *subject->activePlugins.begin();
    QVERIFY(plugin != 0);
    QCOMPARE(plugin->name(), pluginName);
    QCOMPARE(subject->activeSubView(OnScreen), QString("dummyimsv1"));

    QVERIFY(m_clientInterface);

    m_clientInterface->call(QDBus::NoBlock, "setActivePlugin", pluginName3, static_cast<int>(OnScreen));
    handleMessages();

    QVERIFY(subject->activePlugins.size() == 1);
    plugin = *subject->activePlugins.begin();
    QVERIFY(plugin != 0);
    QCOMPARE(plugin->name(), pluginName3);
    QCOMPARE(subject->activeSubView(OnScreen), QString("dummyim3sv1"));

    // try to set a wrong plugin
    m_clientInterface->call(QDBus::NoBlock, "setActivePlugin", pluginName2, static_cast<int>(OnScreen));
    handleMessages();
    QVERIFY(subject->activePlugins.size() == 1);
    plugin = *subject->activePlugins.begin();
    QVERIFY(plugin != 0);
    QCOMPARE(plugin->name(), pluginName3);

    m_clientInterface->call(QDBus::NoBlock, "setActiveSubView", QString("dummyim3sv2"), static_cast<int>(OnScreen));
    handleMessages();
    QCOMPARE(subject->activeSubView(OnScreen), QString("dummyim3sv2"));

    // try to set a wrong subview
    m_clientInterface->call(QDBus::NoBlock, "setActiveSubView", QString("dummyimisv2"), static_cast<int>(OnScreen));
    handleMessages();
    QCOMPARE(subject->activeSubView(OnScreen), QString("dummyim3sv2"));

    // try to set both subview id and plugin
    m_clientInterface->call(QDBus::NoBlock, "setActivePlugin", pluginName, static_cast<int>(OnScreen),
                            QString("dummyimsv1"));
    handleMessages();
    QVERIFY(subject->activePlugins.size() == 1);
    plugin = *subject->activePlugins.begin();
    QVERIFY(plugin != 0);
    QCOMPARE(plugin->name(), pluginName);
    QCOMPARE(subject->activeSubView(OnScreen), QString("dummyimsv1"));
}

void Ut_MIMPluginManager::testRegionUpdates()
{
    MInputMethodPlugin *plugin3 = 0;
    QSignalSpy regionUpdates(manager, SIGNAL(regionUpdated(QRegion)));
    QList<QVariant> regionUpdatesSignal;
    QVariant region;

    foreach(MInputMethodPlugin * plugin, subject->plugins.keys()) {
        if (plugin->name() == "DummyImPlugin3") {
            plugin3 = plugin;
        }
    }

    QVERIFY(plugin3);
    QCOMPARE(regionUpdates.count(), 0);

    // DummyImPlugin3 sends a non-empty region when calling show() on it:
    subject->activatePlugin(plugin3);
    manager->showActivePlugins();
    QCOMPARE(regionUpdates.count(), 1);

    region = regionUpdates.takeFirst().at(0);
    QVERIFY(!region.value<QRegion>().isEmpty());

    // Now make sure that hiding the plugin also sends an empty region update:
    manager->hideActivePlugins();
    QCOMPARE(regionUpdates.count(), 1);

    region = regionUpdates.takeFirst().at(0);
    QVERIFY(region.value<QRegion>().isEmpty());
}

void Ut_MIMPluginManager::testSetToolbar()
{
    MInputMethodPlugin *plugin1 = 0;
    foreach(MInputMethodPlugin * plugin, subject->plugins.keys()) {
        if (plugin->name() == "DummyImPlugin") {
            plugin1 = plugin;
        }
    }
    QVERIFY(plugin1);

    QPointer<DummyInputMethod > inputMethod  = 0;
    inputMethod  = dynamic_cast<DummyInputMethod  *>(subject->plugins[plugin1].inputMethod);
    QVERIFY(inputMethod);

    MToolbarId toolbarId1(1, "toolbarIdTest1");
    MToolbarManager::instance().registerToolbar(toolbarId1, Toolbar1);
    QSharedPointer<const MToolbarData> toolbarData1 =
        MToolbarManager::instance().toolbarData(toolbarId1);
    MToolbarId toolbarId2(2, "toolbarIdTest2");
    MToolbarManager::instance().registerToolbar(toolbarId2, Toolbar2);
    QSharedPointer<const MToolbarData> toolbarData2 =
        MToolbarManager::instance().toolbarData(toolbarId2);

    QVERIFY(toolbarData1.data());
    QVERIFY(toolbarData2.data());
    QVERIFY(toolbarData1.data() != toolbarData2.data());

    subject->setActivePlugin(plugin1->name(), OnScreen);
    manager->setToolbar(toolbarId1);
    QVERIFY(inputMethod->toolbarParam == toolbarData1);
    manager->setToolbar(toolbarId2);
    QVERIFY(inputMethod->toolbarParam == toolbarData2);

    MToolbarManager::instance().unregisterToolbar(toolbarId1);
    MToolbarManager::instance().unregisterToolbar(toolbarId2);
}

void Ut_MIMPluginManager::handleMessages()
{
    while (app->hasPendingEvents()) {
        app->processEvents();
    }
}

QTEST_APPLESS_MAIN(Ut_MIMPluginManager)
