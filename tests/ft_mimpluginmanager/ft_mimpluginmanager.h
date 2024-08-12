#ifndef FT_MIMPLUGINLOADER_H
#define FT_MIMPLUGINLOADER_H

#include <QtTest/QtTest>
#include <QObject>

class MIMPluginManager;
QT_BEGIN_NAMESPACE
class QGraphicsScene;
QT_END_NAMESPACE

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
    MIMPluginManager *subject;
};

#endif
