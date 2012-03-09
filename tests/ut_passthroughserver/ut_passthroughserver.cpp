#include "ut_passthroughserver.h"
#include "mpassthruwindow.h"
#include "mimxapplication.h"

#include <QCommonStyle>

void Ut_PassthroughServer::initTestCase()
{
    static int argc = 1;
    static char *app_name[1] = { (char *) "ut_passthroughserver" };

    app = new MImXApplication(argc, app_name, xOptions);
    serverLogic = app->serverLogic();

    app->setStyle(new QCommonStyle);
    serverLogic->applicationFocusChanged(1); // remote win id should be non-zero
}

void Ut_PassthroughServer::cleanupTestCase()
{
    delete app;
    app = 0;
}

void Ut_PassthroughServer::init()
{
    subject = qobject_cast<MPassThruWindow *>(serverLogic->passThruWindow());
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

    Q_EMIT serverLogic->remoteWindowChanged(0);

    QVERIFY(!subject->testAttribute(Qt::WA_Mapped) && !subject->isVisible());
}

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
