#include "ut_mimapplication.h"

#include <mimxapplication.h>
#include <mpassthruwindow.h>
#include <QCoreApplication>
#include <QSignalSpy>
#include <QGraphicsView>
#include <QDebug>
#include <QCommonStyle>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h> // defines IconicState

namespace {
    const WId FakeRemoteWId = 1; // must be non-zero to be considered "valid"
}

void Ut_MIMApplication::initTestCase()
{
    qRegisterMetaType<MImRemoteWindow*>("MImRemoteWindow *");

#if defined(Q_WS_X11)
    xOptions.selfComposited = false;
#endif
}

void Ut_MIMApplication::cleanupTestCase()
{
}

void Ut_MIMApplication::init()
{
    static char *argv[1] = { (char *) "ut_mimapplication" };
    static int argc = 1;

    // prevent loading of QtMaemo6Style, because it could break our tests
    MImXApplication::setStyle(new QCommonStyle);

    app = new MImXApplication(argc, argv, xOptions);
    subject = static_cast<MPassThruWindow *>(app->passThruWindow());

    app->setTransientHint(FakeRemoteWId);
}

void Ut_MIMApplication::cleanup()
{
    delete app;

#if defined(Q_WS_X11)
    xOptions.selfComposited = false;
#endif
}

void Ut_MIMApplication::testHandleTransientEvents()
{
    XEvent xevent;
    xevent.type = UnmapNotify;
    xevent.xunmap.event = FakeRemoteWId;

    QSignalSpy spy(app, SIGNAL(remoteWindowChanged(MImRemoteWindow*)));
    app->x11EventFilter(&xevent);
    QCOMPARE(spy.count(), 1);
}


void Ut_MIMApplication::testConfigureWidgetsForCompositing_data()
{
    QTest::addColumn<bool>("selfCompositing");

#if defined(Q_WS_X11)
    QTest::newRow("selfComposting enabled") << true;
#endif

    QTest::newRow("selfComposting disabled") << false;
}

void Ut_MIMApplication::testConfigureWidgetsForCompositing()
{
    QFETCH(bool, selfCompositing);

    // destroy and recreate application object with new options
    cleanup();
#if defined(Q_WS_X11)
    xOptions.selfComposited = selfCompositing;
#endif
    init();

    QWidget mainWindow(app->passThruWindow());
    QGraphicsView view(&mainWindow);

    QList<QWidget *> widgets;
    widgets << &mainWindow << &view << view.viewport();

    widgets << app->passThruWindow();

    app->configureWidgetsForCompositing();
    Q_FOREACH (QWidget *w, widgets) {
        QVERIFY(w->testAttribute(Qt::WA_NoSystemBackground));
        QVERIFY(w->testAttribute(Qt::WA_OpaquePaintEvent));
        QVERIFY(w->testAttribute(Qt::WA_TranslucentBackground) != selfCompositing);
        QVERIFY(not w->autoFillBackground());
    }
}

QTEST_APPLESS_MAIN(Ut_MIMApplication)

