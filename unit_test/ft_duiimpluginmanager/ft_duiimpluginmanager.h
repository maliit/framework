#ifndef FT_DUIIMPLUGINLOADER_H
#define FT_DUIIMPLUGINLOADER_H

#include <QtTest/QtTest>
#include <QObject>


class QApplication;
class DuiIMPluginManager;
class QGraphicsScene;

class Ft_DuiIMPluginManager : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void init();
    void cleanup();

    void testLoadPlugins();
    void testSwitchPlugin();
    void testMultilePlugins();
    void testFreeInputMethod();

private:
    QApplication *app;
    QString pluginPath;
    DuiIMPluginManager *subject;
};

#endif
