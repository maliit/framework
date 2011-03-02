#include "ut_passthroughserver.h"
#include "mpassthruwindow.h"
#include "mimapplication.h"

#include <QCommonStyle>

void Ut_PassthroughServer::initTestCase()
{
    static int argc = 1;
    static char *app_name[1] = { (char *) "ut_passthroughserver" };

    app = new MIMApplication(argc, app_name);
    app->setStyle(new QCommonStyle);
    app->setTransientHint(1); // remote win id should be non-zero
}

void Ut_PassthroughServer::cleanupTestCase()
{
    delete app;
    app = 0;
}

void Ut_PassthroughServer::init()
{
    subject = static_cast<MPassThruWindow *>(app->passThruWindow());
}

void Ut_PassthroughServer::cleanup()
{}

void Ut_PassthroughServer::testHideShow()
{
    makeVisible();
    makeInvisible();
}

void Ut_PassthroughServer::testEmergencyHide()
{
    makeVisible();

    emit app->remoteWindowGone();

    QVERIFY(!subject->testAttribute(Qt::WA_Mapped) && !subject->isVisible());
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

void Ut_PassthroughServer::makeVisible()
{
    QVERIFY(!subject->testAttribute(Qt::WA_Mapped) && !subject->isVisible());

    // Should be shown after non-empty region update
    subject->inputPassthrough(QRegion(0, 0, 100, 100));
    QTest::qWaitForWindowShown(subject);

    QVERIFY(subject->testAttribute(Qt::WA_Mapped) && subject->isVisible());

}

void Ut_PassthroughServer::makeInvisible()
{
    // Should be hidden after empty region update
    subject->inputPassthrough(QRegion());

    QVERIFY(!subject->testAttribute(Qt::WA_Mapped) && !subject->isVisible());
}

QTEST_APPLESS_MAIN(Ut_PassthroughServer);
