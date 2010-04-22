#ifndef UT_MIMPLUGINLOADER_H
#define UT_MIMPLUGINLOADER_H

#include <QtTest/QtTest>
#include <QObject>


class MIMPluginManagerPrivate;
class QCoreApplication;

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
    void testFreeInputMethod();

    void testPluginSwitcher_data();
    void testPluginSwitcher();

    void testSwitchToSpecifiedPlugin();

private:
    QCoreApplication *app;
    QString pluginPath;
    MIMPluginManagerPrivate *subject;

    void checkHandlerMap(int handler, const QString &name);
};

#endif
