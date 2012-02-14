#ifndef UT_PASSTHROUGHSERVER_H
#define UT_PASSTHROUGHSERVER_H

#include "mimserveroptions.h"

#include <QtTest/QtTest>
#include <QObject>

class MWindow;
class MImXApplication;
class MPassThruWindow;

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

#if defined(M_IM_DISABLE_TRANSLUCENCY) && defined(M_IM_USE_SHAPE_WINDOW)
    //! Tests window shape based on region signals
    void testWindowShape();
#endif

private:
    void makeVisible();
    void makeInvisible();

    MImXApplication *app;
    MPassThruWindow *subject;
    MImServerXOptions xOptions;
};

#endif
