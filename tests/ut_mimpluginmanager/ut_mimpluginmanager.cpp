#include "ut_mimpluginmanager.h"

#ifdef Q_WS_X11
#include "mimxapplication.h"
#endif

#include "mimsettings_stub.h"
#include "dummyimplugin.h"
#include "dummyimplugin3.h"
#include "dummyinputmethod.h"
#include "dummyinputmethod3.h"
#include "fakegconf.h"
#include "minputcontextconnection.h"

#include "utils.h"

#include <QProcess>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QWidget>
#include <QRegExp>
#include <QCoreApplication>
#include <QPointer>
#include <QDBusInterface>
#include <QDBusReply>
#include <QTimer>
#include <QEventLoop>
#include <mimpluginmanager.h>
#include <mimpluginmanager_p.h>
#include <minputmethodplugin.h>

#include "mattributeextensionmanager.h"

using namespace std::tr1;

typedef QSet<MInputMethod::HandlerState> HandlerStates;
Q_DECLARE_METATYPE(HandlerStates);
Q_DECLARE_METATYPE(MInputMethod::HandlerState);

namespace {
    const QString ConfigRoot          = MALIIT_CONFIG_ROOT;
    const QString MImPluginPaths    = ConfigRoot + "paths";
    const QString MImPluginDisabled = ConfigRoot + "disabledpluginfiles";

    const QString PluginRoot          = MALIIT_CONFIG_ROOT"plugins/";

    const QString EnabledPluginsKey = MALIIT_CONFIG_ROOT"onscreen/enabled";
    const QString ActivePluginKey =   MALIIT_CONFIG_ROOT"onscreen/active";

    const QString pluginName  = "DummyImPlugin";
    const QString pluginName2 = "DummyImPlugin2";
    const QString pluginName3 = "DummyImPlugin3";
    const QString pluginId  = "libdummyimplugin.so";
    const QString pluginId2 = "libdummyimplugin2.so";
    const QString pluginId3 = "libdummyimplugin3.so";

    const char * const DBusMIMPluginManagerServiceName = "com.meego.inputmethodpluginmanager1";
    const char * const DBusMIMPluginManagerPath = "/com/meego/inputmethodpluginmanager1";
    const char * const DBusMIMPluginManagerInterface = "com.meego.inputmethodpluginmanager1";

    const QString testDirectory = "/ut_mimpluginmanager";
    QString Toolbar1 = "/toolbar1.xml";
    QString Toolbar2 = "/toolbar2.xml";

    const QStringList DefaultEnabledPlugins = QStringList()
                                              << pluginId << "dummyimsv1"
                                              << pluginId << "dummyimsv2"
                                              << pluginId3 << "dummyim3sv1"
                                              << pluginId3 << "dummyim3sv2";
}


void Ut_MIMPluginManager::initTestCase()
{
    static char *argv[1] = { (char *) "ut_mimpluginloader" };
    static int argc = 1;

#ifdef Q_WS_X11
    app = new MImXApplication(argc, argv);
    proxyWidget = static_cast<MImXApplication*>(app)->pluginsProxyWidget();
#else
    app = new QApplication(argc, argv);
    proxyWidget = new QWidget;
#endif

    Toolbar1 = MaliitTestUtils::getTestDataPath() + testDirectory + Toolbar1;
    QVERIFY2(QFile(Toolbar1).exists(), "toolbar1.xml does not exist");
    Toolbar2 = MaliitTestUtils::getTestDataPath() + testDirectory + Toolbar2;
    QVERIFY2(QFile(Toolbar2).exists(), "toolbar2.xml does not exist");
}

void Ut_MIMPluginManager::cleanupTestCase()
{
    delete app;
}

void Ut_MIMPluginManager::init()
{
    MImSettings pathConf(MImPluginPaths);
    pathConf.set(MaliitTestUtils::getTestPluginPath());
    MImSettings blackListConf(MImPluginDisabled);

    QStringList blackList;
    blackList << "libdummyimplugin2.so";
    //ignore the meego-keyboard
    blackList << "libmeego-keyboard.so";
    blackListConf.set(blackList);

    MImSettings enabledPluginsSettings(EnabledPluginsKey);
    enabledPluginsSettings.set(DefaultEnabledPlugins);

    MImSettings activePluginSettings(ActivePluginKey);
    QStringList activePlugin;
    activePlugin << pluginId << "dummyimsv1";
    activePluginSettings.set(activePlugin);

    shared_ptr<MInputContextConnection> icConnection(new MInputContextConnection);
    manager = new MIMPluginManager(icConnection, proxyWidget);

    subject = manager->d_ptr;

    QVERIFY(subject->activePlugins.size() == 1);
    MInputMethodPlugin *plugin = 0;
    plugin = *subject->activePlugins.begin();
    QVERIFY(plugin != 0);
    QCOMPARE(plugin->name(), pluginName);
    DummyInputMethod  *inputMethod  = 0;
    MAbstractInputMethod *abstractInputMethod = 0;
    abstractInputMethod = subject->plugins[plugin].inputMethod;
    QVERIFY(abstractInputMethod != 0);
    inputMethod = dynamic_cast<DummyInputMethod *>(abstractInputMethod);
    QVERIFY(inputMethod != 0);
    QCOMPARE(inputMethod->pluginsChangedSignalCount, 1);
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
    Q_FOREACH(MInputMethodPlugin * plugin, subject->plugins.keys()) {
        qDebug() << plugin->name();
    }
    QCOMPARE(subject->plugins.size(), 2);
    QCOMPARE(subject->activePlugins.size(), 1);
    plugin = *subject->activePlugins.begin();
    QCOMPARE(plugin->name(), pluginName);
    bool dummyImPluginFound = false;
    bool dummyImPlugin3Found = false;
    Q_FOREACH(MInputMethodPlugin * plugin, subject->plugins.keys()) {
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
    Q_FOREACH(plugin, subject->plugins.keys()) {
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

    subject->addHandlerMap(MInputMethod::OnScreen, pluginId);
    subject->addHandlerMap(MInputMethod::Hardware, pluginId);
    subject->addHandlerMap(MInputMethod::Accessory, pluginId3);

    plugin = subject->handlerToPlugin[MInputMethod::OnScreen];
    QVERIFY(plugin != 0);
    QCOMPARE(plugin->name(), pluginName);

    plugin = subject->handlerToPlugin[MInputMethod::Hardware];
    QVERIFY(plugin != 0);
    QCOMPARE(plugin->name(), pluginName);

    plugin = subject->handlerToPlugin[MInputMethod::Accessory];
    QVERIFY(plugin != 0);
    QCOMPARE(plugin->name(), pluginName3);
}

void Ut_MIMPluginManager::testSwitchPluginState()
{
    QSet<MInputMethod::HandlerState> actualState;
    DummyImPlugin  *plugin  = 0;
    DummyImPlugin3 *plugin3 = 0;
    MAbstractInputMethod *abstractInputMethod = 0;
    DummyInputMethod  *inputMethod  = 0;
    DummyInputMethod3 *inputMethod3 = 0;

    subject->addHandlerMap(MInputMethod::OnScreen, pluginId);
    subject->addHandlerMap(MInputMethod::Hardware, pluginId);
    subject->addHandlerMap(MInputMethod::Accessory, pluginId3);

    actualState << MInputMethod::OnScreen;
    QVERIFY(subject->activePlugins.size() == 1);
    subject->setActiveHandlers(actualState);
    QCOMPARE(subject->activePlugins.size(), 1);
    plugin = dynamic_cast<DummyImPlugin *>(*subject->activePlugins.begin());
    QVERIFY(plugin != 0);
    QCOMPARE(plugin->name(), pluginName);
    abstractInputMethod = subject->plugins[plugin].inputMethod;
    QVERIFY(abstractInputMethod != 0);
    inputMethod = dynamic_cast<DummyInputMethod *>(abstractInputMethod);
    QVERIFY(inputMethod != 0);
    QCOMPARE(inputMethod->setStateCount, 1);
    inputMethod->setStateCount = 0;
    QCOMPARE(inputMethod->setStateParam.size(), 1);
    QCOMPARE(*inputMethod->setStateParam.begin(), MInputMethod::OnScreen);

    actualState.clear();
    actualState << MInputMethod::Hardware;
    subject->setActiveHandlers(actualState);
    QCOMPARE(subject->activePlugins.size(), 1);
    plugin = dynamic_cast<DummyImPlugin *>(*subject->activePlugins.begin());
    QVERIFY(plugin != 0);
    QCOMPARE(plugin->name(), pluginName);
    abstractInputMethod = subject->plugins[plugin].inputMethod;
    QVERIFY(abstractInputMethod != 0);
    inputMethod = dynamic_cast<DummyInputMethod *>(abstractInputMethod);
    QVERIFY(inputMethod != 0);
    QCOMPARE(inputMethod->setStateCount, 1);
    inputMethod->setStateCount = 0;
    QCOMPARE(inputMethod->setStateParam.size(), 1);
    QCOMPARE(*inputMethod->setStateParam.begin(), MInputMethod::Hardware);

    actualState.clear();
    actualState << MInputMethod::Accessory;
    subject->setActiveHandlers(actualState);
    QCOMPARE(subject->activePlugins.size(), 1);
    plugin3 = dynamic_cast<DummyImPlugin3 *>(*subject->activePlugins.begin());
    QVERIFY(plugin3 != 0);
    QCOMPARE(plugin3->name(), pluginName3);
    abstractInputMethod = subject->plugins[plugin3].inputMethod;
    QVERIFY(abstractInputMethod != 0);
    inputMethod3 = dynamic_cast<DummyInputMethod3 *>(abstractInputMethod);
    QVERIFY(inputMethod3 != 0);
    QCOMPARE(inputMethod3->setStateCount, 1);
    inputMethod3->setStateCount = 0;
    QCOMPARE(inputMethod3->setStateParam.size(), 1);
    QCOMPARE(*inputMethod3->setStateParam.begin(), MInputMethod::Accessory);
}


void Ut_MIMPluginManager::testMultiplePlugins()
{
    QSet<MInputMethod::HandlerState> actualState;
    DummyImPlugin  *plugin  = 0;
    DummyImPlugin3 *plugin3 = 0;
    int pluginCount = 0;
    int plugin3Count = 0;
    MAbstractInputMethod *abstractInputMethod = 0;
    DummyInputMethod  *inputMethod  = 0;
    DummyInputMethod3 *inputMethod3 = 0;

    subject->addHandlerMap(MInputMethod::OnScreen, pluginId);
    subject->addHandlerMap(MInputMethod::Hardware, pluginId);
    subject->addHandlerMap(MInputMethod::Accessory, pluginId3);

    actualState << MInputMethod::Accessory << MInputMethod::Hardware;
    subject->setActiveHandlers(actualState);
    QCOMPARE(subject->activePlugins.size(), 2);
    Q_FOREACH(MInputMethodPlugin * p, subject->activePlugins) {
        plugin3 = dynamic_cast<DummyImPlugin3 *>(p);
        if (plugin3 != 0) {
            ++plugin3Count;
            QCOMPARE(plugin3->name(), pluginName3);
            abstractInputMethod = subject->plugins[p].inputMethod;
            QVERIFY(abstractInputMethod != 0);
            inputMethod3 = dynamic_cast<DummyInputMethod3 *>(abstractInputMethod);
            QVERIFY(inputMethod3 != 0);
            QCOMPARE(inputMethod3->setStateCount, 1);
            inputMethod3->setStateCount = 0;
            QCOMPARE(inputMethod3->setStateParam.size(), 1);
            QCOMPARE(*inputMethod3->setStateParam.begin(), MInputMethod::Accessory);
        }
        plugin = dynamic_cast<DummyImPlugin *>(p);
        if (plugin != 0) {
            ++pluginCount;
            QCOMPARE(plugin->name(), pluginName);
            abstractInputMethod = subject->plugins[p].inputMethod;
            QVERIFY(abstractInputMethod != 0);
            inputMethod = dynamic_cast<DummyInputMethod *>(abstractInputMethod);
            QVERIFY(inputMethod != 0);
            QCOMPARE(inputMethod->setStateCount, 1);
            inputMethod->setStateCount = 0;
            QCOMPARE(inputMethod->setStateParam.size(), 1);
            QCOMPARE(*inputMethod->setStateParam.begin(), MInputMethod::Hardware);
        }
    }
    QCOMPARE(pluginCount, 1);
    QCOMPARE(plugin3Count, 1);
}

void Ut_MIMPluginManager::testExistInputMethod()
{
    MIMPluginManagerPrivate::Plugins::iterator iterator;
    QSet<MInputMethod::HandlerState> actualState;

    subject->addHandlerMap(MInputMethod::OnScreen, pluginId);
    subject->addHandlerMap(MInputMethod::Hardware, pluginId);
    subject->addHandlerMap(MInputMethod::Accessory, pluginId3);

    actualState << MInputMethod::OnScreen;
    subject->setActiveHandlers(actualState);
    actualState.clear();
    actualState << MInputMethod::Accessory;
    subject->setActiveHandlers(actualState);

    for (iterator = subject->plugins.begin(); iterator != subject->plugins.end(); ++iterator) {
        // no matter the plugin is active or not, the inputmethod object is not empty.
        if (!iterator.key()->supportedStates().isEmpty()) {
            QVERIFY(iterator.value().inputMethod != 0);
        }
    }
}

void Ut_MIMPluginManager::testPluginSwitcher_data()
{
    QTest::addColumn<MInputMethod::HandlerState>("state");

    QTest::newRow("OnScreen")  << MInputMethod::OnScreen;
    QTest::newRow("Hardware")  << MInputMethod::Hardware;
    QTest::newRow("Accessory") << MInputMethod::Accessory;
}

void Ut_MIMPluginManager::testPluginSwitcher()
{
    QFETCH(MInputMethod::HandlerState, state);
    QSet<MInputMethod::HandlerState> actualState;
    DummyImPlugin  *plugin  = 0;
    DummyImPlugin3 *plugin3 = 0;
    MAbstractInputMethod *abstractInputMethod = 0;
    QPointer<DummyInputMethod > inputMethod  = 0;
    QPointer<DummyInputMethod3> inputMethod3 = 0;

    subject->addHandlerMap(MInputMethod::Hardware, pluginId);
    subject->addHandlerMap(MInputMethod::OnScreen, pluginId);
    subject->addHandlerMap(MInputMethod::Accessory, pluginId);

    // search for loaded plugins
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
    subject->switchPlugin(MInputMethod::SwitchUndefined, inputMethod);
    QVERIFY(inputMethod != 0);
    QCOMPARE(inputMethod->switchContextCallCount, 0);
    QCOMPARE(subject->plugins[plugin].lastSwitchDirection, MInputMethod::SwitchUndefined);
    QCOMPARE(subject->activePlugins.count(), 1);
    QVERIFY(plugin == *subject->activePlugins.begin());

    // switch forward
    subject->switchPlugin(MInputMethod::SwitchForward, inputMethod);
    QCOMPARE(subject->plugins[plugin].lastSwitchDirection, MInputMethod::SwitchForward);

    QCOMPARE(subject->activePlugins.count(), 1);
    QVERIFY(plugin3 == *subject->activePlugins.begin());
    abstractInputMethod = subject->plugins[plugin3].inputMethod;
    QVERIFY(abstractInputMethod != 0);
    inputMethod3 = dynamic_cast<DummyInputMethod3 *>(abstractInputMethod);
    QVERIFY(inputMethod3 != 0);
    QCOMPARE(inputMethod3->switchContextCallCount, 0);
    QCOMPARE(inputMethod3->setStateCount, 1);
    inputMethod3->setStateCount = 0;
    QCOMPARE(inputMethod3->setStateParam.size(), 1);
    QCOMPARE(*inputMethod3->setStateParam.begin(), state);
    checkHandlerMap(state, pluginId3);

    //switch backward
    inputMethod->setStateCount = 0;
    subject->switchPlugin(MInputMethod::SwitchBackward, inputMethod3);
    QCOMPARE(subject->plugins[plugin3].lastSwitchDirection, MInputMethod::SwitchBackward);

    QCOMPARE(subject->activePlugins.count(), 1);
    QVERIFY(plugin == *subject->activePlugins.begin());
    abstractInputMethod = subject->plugins[plugin].inputMethod;
    QVERIFY(abstractInputMethod != 0);
    inputMethod = dynamic_cast<DummyInputMethod *>(abstractInputMethod);
    QVERIFY(inputMethod != 0);
    QCOMPARE(inputMethod->switchContextCallCount, 0);
    QCOMPARE(inputMethod->setStateCount, 1);
    inputMethod->setStateCount = 0;
    QCOMPARE(inputMethod->setStateParam.size(), 1);
    QCOMPARE(*inputMethod->setStateParam.begin(), state);
    checkHandlerMap(state, pluginId);

    // ... again
    subject->switchPlugin(MInputMethod::SwitchBackward, inputMethod);
    QCOMPARE(subject->plugins[plugin].lastSwitchDirection, MInputMethod::SwitchBackward);

    QCOMPARE(subject->activePlugins.count(), 1);
    QVERIFY(plugin3 == *subject->activePlugins.begin());
    abstractInputMethod = subject->plugins[plugin3].inputMethod;
    QVERIFY(abstractInputMethod != 0);
    inputMethod3 = dynamic_cast<DummyInputMethod3 *>(abstractInputMethod);
    QVERIFY(inputMethod3 != 0);
    QCOMPARE(inputMethod3->switchContextCallCount, 1);
    inputMethod3->switchContextCallCount = 0;
    QCOMPARE(inputMethod3->directionParam, MInputMethod::SwitchBackward);
    QVERIFY(inputMethod3->enableAnimationParam == false);
    QCOMPARE(inputMethod3->setStateCount, 1);
    inputMethod3->setStateCount = 0;
    QCOMPARE(inputMethod3->setStateParam.size(), 1);
    QCOMPARE(*inputMethod3->setStateParam.begin(), state);
    checkHandlerMap(state, pluginId3);

    // try to switch to plugin which could not support the same state
    // nothing should be changed
    plugin->allowedStates.clear();
    subject->switchPlugin(MInputMethod::SwitchBackward, inputMethod3);
    plugin->allowedStates << MInputMethod::OnScreen << MInputMethod::Hardware
                          << MInputMethod::Accessory; // restore default configuration
    QCOMPARE(subject->plugins[plugin3].lastSwitchDirection, MInputMethod::SwitchBackward);

    QCOMPARE(subject->activePlugins.count(), 1);
    QVERIFY(plugin3 == *subject->activePlugins.begin());
    QVERIFY(inputMethod3 != 0);
    abstractInputMethod = inputMethod3;
    QVERIFY(abstractInputMethod == subject->plugins[plugin3].inputMethod);
    QCOMPARE(inputMethod3->switchContextCallCount, 0);
    QCOMPARE(inputMethod3->setStateCount, 0);
    inputMethod3->setStateCount = 0;
    checkHandlerMap(state, pluginId3);

    //test toolbar status when switch plugin
    MAttributeExtensionId toolbarId1(1, "toolbarIdTest1");
    subject->mAttributeExtensionManager->registerAttributeExtension(toolbarId1, Toolbar1);
    QSharedPointer<const MToolbarData> toolbarData1 =
        subject->mAttributeExtensionManager->toolbarData(toolbarId1);
    MAttributeExtensionId toolbarId2(2, "toolbarIdTest2");
    subject->mAttributeExtensionManager->registerAttributeExtension(toolbarId2, Toolbar2);
    QSharedPointer<const MToolbarData> toolbarData2 =
        subject->mAttributeExtensionManager->toolbarData(toolbarId2);

    QVERIFY(toolbarData1.data());
    QVERIFY(toolbarData2.data());
    QVERIFY(toolbarData1.data() != toolbarData2.data());

    manager->setToolbar(toolbarId1);
    subject->setActivePlugin(pluginId, MInputMethod::OnScreen);
    subject->switchPlugin(MInputMethod::SwitchForward, inputMethod);
    QVERIFY(inputMethod->toolbarParam == toolbarData1);
    QVERIFY(inputMethod3->toolbarParam == toolbarData1);

    manager->setToolbar(toolbarId2);
    subject->setActivePlugin(pluginId3, MInputMethod::OnScreen);
    subject->switchPlugin(MInputMethod::SwitchBackward, inputMethod3);
    QVERIFY(inputMethod->toolbarParam == toolbarData2);
    QVERIFY(inputMethod3->toolbarParam == toolbarData2);

    subject->mAttributeExtensionManager->unregisterAttributeExtension(toolbarId1);
    subject->mAttributeExtensionManager->unregisterAttributeExtension(toolbarId2);
}

void Ut_MIMPluginManager::checkHandlerMap(int handler, const QString &name)
{
    if (handler != MInputMethod::OnScreen) {
        const QString key
            = QString(PluginRoot
                      + subject->inputSourceName(static_cast<MInputMethod::HandlerState>(handler)));
        MImSettings gconf(key);
        QCOMPARE(gconf.value().toString(), name);
    }
}

void Ut_MIMPluginManager::testSwitchToSpecifiedPlugin()
{
    const MInputMethod::HandlerState state = MInputMethod::OnScreen;
    QSet<MInputMethod::HandlerState> actualState;
    DummyImPlugin  *plugin  = 0;
    DummyImPlugin3 *plugin3 = 0;
    MAbstractInputMethod *abstractInputMethod = 0;
    QPointer<DummyInputMethod > inputMethod  = 0;
    QPointer<DummyInputMethod3> inputMethod3 = 0;
    MImSettings lastActiveSubviewGconf(ActivePluginKey);

    subject->addHandlerMap(MInputMethod::OnScreen, pluginId);
    subject->addHandlerMap(MInputMethod::Hardware, pluginId);
    subject->addHandlerMap(MInputMethod::Accessory, pluginId);

    // search for loaded plugins
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
    subject->switchPlugin(pluginId, inputMethod);
    QCOMPARE(QString("dummyimsv1"), lastActiveSubviewGconf.value().toStringList().last());
    QVERIFY(inputMethod != 0);
    QCOMPARE(inputMethod->switchContextCallCount, 0);
    QCOMPARE(subject->plugins[plugin].lastSwitchDirection, MInputMethod::SwitchUndefined);
    QCOMPARE(subject->activePlugins.count(), 1);
    QVERIFY(plugin == *subject->activePlugins.begin());
    Q_FOREACH (MInputMethodPlugin *handler, subject->handlerToPlugin.values()) {
        QVERIFY(handler == plugin);
    }

    QCOMPARE(lastActiveSubviewGconf.value().toStringList().last(), QString("dummyimsv1"));
    // switch to another plugin
    subject->switchPlugin(pluginId3, inputMethod);
    QCOMPARE(lastActiveSubviewGconf.value().toStringList().last(), QString("dummyim3sv1"));
    QCOMPARE(subject->plugins[plugin].lastSwitchDirection, MInputMethod::SwitchUndefined);
    QVERIFY(inputMethod != 0);

    QCOMPARE(subject->activePlugins.count(), 1);
    QVERIFY(plugin3 == *subject->activePlugins.begin());
    abstractInputMethod = subject->plugins[plugin3].inputMethod;
    QVERIFY(abstractInputMethod != 0);
    inputMethod3 = dynamic_cast<DummyInputMethod3 *>(abstractInputMethod);
    QVERIFY(inputMethod3 != 0);
    QCOMPARE(inputMethod3->switchContextCallCount, 1);
    QCOMPARE(inputMethod3->directionParam, MInputMethod::SwitchUndefined);
    QCOMPARE(inputMethod3->setStateCount, 1);
    inputMethod3->setStateCount = 0;
    QCOMPARE(inputMethod3->setStateParam.size(), 1);
    QCOMPARE(*inputMethod3->setStateParam.begin(), state);
    Q_FOREACH (MInputMethodPlugin *handler, subject->handlerToPlugin.values()) {
        qDebug() << handler << plugin3;
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

    subject->setActivePlugin(pluginId3, MInputMethod::OnScreen);

    // check gconf item
    MImSettings handlerItem(ActivePluginKey);
    QCOMPARE(handlerItem.value().toStringList().first(), pluginId3);

    QVERIFY(subject->activePlugins.size() == 1);
    plugin = *subject->activePlugins.begin();
    QVERIFY(plugin != 0);
    QCOMPARE(plugin->name(), pluginName3);
}

void Ut_MIMPluginManager::testSubViews()
{
    QList<MAbstractInputMethod::MInputMethodSubView> subViews;
    Q_FOREACH (MInputMethodPlugin *plugin, subject->plugins.keys()) {
        subViews += subject->plugins[plugin].inputMethod->subViews(MInputMethod::OnScreen);
    }
    // only has subviews for MInputMethod::OnScreen
    QCOMPARE(subViews.count(), 5);

    subViews.clear();
    Q_FOREACH (MInputMethodPlugin *plugin, subject->plugins.keys()) {
        subViews += subject->plugins[plugin].inputMethod->subViews(MInputMethod::Hardware);
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
    QCOMPARE(subject->activeSubView(MInputMethod::OnScreen), QString("dummyimsv1"));
    subject->_q_setActiveSubView(QString("dummyimsv2"), MInputMethod::OnScreen);
    QCOMPARE(subject->activeSubView(MInputMethod::OnScreen), QString("dummyimsv2"));

    subject->setActivePlugin(pluginId3, MInputMethod::OnScreen);
    QCOMPARE(subject->activePlugins.size(), 1);
    plugin = *subject->activePlugins.begin();
    QVERIFY(plugin != 0);
    QCOMPARE(plugin->name(), pluginName3);
    QCOMPARE(subject->activeSubView(MInputMethod::OnScreen), QString("dummyim3sv1"));

    subject->_q_setActiveSubView(QString("dummyim3sv2"), MInputMethod::OnScreen);
    QCOMPARE(subject->activeSubView(MInputMethod::OnScreen), QString("dummyim3sv2"));
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
    QCOMPARE(subject->activeSubView(MInputMethod::OnScreen), QString("dummyimsv1"));

    QVERIFY(m_clientInterface);
    QDBusReply<QStringList> AvailablePluginReply
        = m_clientInterface->call("queryAvailablePlugins", MInputMethod::OnScreen);
    QVERIFY(AvailablePluginReply.isValid());
    QCOMPARE(AvailablePluginReply.value().count(), 2);
    QVERIFY(AvailablePluginReply.value().contains(pluginId));
    QVERIFY(AvailablePluginReply.value().contains(pluginId3));

    QDBusReply<QString> ActivePluginReply = m_clientInterface->call("queryActivePlugin",
                                                                    MInputMethod::OnScreen);
    QVERIFY(ActivePluginReply.isValid());
    QCOMPARE(ActivePluginReply.value(), pluginId);

    QDBusReply< QMap<QString, QVariant> > activeSubViewReply
        = m_clientInterface->call("queryActiveSubView", MInputMethod::OnScreen);
    QVERIFY(activeSubViewReply.isValid());
    QVERIFY(activeSubViewReply.value().count() > 0);
    QCOMPARE(activeSubViewReply.value().values().at(0).toString(), pluginId);
    QCOMPARE(activeSubViewReply.value().keys().at(0), QString("dummyimsv1"));

    QDBusReply< QMap<QString, QVariant> > subViewsReply
        = m_clientInterface->call("queryAvailableSubViews", pluginId, MInputMethod::OnScreen);
    QVERIFY(subViewsReply.isValid());
    QCOMPARE(subViewsReply.value().count(), 3);
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
    QCOMPARE(subject->activeSubView(MInputMethod::OnScreen), QString("dummyimsv1"));

    QVERIFY(m_clientInterface);

    m_clientInterface->call(QDBus::NoBlock, "setActivePlugin", pluginId3,
                            static_cast<int>(MInputMethod::OnScreen));
    handleMessages();

    QVERIFY(subject->activePlugins.size() == 1);
    plugin = *subject->activePlugins.begin();
    QVERIFY(plugin != 0);
    QCOMPARE(plugin->name(), pluginName3);
    QCOMPARE(subject->activeSubView(MInputMethod::OnScreen), QString("dummyim3sv1"));

    // try to set a wrong plugin
    m_clientInterface->call(QDBus::Block, "setActivePlugin", pluginId2,
                            static_cast<int>(MInputMethod::OnScreen));
    handleMessages();
    QVERIFY(subject->activePlugins.size() == 1);
    plugin = *subject->activePlugins.begin();
    QVERIFY(plugin != 0);
    QCOMPARE(plugin->name(), pluginName3);

    m_clientInterface->call(QDBus::Block, "setActiveSubView", QString("dummyim3sv2"),
                            static_cast<int>(MInputMethod::OnScreen));
    handleMessages();
    QCOMPARE(subject->activeSubView(MInputMethod::OnScreen), QString("dummyim3sv2"));

    // try to set a wrong subview
    m_clientInterface->call(QDBus::Block, "setActiveSubView", QString("dummyimisv2"),
                            static_cast<int>(MInputMethod::OnScreen));
    handleMessages();
    QCOMPARE(subject->activeSubView(MInputMethod::OnScreen), QString("dummyim3sv2"));

    // try to set both subview id and plugin
    m_clientInterface->call(QDBus::Block, "setActivePlugin", pluginId,
                            static_cast<int>(MInputMethod::OnScreen),
                            QString("dummyimsv1"));
    handleMessages();
    QVERIFY(subject->activePlugins.size() == 1);
    plugin = *subject->activePlugins.begin();
    QVERIFY(plugin != 0);
    QCOMPARE(plugin->name(), pluginName);
    QCOMPARE(subject->activeSubView(MInputMethod::OnScreen), QString("dummyimsv1"));
}

void Ut_MIMPluginManager::testRegionUpdates()
{
    MInputMethodPlugin *plugin3 = 0;
    QSignalSpy regionUpdates(manager, SIGNAL(regionUpdated(QRegion)));
    QList<QVariant> regionUpdatesSignal;
    QVariant region;

    Q_FOREACH(MInputMethodPlugin * plugin, subject->plugins.keys()) {
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

    // DummyImPlugin3 is a badly behaving plugin that doesn't send an empty region
    // when it's hidden...
    manager->hideActivePlugins();
    QCOMPARE(regionUpdates.count(), 0);
    // ...so make sure the region is sent by the plugin manager after a timeout.
    MaliitTestUtils::waitForSignal(manager, SIGNAL(regionUpdated(QRegion)), 3000);
    QCOMPARE(regionUpdates.count(), 1);

    region = regionUpdates.takeFirst().at(0);
    QVERIFY(region.value<QRegion>().isEmpty());
}

void Ut_MIMPluginManager::testSetToolbar()
{
    MInputMethodPlugin *plugin1 = 0;
    Q_FOREACH(MInputMethodPlugin * plugin, subject->plugins.keys()) {
        if (plugin->name() == "DummyImPlugin") {
            plugin1 = plugin;
        }
    }
    QVERIFY(plugin1);

    QPointer<DummyInputMethod > inputMethod  = 0;
    inputMethod  = dynamic_cast<DummyInputMethod  *>(subject->plugins[plugin1].inputMethod);
    QVERIFY(inputMethod);

    MAttributeExtensionId toolbarId1(1, "toolbarIdTest1");
    subject->mAttributeExtensionManager->registerAttributeExtension(toolbarId1, Toolbar1);
    QSharedPointer<const MToolbarData> toolbarData1 =
        subject->mAttributeExtensionManager->toolbarData(toolbarId1);
    MAttributeExtensionId toolbarId2(2, "toolbarIdTest2");
    subject->mAttributeExtensionManager->registerAttributeExtension(toolbarId2, Toolbar2);
    QSharedPointer<const MToolbarData> toolbarData2 =
        subject->mAttributeExtensionManager->toolbarData(toolbarId2);

    QVERIFY(toolbarData1.data());
    QVERIFY(toolbarData2.data());
    QVERIFY(toolbarData1.data() != toolbarData2.data());

    subject->setActivePlugin(plugin1->name(), MInputMethod::OnScreen);
    manager->setToolbar(toolbarId1);
    QVERIFY(inputMethod->toolbarParam == toolbarData1);
    manager->setToolbar(toolbarId2);
    QVERIFY(inputMethod->toolbarParam == toolbarData2);

    subject->mAttributeExtensionManager->unregisterAttributeExtension(toolbarId1);
    subject->mAttributeExtensionManager->unregisterAttributeExtension(toolbarId2);
}

void Ut_MIMPluginManager::testLoadedPluginsInfo_data()
{
    QTest::addColumn<QStringList>("expectedPlugins");
    QTest::addColumn<MInputMethod::HandlerState>("state");

    QTest::newRow("OnScreen") << (QStringList() << pluginName << pluginName3) << MInputMethod::OnScreen;

    QTest::newRow("Hardware") << (QStringList() << pluginName) << MInputMethod::Hardware;
}

void Ut_MIMPluginManager::testLoadedPluginsInfo()
{
    QFETCH(QStringList, expectedPlugins);
    QFETCH(MInputMethod::HandlerState, state);

    QStringList foundPlugins;

    DummyImPlugin3 *plugin3 = 0;
    // search for loaded plugins
    for (MIMPluginManagerPrivate::Plugins::iterator iterator(subject->plugins.begin());
         iterator != subject->plugins.end();
         ++iterator) {
        if (iterator.key() == 0) {
            continue;
        }
        if (pluginName3 == iterator.key()->name()) {
            plugin3 = dynamic_cast<DummyImPlugin3 *>(iterator.key());
        }
    }

    QVERIFY(plugin3 != 0);
    plugin3->allowedStates.clear();
    plugin3->allowedStates << MInputMethod::OnScreen;

    QList<MImPluginDescription> list = subject->pluginDescriptions(state);

    QCOMPARE(list.count(), expectedPlugins.count());

    Q_FOREACH(const MImPluginDescription &info, list) {
        QVERIFY(expectedPlugins.contains(info.name()));

        // check for duplicates
        QVERIFY(!foundPlugins.contains(info.name()));
        foundPlugins.append(info.name());
    }
}

void Ut_MIMPluginManager::testSubViewsInfo_data()
{
    QTest::addColumn<QStringList>("enabledPlugins");
    QTest::addColumn<QString>("activeSubView");
    QTest::addColumn<QStringList>("expectedTitles");

    QTest::newRow("no subviews") << QStringList() // at least we should not crash here
                                 << "dummyimsv1"
                                 << QStringList();

    QTest::newRow("one subview") << (QStringList() << pluginId << "dummyimsv1")
                                 << "dummyimsv1"
                                 << QStringList();

    QTest::newRow("two subviews") << (QStringList() << pluginId << "dummyimsv1" << pluginId << "dummyimsv2")
                                  << "dummyimsv1"
                                  << (QStringList() << "dummyimsv2" << "dummyimsv2");

    QTest::newRow("sv1") << DefaultEnabledPlugins
                         << "dummyimsv1"
                         << (QStringList() << "dummyim3sv2" << "dummyimsv2");

    // last test case have to use DefaultEnabledPlugins to restore default settings
    QTest::newRow("sv2") << DefaultEnabledPlugins
                         << "dummyimsv2"
                         << (QStringList() << "dummimysv1" << "dummyim3sv1");
}

void Ut_MIMPluginManager::testSubViewsInfo()
{
    QFETCH(QStringList, enabledPlugins);
    QFETCH(QString, activeSubView);
    QFETCH(QStringList, expectedTitles);

    MImSettings enabledPluginsSettings(EnabledPluginsKey);
    enabledPluginsSettings.set(enabledPlugins);

    subject->_q_setActiveSubView(activeSubView, MInputMethod::OnScreen);
    QVERIFY(subject->activeSubView(MInputMethod::OnScreen) == activeSubView);
    QList<MImSubViewDescription> subViewDescriptions = subject->surroundingSubViewDescriptions(MInputMethod::OnScreen);

    QCOMPARE(subViewDescriptions.size(), expectedTitles.size());
    for(int n = 0; n < subViewDescriptions.size(); ++n) {
        QCOMPARE(subViewDescriptions.at(n).title(), expectedTitles.at(n));
    }
}

void Ut_MIMPluginManager::handleMessages()
{
    while (app->hasPendingEvents()) {
        app->processEvents();
    }
}

QTEST_APPLESS_MAIN(Ut_MIMPluginManager)
