#ifndef UT_MIMPLUGINLOADER_H
#define UT_MIMPLUGINLOADER_H

#include "mimserveroptions.h"

#include <QtTest/QtTest>
#include <QObject>

class MIMPluginManager;
class MIMPluginManagerPrivate;
class MInputContextTestConnection;
QT_BEGIN_NAMESPACE
class QDBusInterface;
QT_END_NAMESPACE

class Ut_MIMPluginManager : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void init();
    void cleanup();

    void testLoadPlugins();
    void testAddHandlerMap();
    void testSwitchPluginState();
    void testMultiplePlugins();
    void testExistInputMethod();

    void testPluginSwitcher_data();
    void testPluginSwitcher();

    void testSwitchToSpecifiedPlugin();

    void testSwitchShow_data();
    void testSwitchShow();

    void testSetActivePlugin();

    void testSubViews();
    void testActiveSubView();

    void testLoadedPluginsInfo_data();
    void testLoadedPluginsInfo();

    void testSubViewsInfo_data();
    void testSubViewsInfo();

    void testEnableAllSubviews();

    void testPluginSettingsList();
    void testPluginSettingsUpdate();

private:
    void handleMessages();

    QString pluginPath;
    MIMPluginManager *manager;
    MIMPluginManagerPrivate *subject;
    MInputContextTestConnection *connection;
    void checkHandlerMap(int handler, const QString &name);
};

#endif
