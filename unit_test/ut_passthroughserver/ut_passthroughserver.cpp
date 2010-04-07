#include "ut_passthroughserver.h"
#include "duipassthruwindow.h"
#include "duiplainwindow.h"

#include <DuiApplication>
#include <DuiWindow>

void Ut_PassthroughServer::initTestCase()
{
    static int argc = 1;
    static char *app_name[1] = { (char *)"ut_passthroughserver" };

    DuiApplication::setLoadDuiInputContext(false);
    app = new DuiApplication(argc, app_name);
    new DuiPlainWindow;
}

void Ut_PassthroughServer::cleanupTestCase()
{
    delete DuiPlainWindow::instance();
    delete app;
    app = 0;
}

void Ut_PassthroughServer::init()
{
    subject = new DuiPassThruWindow;
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

#if defined(DUI_IM_DISABLE_TRANSLUCENCY) && defined(DUI_IM_USE_SHAPE_WINDOW)
void Ut_PassthroughServer::testWindowShape()
{
    const QRegion region(0, 10, 100, 200);

    QCOMPARE(QRegion(), subject->mask());

    subject->inputPassthrough(region);
    QCOMPARE(region, subject->mask());
}
#endif


QTEST_APPLESS_MAIN(Ut_PassthroughServer);

