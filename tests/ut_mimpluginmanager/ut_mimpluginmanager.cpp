#include "ut_mimpluginmanager.h"

#include "dummyimplugin.h"
#include "dummyimplugin3.h"
#include "dummyinputmethod.h"
#include "dummyinputmethod3.h"

#include "minputcontextconnection.h"
#include "mimsettingsqsettings.h"

#include "core-utils.h"

#include <QProcess>
#include <QRegExp>
#include <QCoreApplication>
#include <QPointer>
#include <QTimer>
#include <QEventLoop>
#include <QStringList>
#include <mimpluginmanager.h>
#include <mimpluginmanager_p.h>
#include <maliit/plugins/inputmethodplugin.h>
#include <unknownplatform.h>

#include "mattributeextensionmanager.h"
#include "msharedattributeextensionmanager.h"

typedef QSet<Maliit::HandlerState> HandlerStates;
Q_DECLARE_METATYPE(HandlerStates);
Q_DECLARE_METATYPE(Maliit::HandlerState);

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

    const QString testDirectory = "/ut_mimpluginmanager";

    const QStringList DefaultEnabledPlugins = QStringList()
                                              << pluginId + ":" + "dummyimsv1"
                                              << pluginId + ":" + "dummyimsv2"
                                              << pluginId3 + ":" + "dummyim3sv1"
                                              << pluginId3 + ":" + "dummyim3sv2";

    const QStringList DefaultActivePlugin = QStringList() << pluginId + ":" + "dummyimsv1";
    const QStringList DefaultBlackList = QStringList() << "libdummyimplugin2.so" << "libmeego-keyboard.so";
}

class MInputContextTestConnection : public MInputContextConnection
{
public:
    MInputContextTestConnection() :
        notifyExtendedAttributeChanged_called(0)
    {
    }

    void notifyExtendedAttributeChanged(const QList<int> &clientIds, int id, const QString &target, const QString &targetItem, const QString &attribute, const QVariant &value)
    {
        Q_UNUSED(id);

        notifyExtendedAttributeChanged_called++;
        notifyExtendedAttributeChanged_clientIds = clientIds;
        notifyExtendedAttributeChanged_key = QString("%1/%2/%3").arg(target).arg(targetItem).arg(attribute);
        notifyExtendedAttributeChanged_value = value;
    }

    int notifyExtendedAttributeChanged_called;
    QList<int> notifyExtendedAttributeChanged_clientIds;
    QString notifyExtendedAttributeChanged_key;
    QVariant notifyExtendedAttributeChanged_value;
};


void Ut_MIMPluginManager::initTestCase()
{
    MImSettings::setPreferredSettingsType(MImSettings::TemporarySettings);
}

void Ut_MIMPluginManager::cleanupTestCase()
{
}

void Ut_MIMPluginManager::init()
{
    MImSettings pathConf(MImPluginPaths);
    pathConf.set(MaliitTestUtils::getTestPluginPath());

    MImSettings blackListConf(MImPluginDisabled);
    blackListConf.set(DefaultBlackList);

    MImSettings enabledPluginsSettings(EnabledPluginsKey);
    enabledPluginsSettings.set(DefaultEnabledPlugins);

    QSharedPointer<MInputContextTestConnection> icConnection(new MInputContextTestConnection);
    manager = new MIMPluginManager(icConnection, QSharedPointer<Maliit::AbstractPlatform>(new Maliit::UnknownPlatform));

    connection = icConnection.data();
    subject = manager->d_ptr;

    QVERIFY(subject->activePlugins.size() == 1);
    subject->setActivePlugin(pluginId, Maliit::OnScreen);
    Maliit::Plugins::InputMethodPlugin *plugin = 0;
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
}

void Ut_MIMPluginManager::cleanup()
{
    delete manager;
    manager = 0;
    subject = 0;
}


// Test methods..............................................................

void Ut_MIMPluginManager::testLoadPlugins()
{
    Maliit::Plugins::InputMethodPlugin *plugin = 0;
    Maliit::Plugins::InputMethodPlugin *plugin3 = 0;

    // Initial load based on settings -> DummyImPlugin loaded and activated,
    // DummyImPlugin3 loaded, DummyPlugin2 not loaded (skipped).  Also,
    // DummyPlugin ignored (it is currently left loaded but even though that
    // might better be fixed, it is not relevant to this test) and
    // libinvalidplugin not loaded.  The only "test" for these two is that the
    // test does not crash.  (One may also observe the warning/debug messages
    // concerning loading of those two plugins.)
    Q_FOREACH(Maliit::Plugins::InputMethodPlugin * plugin, subject->plugins.keys()) {
        qDebug() << plugin->name();
    }
    QCOMPARE(subject->plugins.size(), 2);
    QCOMPARE(subject->activePlugins.size(), 1);
    plugin = *subject->activePlugins.begin();
    QCOMPARE(plugin->name(), pluginName);
    bool dummyImPluginFound = false;
    bool dummyImPlugin3Found = false;
    Q_FOREACH(Maliit::Plugins::InputMethodPlugin * plugin, subject->plugins.keys()) {
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
    Maliit::Plugins::InputMethodPlugin *plugin = 0;

    subject->addHandlerMap(Maliit::OnScreen, pluginId);
    subject->addHandlerMap(Maliit::Hardware, pluginId);
    subject->addHandlerMap(Maliit::Accessory, pluginId3);

    plugin = subject->handlerToPlugin[Maliit::OnScreen];
    QVERIFY(plugin != 0);
    QCOMPARE(plugin->name(), pluginName);

    plugin = subject->handlerToPlugin[Maliit::Hardware];
    QVERIFY(plugin != 0);
    QCOMPARE(plugin->name(), pluginName);

    plugin = subject->handlerToPlugin[Maliit::Accessory];
    QVERIFY(plugin != 0);
    QCOMPARE(plugin->name(), pluginName3);
}

void Ut_MIMPluginManager::testSwitchPluginState()
{
    QSet<Maliit::HandlerState> actualState;
    DummyImPlugin  *plugin  = 0;
    DummyImPlugin3 *plugin3 = 0;
    MAbstractInputMethod *abstractInputMethod = 0;
    DummyInputMethod  *inputMethod  = 0;
    DummyInputMethod3 *inputMethod3 = 0;

    subject->addHandlerMap(Maliit::OnScreen, pluginId);
    subject->addHandlerMap(Maliit::Hardware, pluginId);
    subject->addHandlerMap(Maliit::Accessory, pluginId3);

    actualState << Maliit::OnScreen;
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
    QCOMPARE(*inputMethod->setStateParam.begin(), Maliit::OnScreen);

    actualState.clear();
    actualState << Maliit::Hardware;
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
    QCOMPARE(*inputMethod->setStateParam.begin(), Maliit::Hardware);

    actualState.clear();
    actualState << Maliit::Accessory;
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
    QCOMPARE(*inputMethod3->setStateParam.begin(), Maliit::Accessory);
}


void Ut_MIMPluginManager::testMultiplePlugins()
{
    QSet<Maliit::HandlerState> actualState;
    DummyImPlugin  *plugin  = 0;
    DummyImPlugin3 *plugin3 = 0;
    int pluginCount = 0;
    int plugin3Count = 0;
    MAbstractInputMethod *abstractInputMethod = 0;
    DummyInputMethod  *inputMethod  = 0;
    DummyInputMethod3 *inputMethod3 = 0;

    subject->addHandlerMap(Maliit::OnScreen, pluginId);
    subject->addHandlerMap(Maliit::Hardware, pluginId);
    subject->addHandlerMap(Maliit::Accessory, pluginId3);

    actualState << Maliit::Accessory << Maliit::Hardware;
    subject->setActiveHandlers(actualState);
    QCOMPARE(subject->activePlugins.size(), 2);
    Q_FOREACH(Maliit::Plugins::InputMethodPlugin * p, subject->activePlugins) {
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
            QCOMPARE(*inputMethod3->setStateParam.begin(), Maliit::Accessory);
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
            QCOMPARE(*inputMethod->setStateParam.begin(), Maliit::Hardware);
        }
    }
    QCOMPARE(pluginCount, 1);
    QCOMPARE(plugin3Count, 1);
}

void Ut_MIMPluginManager::testExistInputMethod()
{
    MIMPluginManagerPrivate::Plugins::iterator iterator;
    QSet<Maliit::HandlerState> actualState;

    subject->addHandlerMap(Maliit::OnScreen, pluginId);
    subject->addHandlerMap(Maliit::Hardware, pluginId);
    subject->addHandlerMap(Maliit::Accessory, pluginId3);

    actualState << Maliit::OnScreen;
    subject->setActiveHandlers(actualState);
    actualState.clear();
    actualState << Maliit::Accessory;
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
    QTest::addColumn<Maliit::HandlerState>("state");

    QTest::newRow("OnScreen")  << Maliit::OnScreen;
    QTest::newRow("Hardware")  << Maliit::Hardware;
    QTest::newRow("Accessory") << Maliit::Accessory;
}

void Ut_MIMPluginManager::testPluginSwitcher()
{
    QFETCH(Maliit::HandlerState, state);
    QSet<Maliit::HandlerState> actualState;
    DummyImPlugin  *plugin  = 0;
    DummyImPlugin3 *plugin3 = 0;
    MAbstractInputMethod *abstractInputMethod = 0;
    QPointer<DummyInputMethod > inputMethod  = 0;
    QPointer<DummyInputMethod3> inputMethod3 = 0;

    subject->addHandlerMap(Maliit::Hardware, pluginId);
    subject->addHandlerMap(Maliit::OnScreen, pluginId);
    subject->addHandlerMap(Maliit::Accessory, pluginId);

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
    subject->switchPlugin(Maliit::SwitchUndefined, inputMethod);
    QVERIFY(inputMethod != 0);
    QCOMPARE(inputMethod->switchContextCallCount, 0);
    QCOMPARE(subject->plugins[plugin].lastSwitchDirection, Maliit::SwitchUndefined);
    QCOMPARE(subject->activePlugins.count(), 1);
    QVERIFY(plugin == *subject->activePlugins.begin());

    // switch forward
    subject->switchPlugin(Maliit::SwitchForward, inputMethod);
    QCOMPARE(subject->plugins[plugin].lastSwitchDirection, Maliit::SwitchForward);

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
    subject->switchPlugin(Maliit::SwitchBackward, inputMethod3);
    QCOMPARE(subject->plugins[plugin3].lastSwitchDirection, Maliit::SwitchBackward);

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
    subject->switchPlugin(Maliit::SwitchBackward, inputMethod);
    QCOMPARE(subject->plugins[plugin].lastSwitchDirection, Maliit::SwitchBackward);

    QCOMPARE(subject->activePlugins.count(), 1);
    QVERIFY(plugin3 == *subject->activePlugins.begin());
    abstractInputMethod = subject->plugins[plugin3].inputMethod;
    QVERIFY(abstractInputMethod != 0);
    inputMethod3 = dynamic_cast<DummyInputMethod3 *>(abstractInputMethod);
    QVERIFY(inputMethod3 != 0);
    QCOMPARE(inputMethod3->switchContextCallCount, 1);
    inputMethod3->switchContextCallCount = 0;
    QCOMPARE(inputMethod3->directionParam, Maliit::SwitchBackward);
    QVERIFY(inputMethod3->enableAnimationParam == false);
    QCOMPARE(inputMethod3->setStateCount, 1);
    inputMethod3->setStateCount = 0;
    QCOMPARE(inputMethod3->setStateParam.size(), 1);
    QCOMPARE(*inputMethod3->setStateParam.begin(), state);
    checkHandlerMap(state, pluginId3);

    // try to switch to plugin which could not support the same state
    // nothing should be changed
    plugin->allowedStates.clear();
    subject->switchPlugin(Maliit::SwitchBackward, inputMethod3);
    plugin->allowedStates << Maliit::OnScreen << Maliit::Hardware
                          << Maliit::Accessory; // restore default configuration
    QCOMPARE(subject->plugins[plugin3].lastSwitchDirection, Maliit::SwitchBackward);

    QCOMPARE(subject->activePlugins.count(), 1);
    QVERIFY(plugin3 == *subject->activePlugins.begin());
    QVERIFY(inputMethod3 != 0);
    abstractInputMethod = inputMethod3;
    QVERIFY(abstractInputMethod == subject->plugins[plugin3].inputMethod);
    QCOMPARE(inputMethod3->switchContextCallCount, 0);
    QCOMPARE(inputMethod3->setStateCount, 0);
    inputMethod3->setStateCount = 0;
    checkHandlerMap(state, pluginId3);
}

void Ut_MIMPluginManager::checkHandlerMap(int handler, const QString &name)
{
    if (handler != Maliit::OnScreen) {
        const QString key
            = QString(PluginRoot
                      + subject->inputSourceName(static_cast<Maliit::HandlerState>(handler)));
        MImSettings setting(key);
        QCOMPARE(setting.value().toString(), name);
    }
}

void Ut_MIMPluginManager::testSwitchToSpecifiedPlugin()
{
    const Maliit::HandlerState state = Maliit::OnScreen;
    QSet<Maliit::HandlerState> actualState;
    DummyImPlugin  *plugin  = 0;
    DummyImPlugin3 *plugin3 = 0;
    MAbstractInputMethod *abstractInputMethod = 0;
    QPointer<DummyInputMethod > inputMethod  = 0;
    QPointer<DummyInputMethod3> inputMethod3 = 0;
    MImSettings lastActiveSubviewSetting(ActivePluginKey);

    subject->addHandlerMap(Maliit::OnScreen, pluginId);
    subject->addHandlerMap(Maliit::Hardware, pluginId);
    subject->addHandlerMap(Maliit::Accessory, pluginId);

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
    QCOMPARE(QString("dummyimsv1"), lastActiveSubviewSetting.value().toString().section(':', 1));
    QVERIFY(inputMethod != 0);
    QCOMPARE(inputMethod->switchContextCallCount, 0);
    QCOMPARE(subject->plugins[plugin].lastSwitchDirection, Maliit::SwitchUndefined);
    QCOMPARE(subject->activePlugins.count(), 1);
    QVERIFY(plugin == *subject->activePlugins.begin());
    Q_FOREACH (Maliit::Plugins::InputMethodPlugin *handler, subject->handlerToPlugin.values()) {
        QVERIFY(handler == plugin);
    }

    QCOMPARE(lastActiveSubviewSetting.value().toString().section(':', 1), QString("dummyimsv1"));
    // switch to another plugin
    subject->switchPlugin(pluginId3, inputMethod);
    QCOMPARE(lastActiveSubviewSetting.value().toString().section(':', 1), QString("dummyim3sv1"));
    QCOMPARE(subject->plugins[plugin].lastSwitchDirection, Maliit::SwitchUndefined);
    QVERIFY(inputMethod != 0);

    QCOMPARE(subject->activePlugins.count(), 1);
    QVERIFY(plugin3 == *subject->activePlugins.begin());
    abstractInputMethod = subject->plugins[plugin3].inputMethod;
    QVERIFY(abstractInputMethod != 0);
    inputMethod3 = dynamic_cast<DummyInputMethod3 *>(abstractInputMethod);
    QVERIFY(inputMethod3 != 0);
    QCOMPARE(inputMethod3->switchContextCallCount, 1);
    QCOMPARE(inputMethod3->directionParam, Maliit::SwitchUndefined);
    QCOMPARE(inputMethod3->setStateCount, 1);
    inputMethod3->setStateCount = 0;
    QCOMPARE(inputMethod3->setStateParam.size(), 1);
    QCOMPARE(*inputMethod3->setStateParam.begin(), state);
    Q_FOREACH (Maliit::Plugins::InputMethodPlugin *handler, subject->handlerToPlugin.values()) {
        qDebug() << handler << plugin3;
        QVERIFY(handler == plugin3);
    }
}

void Ut_MIMPluginManager::testSwitchShow_data()
{
    QTest::addColumn<bool>("visible");
    QTest::addColumn<int>("showCount");

    QTest::newRow("visible") << true << 1;
    QTest::newRow("hidden") << false << 0;
}

// see NB#296576 - Vkb is not shown in text field, when swype keyboard is enabled & disabled in a scenario
// switching a plugin will call show on the new one,
// if that happens while vkb is shown (which is correct),
// and shortly after vkb is hidden (which is incorrect)
void Ut_MIMPluginManager::testSwitchShow()
{
    QFETCH(bool, visible);
    QFETCH(int, showCount);

    // preparation
    MImSettings enabledPluginsSettings(EnabledPluginsKey);
    enabledPluginsSettings.set(QStringList()
                                << pluginId3 + ":" + "dummyim3sv1"
                                << pluginId + ":" + "dummyimsv1");
    subject->setActivePlugin(pluginId, Maliit::OnScreen);
    subject->showActivePlugins();

    // current input method
    MAbstractInputMethod *im = 0;
    // the other input method, we will watch it
    DummyInputMethod3 *dummyIm3 = 0;
    Q_FOREACH (MIMPluginManagerPrivate::PluginDescription pd, subject->plugins.values()) {
        MAbstractInputMethod *imi = pd.inputMethod;
        Q_FOREACH (MAbstractInputMethod::MInputMethodSubView v, imi->subViews()) {
            if (v.subViewId == "dummyimsv1") {
                im = imi;
            }
            if (v.subViewId == "dummyim3sv1") {
                dummyIm3 = qobject_cast<DummyInputMethod3*>(imi);
            }
        }
    }
    QVERIFY(im != 0);
    QVERIFY(dummyIm3 != 0);
    // end of preparation

    if (!visible) {
        subject->hideActivePlugins();
    }

    QSignalSpy shown(dummyIm3, SIGNAL(showCalled()));
    // swithching to the dummyIm3
    subject->switchPlugin(Maliit::SwitchForward, im);
    QCOMPARE(shown.size(), showCount);
}

void Ut_MIMPluginManager::testSetActivePlugin()
{
    QVERIFY(subject->activePlugins.size() == 1);
    Maliit::Plugins::InputMethodPlugin *plugin = 0;
    plugin = *subject->activePlugins.begin();
    QVERIFY(plugin != 0);
    QCOMPARE(plugin->name(), pluginName);

    subject->setActivePlugin(pluginId3, Maliit::OnScreen);

    // check settings entry
    MImSettings handlerItem(ActivePluginKey);
    QCOMPARE(handlerItem.value().toString().section(':', 0, 0), pluginId3);

    QVERIFY(subject->activePlugins.size() == 1);
    plugin = *subject->activePlugins.begin();
    QVERIFY(plugin != 0);
    QCOMPARE(plugin->name(), pluginName3);
}

void Ut_MIMPluginManager::testSubViews()
{
    QList<MAbstractInputMethod::MInputMethodSubView> subViews;
    Q_FOREACH (Maliit::Plugins::InputMethodPlugin *plugin, subject->plugins.keys()) {
        subViews += subject->plugins[plugin].inputMethod->subViews(Maliit::OnScreen);
    }
    // only has subviews for Maliit::OnScreen
    QCOMPARE(subViews.count(), 8);

    subViews.clear();
    Q_FOREACH (Maliit::Plugins::InputMethodPlugin *plugin, subject->plugins.keys()) {
        subViews += subject->plugins[plugin].inputMethod->subViews(Maliit::Hardware);
    }
    // doesn't have subviews for Hardware
    QCOMPARE(subViews.count(), 0);

}

void Ut_MIMPluginManager::testActiveSubView()
{
    QVERIFY(subject->activePlugins.size() == 1);
    Maliit::Plugins::InputMethodPlugin *plugin = 0;
    plugin = *subject->activePlugins.begin();
    QVERIFY(plugin != 0);
    QCOMPARE(plugin->name(), pluginName);
    QCOMPARE(subject->activeSubView(Maliit::OnScreen), QString("dummyimsv1"));
    subject->_q_setActiveSubView(QString("dummyimsv2"), Maliit::OnScreen);
    QCOMPARE(subject->activeSubView(Maliit::OnScreen), QString("dummyimsv2"));

    subject->setActivePlugin(pluginId3, Maliit::OnScreen);
    QCOMPARE(subject->activePlugins.size(), 1);
    plugin = *subject->activePlugins.begin();
    QVERIFY(plugin != 0);
    QCOMPARE(plugin->name(), pluginName3);
    QCOMPARE(subject->activeSubView(Maliit::OnScreen), QString("dummyim3sv1"));

    subject->_q_setActiveSubView(QString("dummyim3sv2"), Maliit::OnScreen);
    QCOMPARE(subject->activeSubView(Maliit::OnScreen), QString("dummyim3sv2"));
}

void Ut_MIMPluginManager::testLoadedPluginsInfo_data()
{
    QTest::addColumn<QStringList>("expectedPlugins");
    QTest::addColumn<Maliit::HandlerState>("state");

    QTest::newRow("OnScreen") << (QStringList() << pluginName << pluginName3) << Maliit::OnScreen;

    QTest::newRow("Hardware") << (QStringList() << pluginName) << Maliit::Hardware;
}

void Ut_MIMPluginManager::testLoadedPluginsInfo()
{
    QFETCH(QStringList, expectedPlugins);
    QFETCH(Maliit::HandlerState, state);

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
    plugin3->allowedStates << Maliit::OnScreen;

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

    QTest::newRow("one subview") << (QStringList() << pluginId + ":" + "dummyimsv1")
                                 << "dummyimsv1"
                                 << QStringList();

    QTest::newRow("two subviews") << (QStringList() << pluginId + ":" + "dummyimsv1" << pluginId + ":" + "dummyimsv2")
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

    subject->_q_setActiveSubView(activeSubView, Maliit::OnScreen);
    QVERIFY(subject->activeSubView(Maliit::OnScreen) == activeSubView);
    QList<MImSubViewDescription> subViewDescriptions = subject->surroundingSubViewDescriptions(Maliit::OnScreen);

    QCOMPARE(subViewDescriptions.size(), expectedTitles.size());
    for(int n = 0; n < subViewDescriptions.size(); ++n) {
        QCOMPARE(subViewDescriptions.at(n).title(), expectedTitles.at(n));
    }
}

void Ut_MIMPluginManager::testEnableAllSubviews()
{
    //load all subviews provided by all available plugins
    manager->setAllSubViewsEnabled(true);
    handleMessages();

    // enumerate all subviews provided by all available plugins
    QStringList allSubViews;
    Q_FOREACH(Maliit::Plugins::InputMethodPlugin * p, subject->plugins.keys()) {
        Q_FOREACH (const MAbstractInputMethod::MInputMethodSubView &s,
                   subject->plugins[p].inputMethod->subViews(Maliit::OnScreen)) {
                allSubViews << subject->plugins[p].pluginId + ":" + s.subViewId;
        }
    }

    MImSettings enabledPluginsSettings(EnabledPluginsKey);
    QStringList enabledSubViews = enabledPluginsSettings.value().value<QStringList>();

    // every available subview should be enabled
    for (int n = 0; n < allSubViews.count() - 1; ++n) {
        int index = enabledSubViews.indexOf(allSubViews.at(n + 1));
        QVERIFY(index > 0);
    }
}

void Ut_MIMPluginManager::handleMessages()
{
    QTest::qWait(100);
    QCoreApplication::processEvents();
}

QTEST_MAIN(Ut_MIMPluginManager)
