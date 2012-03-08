#ifndef UT_MIMAPPLICATION_H
#define UT_MIMAPPLICATION_H

#include "mimserveroptions.h"

#include <QtTest/QtTest>
#include <QObject>

class MPassThruWindow;
class MImXApplication;
class MImXServerLogic;

class Ut_MIMApplication : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void init();
    void cleanup();

    void testHandleTransientEvents();

    void testConfigureWidgetsForCompositing_data();
    void testConfigureWidgetsForCompositing();

private:
    void handleMessages();

    MPassThruWindow *subject;
    MImXApplication *app;
    MImServerXOptions xOptions;
    MImXServerLogic *serverLogic;
};

#endif
