#include "ft_mimpluginmanager.h"

#include "mkeyboardstatetracker_stub.h"
#include "dummyimplugin.h"
#include "dummyimplugin3.h"
#include "dummyinputmethod.h"
#include "dummyinputmethod3.h"
#include "core-utils.h"
#include "mimsettingsqsettings.h"

#include <minputcontextconnection.h>
#include <mimpluginmanager.h>
#include <mimpluginmanager_p.h>
#include <msharedattributeextensionmanager.h>

#include <QProcess>
#include <QGraphicsScene>
#include <QRegExp>
#include <QApplication>
#include <QPointer>

using namespace std::tr1;

namespace
{
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
    const QString PluginSettings     = MALIIT_CONFIG_ROOT"pluginsettings";
    const QString MImAccesoryEnabled = MALIIT_CONFIG_ROOT"accessoryenabled";

    const MImPluginDescription* findPluginDescriptions(const QList<MImPluginDescription> &list, const QString &pluginName)
    {
        Q_FOREACH (const MImPluginDescription &desc, list) {
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
    MImSettings::setImplementationFactory(new MImSettingsQSettingsTemporaryBackendFactory);
}

void Ft_MIMPluginManager::cleanupTestCase()
{
}

void Ft_MIMPluginManager::init()
{
    MImSettings(MImPluginPaths).set(MaliitTestUtils::getTestPluginPath());
    MImSettings(MImPluginDisabled).set(QStringList("libdummyimplugin2.so"));
    MImSettings(MImPluginActive).set(QStringList("DummyImPlugin"));

    MImSettings(PluginRoot + "hardware").set(pluginId);
    MImSettings(PluginRoot + "accessory").set(pluginId3);

    MImSettings enabledPluginsSettings(EnabledPluginsKey);
    QStringList enabledPlugins;
    enabledPlugins << pluginId + ":" + "dummyimsv1";
    enabledPlugins << pluginId + ":" + "dummyimsv2";
    enabledPlugins << pluginId3 + ":" + "dummyim3sv1";
    enabledPlugins << pluginId3 + ":" + "dummyim3sv2";
    enabledPluginsSettings.set(enabledPlugins);

    MImSettings activePluginSettings(ActivePluginKey);
    QString activePlugin;
    activePlugin = pluginId + ":" + "dummyimsv1";
    activePluginSettings.set(activePlugin);

    gMKeyboardStateTrackerStub->setOpenState(false);
    MImSettings(MImAccesoryEnabled).set(QVariant(false));

    shared_ptr<MInputContextConnection> icConnection(new MInputContextConnection);
    subject = new MIMPluginManager(icConnection, QSharedPointer<Maliit::Server::AbstractSurfaceGroupFactory>(new MaliitTestUtils::TestSurfaceGroupFactory));
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

    enabledPlugins << pluginId + ":" + "dummyimsv1";
    enabledPluginsSettings.set(enabledPlugins);
    QCOMPARE(spy.count(), 1);

    description = findPluginDescriptions(subject->pluginDescriptions(Maliit::OnScreen), pluginName);
    QVERIFY(description);
    QVERIFY(description->enabled());
    description = findPluginDescriptions(subject->pluginDescriptions(Maliit::OnScreen), pluginName3);
    QVERIFY(description);
    QVERIFY(!description->enabled());
    description = 0;

    enabledPlugins << pluginId + ":" + "dummyimsv2";
    enabledPluginsSettings.set(enabledPlugins);
    QCOMPARE(spy.count(), 2);

    description = findPluginDescriptions(subject->pluginDescriptions(Maliit::OnScreen), pluginName);
    QVERIFY(description);
    QVERIFY(description->enabled());
    description = findPluginDescriptions(subject->pluginDescriptions(Maliit::OnScreen), pluginName3);
    QVERIFY(description);
    QVERIFY(!description->enabled());
    description = 0;

    enabledPlugins << pluginId3 + ":" + "dummyim3sv2";
    enabledPluginsSettings.set(enabledPlugins);
    QCOMPARE(spy.count(), 3);

    description = findPluginDescriptions(subject->pluginDescriptions(Maliit::OnScreen), pluginName);
    QVERIFY(description);
    QVERIFY(description->enabled());
    description = findPluginDescriptions(subject->pluginDescriptions(Maliit::OnScreen), pluginName3);
    QVERIFY(description);
    QVERIFY(description->enabled());
    description = 0;

    //at least we should not crash here
    enabledPluginsSettings.set(QStringList());
}

Q_DECLARE_METATYPE(QList<int>)

void Ft_MIMPluginManager::testPluginSetting()
{
    qRegisterMetaType<QList<int> >();

    QString pluginId;
    QSignalSpy spy(subject->d_ptr->sharedAttributeExtensionManager.data(),
                   SIGNAL(notifyExtensionAttributeChanged(QList<int>,int,QString,QString,QString,QVariant)));
    QList<QVariant> arguments;

    DummyImPlugin3 *plugin = 0;
    DummyInputMethod3 *inputMethod = 0;

    for (MIMPluginManagerPrivate::Plugins::iterator iterator(subject->d_ptr->plugins.begin());
            iterator != subject->d_ptr->plugins.end();
            ++iterator) {
        if (pluginName3 == iterator.key()->name()) {
            plugin = dynamic_cast<DummyImPlugin3 *>(iterator.key());
            pluginId = iterator.value().pluginId;
        }
    }

    QVERIFY(plugin != 0);
    inputMethod = dynamic_cast<DummyInputMethod3 *>(subject->d_ptr->plugins[plugin].inputMethod);
    QVERIFY(inputMethod != 0);

    QCOMPARE(inputMethod->setting->value(), QVariant("Test"));

    QString settingKey = PluginSettings + "/" + pluginId + "/setting";

    // tests
    // - that plugin settings are registered in a per-plugin namespace
    // - that they are correctly registered in the shared attribute extension manager
    // - that the data returned to clients reflects plugin settings
    // - that changing setting value notifies both subscribed clients and the plugin

    // setting registered in global setting list
    bool setting_registered = false;

    Q_FOREACH (const MImPluginSettingsInfo &info, subject->d_ptr->settings) {
        if (info.plugin_name == pluginId) {
            setting_registered = true;

            QCOMPARE(info.plugin_description, QString("DummyImPlugin3"));
            QCOMPARE(info.entries.count(), 1);
            QCOMPARE(info.entries[0].description, QString("Example setting"));
            QCOMPARE(info.entries[0].extension_key, settingKey);
            QCOMPARE(info.entries[0].type, Maliit::StringType);
            QCOMPARE(info.entries[0].attributes[Maliit::SettingEntryAttributes::defaultValue], QVariant("Test"));
            break;
        }
    }

    QVERIFY(setting_registered);

    // simulate client susbcription
    subject->d_ptr->sharedAttributeExtensionManager->handleAttributeExtensionRegistered(12, MSharedAttributeExtensionManager::PluginSettings, settingKey);

    MImSettings setting(settingKey);

    // setting the value from the server
    setting.set("Test1");

    QCOMPARE(spy.count(), 1);
    QCOMPARE(inputMethod->setting->value(), QVariant("Test1"));
    QCOMPARE(inputMethod->localSettingValue, QVariant("Test1"));

    arguments = spy[0];
    QCOMPARE(arguments[2], QVariant("/maliit"));
    QCOMPARE(arguments[3], QVariant("pluginsettings/" + pluginId));
    QCOMPARE(arguments[4], QVariant("setting"));
    QCOMPARE(arguments[5].value<QVariant>(), QVariant("Test1"));

    // setting the value from the plugin
    inputMethod->setting->set("Test2");

    QCOMPARE(spy.count(), 2);
    QCOMPARE(setting.value(), QVariant("Test2"));
    QCOMPARE(inputMethod->localSettingValue, QVariant("Test2"));

    arguments = spy[1];
    QCOMPARE(arguments[2], QVariant("/maliit"));
    QCOMPARE(arguments[3], QVariant("pluginsettings/" + pluginId));
    QCOMPARE(arguments[4], QVariant("setting"));
    QCOMPARE(arguments[5].value<QVariant>(), QVariant("Test2"));
}

QTEST_MAIN(Ft_MIMPluginManager)

