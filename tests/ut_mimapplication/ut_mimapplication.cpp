#include "ut_mimapplication.h"

#include <mimxapplication.h>
#include <mpassthruwindow.h>
#include <QCoreApplication>
#include <QSignalSpy>
#include <QGraphicsView>
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

    app = new MImXApplication(argc, argv, xOptions);
    serverLogic = app->serverLogic();

    subject = static_cast<MPassThruWindow *>(serverLogic->passThruWindow());
}

void Ut_MIMApplication::cleanupTestCase()
{
    delete app;
}

void Ut_MIMApplication::init()
{
    xOptions.selfComposited = false;
    serverLogic->applicationFocusChanged(FakeRemoteWId);
}

void Ut_MIMApplication::cleanup()
{
}

void Ut_MIMApplication::testHandleTransientEvents()
{
    XEvent xevent;
    xevent.type = UnmapNotify;
    xevent.xunmap.event = FakeRemoteWId;

    QSignalSpy spy(serverLogic, SIGNAL(remoteWindowChanged(MImRemoteWindow*)));
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
    QFETCH(bool, selfCompositing);

    xOptions.selfComposited = selfCompositing;

    QWidget mainWindow(subject);
    QGraphicsView view(&mainWindow);

    QList<QWidget *> widgets;
    widgets << &mainWindow << &view << view.viewport();

    widgets << subject;

    serverLogic->pluginLoaded();

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

QTEST_APPLESS_MAIN(Ut_MIMApplication)

