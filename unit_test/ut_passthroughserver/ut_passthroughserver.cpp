#include "ut_passthroughserver.h"
#include "mpassthruwindow.h"
#include "mplainwindow.h"

#include <MApplication>
#include <MWindow>

void Ut_PassthroughServer::initTestCase()
{
    static int argc = 1;
    static char *app_name[1] = { (char *)"ut_passthroughserver" };

    MApplication::setLoadMInputContext(false);
    app = new MApplication(argc, app_name);
    new MPlainWindow;
}

void Ut_PassthroughServer::cleanupTestCase()
{
    delete MPlainWindow::instance();
    delete app;
    app = 0;
}

void Ut_PassthroughServer::init()
{
    subject = new MPassThruWindow;
}

void Ut_PassthroughServer::cleanup()
{
    delete subject;
    subject = 0;
}

void Ut_PassthroughServer::testHideShow()
{
    // Should be hidden initially
    QVERIFY(!subject->isVisible());

    // Should be shown after non-empty region update
    subject->inputPassthrough(QRegion(0, 0, 100, 100));
    QVERIFY(subject->isVisible());

    // Should be hidden again after empty region update
    subject->inputPassthrough(QRegion());
    QVERIFY(!subject->isVisible());
}

#if defined(M_IM_DISABLE_TRANSLUCENCY) && defined(M_IM_USE_SHAPE_WINDOW)
void Ut_PassthroughServer::testWindowShape()
{
    const QRegion region(0, 10, 100, 200);

    QCOMPARE(QRegion(), subject->mask());

    subject->inputPassthrough(region);
    QCOMPARE(region, subject->mask());
}
#endif


QTEST_APPLESS_MAIN(Ut_PassthroughServer);

