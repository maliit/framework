#ifndef UT_MIMSETTINGDIALOG_H
#define UT_MIMSETTINGDIALOG_H

#include <QtTest/QtTest>
#include <QObject>

class MApplication;
class MIMSettingsDialog;
class MIMPluginManager;
class MPlainWindow;

class Ut_MIMSettingsDialog : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void init();
    void cleanup();

    void testShowAvailableSubViewList();
    void testSetActiveView();
    void testRetranslateUi();
    void testHandleDialogDisappeared();

private:
    MApplication *app;
    QString pluginPath;
    MIMSettingsDialog *subject;
    MIMPluginManager *pluginMgr;
};

#endif

