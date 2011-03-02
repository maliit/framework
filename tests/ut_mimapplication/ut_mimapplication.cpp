#include "ut_mimapplication.h"

#include <mimapplication.h>
#include <mpassthruwindow.h>
#include <QCoreApplication>
#include <QSignalSpy>
#include <QDebug>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h> // defines IconicState

namespace {
    const WId FakeRemoteWId = 1; // must be non-zero to be considered "valid"
}

void Ut_MIMApplication::initTestCase()
{
    static char *argv[1] = { (char *) "ut_mimapplication" };
    static int argc = 1;

#ifdef HAVE_MEEGOTOUCH
    MApplication::setLoadMInputContext(false);
#endif

    app = new MIMApplication(argc, argv);
    subject = static_cast<MPassThruWindow *>(app->passThruWindow());
}

void Ut_MIMApplication::cleanupTestCase()
{
    delete app;
}

void Ut_MIMApplication::init()
{
    app->setTransientHint(FakeRemoteWId);
}

void Ut_MIMApplication::cleanup()
{
}

void Ut_MIMApplication::testHandleTransientEvents()
{
    XEvent xevent;
    xevent.type = UnmapNotify;
    xevent.xunmap.event = FakeRemoteWId;

    QSignalSpy spy(app, SIGNAL(remoteWindowGone()));
    app->x11EventFilter(&xevent);
    QCOMPARE(spy.count(), 1);
}

void Ut_MIMApplication::testHandleMapNotifyEvents()
{
    XEvent xevent;
    xevent.type = MapNotify;
    xevent.xmap.event = subject->effectiveWinId();

    QSignalSpy mapSpy(app, SIGNAL(passThruWindowMapped()));
    app->x11EventFilter(&xevent);
    QCOMPARE(mapSpy.count(), 1);

    xevent.type = UnmapNotify;
    xevent.xunmap.event = subject->effectiveWinId();

    QSignalSpy unmapSpy(app, SIGNAL(passThruWindowUnmapped()));
    app->x11EventFilter(&xevent);
    QCOMPARE(unmapSpy.count(), 1);
}

QTEST_APPLESS_MAIN(Ut_MIMApplication)

