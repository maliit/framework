#include "ut_mimpluginmanagerconfig.h"

#include "dummyimplugin.h"
#include "dummyimplugin3.h"
#include "dummyinputmethod.h"
#include "dummyinputmethod3.h"

#include "minputcontextconnection.h"
#include "mimsettingsqsettings.h"

#include "core-utils.h"

#include <QTest>
#include <QProcess>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QWidget>
#include <QRegExp>
#include <QCoreApplication>
#include <QPointer>
#include <QTimer>
#include <QEventLoop>
#include <QStringList>
#include <mimpluginmanager.h>
#include <mimpluginmanager_p.h>
#include <maliit/plugins/inputmethodplugin.h>

#include "mattributeextensionmanager.h"
#include "msharedattributeextensionmanager.h"

using namespace std::tr1;

typedef QSet<Maliit::HandlerState> HandlerStates;
Q_DECLARE_METATYPE(HandlerStates);
Q_DECLARE_METATYPE(Maliit::HandlerState);

namespace {
    const QString Organization = "maliit.org";
    const QString Application = "server-tests";

    const QString ConfigRoot = MALIIT_CONFIG_ROOT;
    const QString MImPluginPaths = ConfigRoot + "paths";

    const QString EnabledPluginsKey = MALIIT_CONFIG_ROOT"onscreen/enabled";
    const QString ActivePluginKey = MALIIT_CONFIG_ROOT"onscreen/active";

    const QString pluginName  = "DummyImPlugin";
    const QString pluginName2 = "DummyImPlugin2";
    const QString pluginName3 = "DummyImPlugin3";
    const QString pluginId = "libdummyimplugin.so";
    const QString pluginId2 = "libdummyimplugin2.so";
    const QString pluginId3 = "libdummyimplugin3.so";

    const QStringList DefaultEnabledPlugins = QStringList()
                                              << pluginId + ":" + "dummyimsv1"
                                              << pluginId + ":" + "dummyimsv2"
                                              << pluginId3 + ":" + "dummyim3sv1"
                                              << pluginId3 + ":" + "dummyim3sv2";
}


void Ut_MIMPluginManagerConfig::initTestCase()
{
    MImSettings::setPreferredSettingsType(MImSettings::TemporarySettings);
    MImSettings::setImplementationFactory(new MImSettingsQSettingsBackendFactory(Organization, Application));
}

void Ut_MIMPluginManagerConfig::cleanupTestCase()
{
}

void Ut_MIMPluginManagerConfig::init()
{
    MImSettings pathConf(MImPluginPaths);
    pathConf.set(MaliitTestUtils::getTestPluginPath());
    connection = new MInputContextConnection;
    enabledPluginSettings = new MImSettings(EnabledPluginsKey);
    activePluginSettings = new MImSettings(ActivePluginKey);
}

void Ut_MIMPluginManagerConfig::cleanup()
{
    delete manager;
    delete connection;
    delete enabledPluginSettings;
    delete activePluginSettings;
    manager = 0;
    subject = 0;
}


// Test methods..............................................................

void Ut_MIMPluginManagerConfig::testNoActiveSubView()
{
    enabledPluginSettings->set(DefaultEnabledPlugins);
    activePluginSettings->unset();

    shared_ptr<MInputContextConnection> icConnection(connection);
    manager = new MIMPluginManager(icConnection, QSharedPointer<Maliit::Server::AbstractSurfaceGroupFactory>(new MaliitTestUtils::TestSurfaceGroupFactory));
    subject = manager->d_ptr;

    // The first enabled subview should have been auto-selected as the
    // active subview
    MImOnScreenPlugins::SubView subView = subject->onScreenPlugins.activeSubView();
    QCOMPARE(subView.plugin + ":" + subView.id, DefaultEnabledPlugins.first());
}

void Ut_MIMPluginManagerConfig::testEmptyConfig()
{
    enabledPluginSettings->unset();
    activePluginSettings->unset();

    shared_ptr<MInputContextConnection> icConnection(connection);
    manager = new MIMPluginManager(icConnection, QSharedPointer<Maliit::Server::AbstractSurfaceGroupFactory>(new MaliitTestUtils::TestSurfaceGroupFactory));
    subject = manager->d_ptr;

    // One subview should have been auto-activated and enabled.
    MImOnScreenPlugins& plugins = subject->onScreenPlugins;
    MImOnScreenPlugins::SubView active = plugins.activeSubView();
    QList<MImOnScreenPlugins::SubView> enabled = plugins.enabledSubViews(active.plugin);

    QCOMPARE(enabled.size(), 1);
    QCOMPARE(enabled.first(), active);
}

void Ut_MIMPluginManagerConfig::autoLanguageSubView()
{
    // Force autodetection of enabled plugins in DummyPlugin3
    enabledPluginSettings->unset();
    activePluginSettings->set(QStringList() << pluginId3 + ":");

    shared_ptr<MInputContextConnection> icConnection(connection);
    manager = new MIMPluginManager(icConnection, QSharedPointer<Maliit::Server::AbstractSurfaceGroupFactory>(new MaliitTestUtils::TestSurfaceGroupFactory));
    subject = manager->d_ptr;

    MImOnScreenPlugins& plugins = subject->onScreenPlugins;
    QList<MImOnScreenPlugins::SubView> enabled = plugins.enabledSubViews(pluginId3);

    // Expect 3 results; ur_PK should have been skipped (no match)
    QCOMPARE(enabled.size(), 3);

    // First one should be en_gb - simple match
    QCOMPARE(enabled[0].id, QString("en_gb"));

    // Check that we matched after stripping the country code
    QCOMPARE(enabled[1].id, QString("es"));

    // Check that we matched after auto-adding the country code
    QCOMPARE(enabled[2].id, QString("fr_fr"));
}

int main()
{
    // Provide our own main function so that we can override LANGUAGE
    // before Qt gets loaded.
    setenv("LANGUAGE", "en_GB.utf8:ur_PK.utf8:es_NI.utf8:fr", 1);

    Ut_MIMPluginManagerConfig test;
    QTest::qExec(&test);
    return 0;
}
