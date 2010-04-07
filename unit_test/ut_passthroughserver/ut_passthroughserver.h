#ifndef UT_PASSTHROUGHSERVER_H
#define UT_PASSTHROUGHSERVER_H

#include <QtTest/QtTest>
#include <QObject>

class DuiWindow;
class DuiApplication;
class DuiPassThruWindow;

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

#if defined(DUI_IM_DISABLE_TRANSLUCENCY) && defined(DUI_IM_USE_SHAPE_WINDOW)
    //! Tests window shape based on region signals
    void testWindowShape();
#endif

private:
    DuiApplication *app;
    DuiPassThruWindow *subject;
};

#endif

