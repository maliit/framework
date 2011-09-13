#ifndef UT_MIMPLUGINLOADER_H
#define UT_MIMPLUGINLOADER_H

#include <QtTest/QtTest>
#include <QObject>

class MIMApplication;
class MIMPluginManager;
class MIMPluginManagerPrivate;
class QCoreApplication;
class QDBusInterface;

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
    void testConvertAndFilterHandlers_data();
    void testConvertAndFilterHandlers();
    void testSwitchPluginState();
    void testMultiplePlugins();
    void testExistInputMethod();

    void testPluginSwitcher_data();
    void testPluginSwitcher();

    void testSwitchToSpecifiedPlugin();

    void testSetActivePlugin();

    void testSubViews();
    void testActiveSubView();

    void testDBusQueryCalls();
    void testDBusSetCalls();

    void testRegionUpdates();
    void testSetToolbar();

    void testLoadedPluginsInfo_data();
    void testLoadedPluginsInfo();

    void testSubViewsInfo_data();
    void testSubViewsInfo();

private:
    void handleMessages();

    MIMApplication *app;
    QWidget *proxyWidget;
    QString pluginPath;
    MIMPluginManager *manager;
    MIMPluginManagerPrivate *subject;
    QDBusInterface *m_clientInterface;

    void checkHandlerMap(int handler, const QString &name);
};

#endif
