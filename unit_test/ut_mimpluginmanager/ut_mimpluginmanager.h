#ifndef UT_MIMPLUGINLOADER_H
#define UT_MIMPLUGINLOADER_H

#include <QtTest/QtTest>
#include <QObject>


class MIMPluginManager;
class MIMPluginManagerPrivate;
class QCoreApplication;
class QDBusInterface;

class Ut_MIMPluginManager : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void init();
    void cleanup();

    void testLoadPlugins();
    void testAddHandlerMap();
    void testConvertAndFilterHandlers_data();
    void testConvertAndFilterHandlers();
    void testSwitchPluginState();
    void testMultilePlugins();
    void testExistInputMethod();

    void testPluginSwitcher_data();
    void testPluginSwitcher();

    void testSwitchToSpecifiedPlugin();

    void testSetActivePlugin();

    void testSubViews();
    void testActiveSubView();

    void testDBusQueryCalls();
    void testDBusSetCalls();
private:
    void handleMessages();

    QCoreApplication *app;
    QString pluginPath;
    MIMPluginManager *manager;
    MIMPluginManagerPrivate *subject;
    QDBusInterface *m_clientInterface;

    void checkHandlerMap(int handler, const QString &name);
};

#endif
