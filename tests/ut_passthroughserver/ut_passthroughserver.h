#ifndef UT_PASSTHROUGHSERVER_H
#define UT_PASSTHROUGHSERVER_H

#include "mimserveroptions.h"

#include <QtTest/QtTest>
#include <QObject>

class MImXApplication;
class MPassThruWindow;
class MImXServerLogic;

class Ut_PassthroughServer : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    //! Test hiding and showing of the window based on region signals
    void testHideShow();

    void testEmergencyHide();

private:
    void makeVisible();
    void makeInvisible();

    MImXApplication *app;
    MImXServerLogic *serverLogic;
    MPassThruWindow *subject;
    MImServerXOptions xOptions;
};

#endif
