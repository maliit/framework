#ifndef UT_MIMAPPLICATION_H
#define UT_MIMAPPLICATION_H

#include <QtTest/QtTest>
#include <QObject>

class MPassThruWindow;
class MIMApplication;

class Ut_MIMApplication : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void init();
    void cleanup();

    void testHandleTransientEvents();
    void testHandleMapNotifyEvents();

private:
    void handleMessages();

    MPassThruWindow *testWidget;
    MIMApplication *app;
};

#endif
