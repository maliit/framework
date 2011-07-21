#include <QtCore/QString>
#include <QtTest/QtTest>
#include <QApplication>

#include <maliit/inputmethod.h>

#include "utils.h"

using Maliit::InputMethod;
using MaliitTestUtils::EventSpyInputContext;

namespace {
    const QRect DEFAULT_AREA;
    const QRect TEST_AREA(QPoint(100, 100), QSize(100, 100));

    const Maliit::OrientationAngle DEFAULT_ANGLE(Maliit::Angle0);
}

class Ut_Maliit_InputMethod : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void testDefaultArea();
    void testDefaultOrientationAngle();
    void testSetArea();
    void testSetArea_data();
    void testStartOrientationAngleChange();
    void testSetOrientationAngle();
    void testSetOrientationAngleSignal();
    void testSetOrientationAngleSignal_data();
    void testEmitKeyPress();
    void testEmitKeyRelease();
    void testRequestInputMethodPanel();
    void testCloseInputMethodPanel();

private:
    QApplication *app;
};

void Ut_Maliit_InputMethod::initTestCase()
{
    static char *argv[1] = { (char *) "Ut_Maliit_InputMethod" };
    static int argc = 1;

    app = new QApplication(argc, argv);
}

void Ut_Maliit_InputMethod::cleanupTestCase()
{
    delete app;
}

void Ut_Maliit_InputMethod::testDefaultArea()
{
    QCOMPARE(InputMethod::instance()->area(), DEFAULT_AREA);
}

void Ut_Maliit_InputMethod::testDefaultOrientationAngle()
{
    QCOMPARE(InputMethod::instance()->orientationAngle(), DEFAULT_ANGLE);
}

void Ut_Maliit_InputMethod::testSetArea()
{
    QFETCH(QRect, area);
    QFETCH(QRect, result);
    QFETCH(int, areaChangedCount);

    InputMethod::instance()->setArea(DEFAULT_AREA);

    QSignalSpy areaChanged(InputMethod::instance(), SIGNAL(areaChanged(QRect)));

    InputMethod::instance()->setArea(area);

    QCOMPARE(InputMethod::instance()->area(), result);

    QCOMPARE(areaChanged.size(), areaChangedCount);
    if (areaChanged.size() > 0)
        QCOMPARE(areaChanged.first().at(0).toRect(), result);
}

void Ut_Maliit_InputMethod::testSetArea_data()
{
    QTest::addColumn<QRect>("area");
    QTest::addColumn<QRect>("result");
    QTest::addColumn<int>("areaChangedCount");


    QTest::newRow("change to different area") << TEST_AREA << TEST_AREA << 1;
    QTest::newRow("change to same area (should not emit a signal)") << DEFAULT_AREA << DEFAULT_AREA << 0;
}

void Ut_Maliit_InputMethod::testStartOrientationAngleChange()
{
    InputMethod::instance()->setOrientationAngle(DEFAULT_ANGLE);

    InputMethod::instance()->startOrientationAngleChange(Maliit::Angle180);

    QCOMPARE(InputMethod::instance()->orientationAngle(), Maliit::Angle180);
}

void Ut_Maliit_InputMethod::testSetOrientationAngle()
{
    InputMethod::instance()->setOrientationAngle(DEFAULT_ANGLE);

    InputMethod::instance()->setOrientationAngle(Maliit::Angle180);

    QCOMPARE(InputMethod::instance()->orientationAngle(), Maliit::Angle180);
}

void Ut_Maliit_InputMethod::testSetOrientationAngleSignal()
{
    QFETCH(Maliit::OrientationAngle, startAngle);
    QFETCH(Maliit::OrientationAngle, setAngle);
    QFETCH(Maliit::OrientationAngle, result);
    QFETCH(int, aboutToChangeResult);
    QFETCH(int, changedResult);

    InputMethod::instance()->setOrientationAngle(DEFAULT_ANGLE);

    QSignalSpy aboutToChange(InputMethod::instance(), SIGNAL(orientationAngleAboutToChange(Maliit::OrientationAngle)));
    QSignalSpy changed(InputMethod::instance(), SIGNAL(orientationAngleChanged(Maliit::OrientationAngle)));

    InputMethod::instance()->startOrientationAngleChange(startAngle);
    InputMethod::instance()->setOrientationAngle(setAngle);

    QCOMPARE(InputMethod::instance()->orientationAngle(), result);
    QCOMPARE(aboutToChange.size(), aboutToChangeResult);
    QCOMPARE(changed.size(), changedResult);
}

void Ut_Maliit_InputMethod::testSetOrientationAngleSignal_data()
{
    QTest::addColumn<Maliit::OrientationAngle>("startAngle");
    QTest::addColumn<Maliit::OrientationAngle>("setAngle");
    QTest::addColumn<Maliit::OrientationAngle>("result");
    QTest::addColumn<int>("aboutToChangeResult");
    QTest::addColumn<int>("changedResult");

    QTest::newRow("startAngle and setAngle to the same angle") << Maliit::Angle90 << Maliit::Angle90 << Maliit::Angle90 << 1 << 1;
    QTest::newRow("just setAngle to an angle") << DEFAULT_ANGLE << Maliit::Angle90 << Maliit::Angle90 << 0 << 1;
    QTest::newRow("startAngle and setAngle to different angles") << Maliit::Angle90 << Maliit::Angle180 << Maliit::Angle180 << 1 << 1;
    QTest::newRow("startAngle and setAngle to the default angle") << DEFAULT_ANGLE  << DEFAULT_ANGLE  << DEFAULT_ANGLE << 0 << 0;
}

void Ut_Maliit_InputMethod::testEmitKeyPress()
{
    const QKeyEvent eventA(QEvent::KeyPress, Qt::Key_A, Qt::NoModifier, QString::fromLatin1("a"));
    const QKeyEvent eventZ(QEvent::KeyPress, Qt::Key_Z, Qt::ShiftModifier, QString::fromLatin1("Z"));

    QSignalSpy keyPress(InputMethod::instance(), SIGNAL(keyPress(QKeyEvent)));

    InputMethod::instance()->emitKeyPress(eventA);
    InputMethod::instance()->emitKeyPress(eventZ);

    QCOMPARE(keyPress.size(), 2);
}

void Ut_Maliit_InputMethod::testEmitKeyRelease()
{
    const QKeyEvent eventA(QEvent::KeyRelease, Qt::Key_A, Qt::NoModifier, QString::fromLatin1("a"));
    const QKeyEvent eventZ(QEvent::KeyRelease, Qt::Key_Z, Qt::ShiftModifier, QString::fromLatin1("Z"));

    QSignalSpy keyRelease(InputMethod::instance(), SIGNAL(keyRelease(QKeyEvent)));

    InputMethod::instance()->emitKeyRelease(eventA);
    InputMethod::instance()->emitKeyRelease(eventZ);

    QCOMPARE(keyRelease.size(), 2);
}

void Ut_Maliit_InputMethod::testRequestInputMethodPanel()
{
    EventSpyInputContext<QEvent::Type> *spy = new EventSpyInputContext<QEvent::Type>(std::mem_fun(&QEvent::type));

    qApp->setInputContext(spy);

    Maliit::requestInputMethodPanel();

    QCOMPARE(spy->size(), 1ul);
    QCOMPARE(spy->at(0), QEvent::RequestSoftwareInputPanel);
}

void Ut_Maliit_InputMethod::testCloseInputMethodPanel()
{
    EventSpyInputContext<QEvent::Type> *spy = new EventSpyInputContext<QEvent::Type>(std::mem_fun(&QEvent::type));

    qApp->setInputContext(spy);

    Maliit::closeInputMethodPanel();

    QCOMPARE(spy->size(), 1ul);
    QCOMPARE(spy->at(0), QEvent::CloseSoftwareInputPanel);
}

QTEST_APPLESS_MAIN(Ut_Maliit_InputMethod)

#include "ut_maliit_inputmethod.moc"
