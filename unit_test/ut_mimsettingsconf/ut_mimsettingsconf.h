#ifndef UT_MIMSETTINGSCONF_H
#define UT_MIMSETTINGSCONF_H

#include <QtTest/QtTest>
#include <QObject>


class MIMPluginManager;
class MImSettingsConf;
class QCoreApplication;

class Ut_MIMSettingsConf : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void init();
    void cleanup();

    void testPlugins();
    void testSetActivePlugin();
    void testSubViews();
    void testActiveSubView();

private:
    void handleMessages();

    QCoreApplication *app;
    QString pluginPath;
    MIMPluginManager *manager;
};

#endif
