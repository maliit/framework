#include "ut_mimapplication.h"

#include <mimapplication.h>
#include <mplainwindow.h>
#include <mpassthruwindow.h>
#include <QCoreApplication>
#include <QSignalSpy>
#include <QDebug>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h> // defines IconicState
            

void Ut_MIMApplication::initTestCase()
{
    static char *argv[1] = { (char *) "ut_mimapplication" };
    static int argc = 1;

    MApplication::setLoadMInputContext(false);
    app = new MIMApplication(argc, argv);

    testWidget = new MPassThruWindow(true);
    testWidget->setFocusPolicy(Qt::NoFocus);
    app->setPassThruWindow(testWidget);

    new MPlainWindow(testWidget);
}

void Ut_MIMApplication::cleanupTestCase()
{
    delete testWidget;
    delete MPlainWindow::instance();
    delete app;
}

void Ut_MIMApplication::init()
{
    app->setTransientHint(testWidget->effectiveWinId());
}

void Ut_MIMApplication::cleanup()
{
}

void Ut_MIMApplication::testHandleTransientEvents()
{
    XEvent xevent;
    xevent.type = UnmapNotify;
    xevent.xunmap.event = testWidget->effectiveWinId();

    QSignalSpy spy(app, SIGNAL(remoteWindowGone()));
    app->x11EventFilter(&xevent);
    QCOMPARE(spy.count(), 1);
}

void Ut_MIMApplication::testHandleMapNotifyEvents()
{
    XEvent xevent;
    xevent.type = MapNotify;
    xevent.xmap.event = testWidget->effectiveWinId();

    QSignalSpy mapSpy(app, SIGNAL(passThruWindowMapped()));
    app->x11EventFilter(&xevent);
    QCOMPARE(mapSpy.count(), 1);

    xevent.type = UnmapNotify;
    xevent.xunmap.event = testWidget->effectiveWinId();

    QSignalSpy unmapSpy(app, SIGNAL(passThruWindowUnmapped()));
    app->x11EventFilter(&xevent);
    QCOMPARE(unmapSpy.count(), 1);
}

QTEST_APPLESS_MAIN(Ut_MIMApplication)

