#ifndef UT_PASSTHROUGHSERVER_H
#define UT_PASSTHROUGHSERVER_H

#include <QtTest/QtTest>
#include <QObject>

class MWindow;
class MIMApplication;
class MPassThruWindow;
class MIMWindowManager;

class Ut_PassthroughServer : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    //! Test hiding and showing of the window based on region signals
    void testHideShow();

    void testEmergencyHide();
    void testPassThruWindowManagement();

#if defined(M_IM_DISABLE_TRANSLUCENCY) && defined(M_IM_USE_SHAPE_WINDOW)
    //! Tests window shape based on region signals
    void testWindowShape();
#endif

private:
    void makeVisible();
    void makeInvisible();
    void waitForWm();

    MIMApplication *app;
    MPassThruWindow *subject;
    MIMWindowManager *wm;
};

#endif

