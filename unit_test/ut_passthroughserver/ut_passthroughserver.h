#ifndef UT_PASSTHROUGHSERVER_H
#define UT_PASSTHROUGHSERVER_H

#include <QtTest/QtTest>
#include <QObject>

class MWindow;
class MApplication;
class MPassThruWindow;

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

#if defined(M_IM_DISABLE_TRANSLUCENCY) && defined(M_IM_USE_SHAPE_WINDOW)
    //! Tests window shape based on region signals
    void testWindowShape();
#endif

private:
    MApplication *app;
    MPassThruWindow *subject;
};

#endif

