#ifndef UT_DUIIMPLUGINLOADER_H
#define UT_DUIIMPLUGINLOADER_H

#include <QtTest/QtTest>
#include <QObject>


class DuiIMPluginManagerPrivate;
class QCoreApplication;

class Ut_DuiIMPluginManager : public QObject
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
    void testSwitchPlugin();
    void testMultilePlugins();
    void testFreeInputMethod();

private:
    QCoreApplication *app;
    QString pluginPath;
    DuiIMPluginManagerPrivate *subject;
};

#endif
