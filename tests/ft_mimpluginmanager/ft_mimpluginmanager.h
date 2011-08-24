#ifndef FT_MIMPLUGINLOADER_H
#define FT_MIMPLUGINLOADER_H

#include <QtTest/QtTest>
#include <QObject>


class MIMApplication;
class MIMPluginManager;
class QGraphicsScene;

class Ft_MIMPluginManager : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void init();
    void cleanup();

    void testLoadPlugins();
    void testSwitchPluginState();
    void testMultiplePlugins();
    void testSwitchPluginBySignal();
    void testSwitchToSpecifiedPlugin();
    void testPluginDescriptions();

private:
    MIMApplication *app;
    QString pluginPath;
    MIMPluginManager *subject;
};

#endif
