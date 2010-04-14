#ifndef FT_MIMPLUGINLOADER_H
#define FT_MIMPLUGINLOADER_H

#include <QtTest/QtTest>
#include <QObject>


class QApplication;
class MIMPluginManager;
class QGraphicsScene;

class Ft_MIMPluginManager : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void init();
    void cleanup();

    void testLoadPlugins();
    void testSwitchPlugin();
    void testMultiplePlugins();
    void testFreeInputMethod();

private:
    QApplication *app;
    QString pluginPath;
    MIMPluginManager *subject;
};

#endif
