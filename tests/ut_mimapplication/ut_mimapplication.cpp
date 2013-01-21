#include "ut_mimapplication.h"

#include <mimxapplication.h>
#include <mpassthruwindow.h>
#include <QCoreApplication>
#include <QSignalSpy>
#include <QGraphicsView>
#include <QDebug>
#include <QX11Info>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h> // defines IconicState

namespace {
    const WId FakeRemoteWId = 1; // must be non-zero to be considered "valid"
}


void Ut_MIMApplication::init()
{
    static char *argv[1] = { (char *) "ut_mimapplication" };
    static int argc = 1;

    app = new MImXApplication(argc, argv);
    subject = static_cast<MPassThruWindow *>(app->passThruWindow());
}

void Ut_MIMApplication::cleanup()
{
    delete app;
    app = 0;
}

void Ut_MIMApplication::testHandleTransientEvents()
{
    app->setTransientHint(FakeRemoteWId);

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
    QTest::newRow("selfComposting enabled") << true;
    QTest::newRow("selfComposting disabled") << false;
}

void Ut_MIMApplication::testConfigureWidgetsForCompositing()
{
    app->setTransientHint(FakeRemoteWId);

    QFETCH(bool, selfCompositing);

    QWidget mainWindow(app->passThruWindow());
    QGraphicsView view(&mainWindow);

    QList<QWidget *> widgets;
    widgets << &mainWindow << &view << view.viewport();

    widgets << app->passThruWindow();

    app->configureWidgetsForCompositing();

    Q_FOREACH (QWidget *w, widgets) {
        QVERIFY(w->testAttribute(Qt::WA_NoSystemBackground)
                || not selfCompositing);
        QVERIFY(w->testAttribute(Qt::WA_OpaquePaintEvent)
                || not selfCompositing);
        QVERIFY(w->testAttribute(Qt::WA_TranslucentBackground)
                || selfCompositing);
        QVERIFY(not w->autoFillBackground());
    }
}

void Ut_MIMApplication::testHandleTransient()
{
    // simulate first focus when passthru gets shown only after app has signaled on new focus target
    QWidget dummyWindow;
    dummyWindow.show();
    app->setTransientHint(dummyWindow.effectiveWinId());

    QWidget *passthru = app->passThruWindow();
    passthru->show();
    int passthruWinId = passthru->effectiveWinId();

    WId transientTarget;
    XGetTransientForHint(QX11Info::display(), passthruWinId, &transientTarget);
    QCOMPARE(transientTarget, dummyWindow.effectiveWinId());
}

QTEST_APPLESS_MAIN(Ut_MIMApplication)

