/* * This file is part of meego-im-framework *
 *
 * Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * Contact: Nokia Corporation (directui@nokia.com)
 *
 * If you have questions regarding the use of this file, please contact
 * Nokia at directui@nokia.com.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 */

#include <QtCore/QString>
#include <QtTest/QtTest>
#include <QApplication>
#include <QScopedPointer>

#include <maliit/inputmethod.h>

#include "utils.h"

using Maliit::InputMethod;
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
using MaliitTestUtils::EventSpyInputContext;
#endif

namespace {
    const QRect DefaultArea;
    const QRect TestArea(QPoint(100, 100), QSize(100, 100));

    const Maliit::OrientationAngle DefaultAngle(Maliit::Angle0);

    void resetInputMethodToDefault() {
        InputMethod::instance()->setArea(DefaultArea);
        InputMethod::instance()->setOrientationAngle(DefaultAngle);
    }
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
    QScopedPointer<QApplication> app;
};

void Ut_Maliit_InputMethod::initTestCase()
{
    static char *argv[1] = { (char *) "Ut_Maliit_InputMethod" };
    static int argc = 1;

    app.reset(new QApplication(argc, argv));
}

void Ut_Maliit_InputMethod::cleanupTestCase()
{
    app.reset();
}

void Ut_Maliit_InputMethod::testDefaultArea()
{
    QCOMPARE(InputMethod::instance()->area(), DefaultArea);
}

void Ut_Maliit_InputMethod::testDefaultOrientationAngle()
{
    QCOMPARE(InputMethod::instance()->orientationAngle(), DefaultAngle);
}

void Ut_Maliit_InputMethod::testSetArea()
{
    resetInputMethodToDefault();
    QFETCH(QRect, area);
    QFETCH(QRect, expectedArea);
    QFETCH(int, areaChangedCount);

    QSignalSpy areaChanged(InputMethod::instance(), SIGNAL(areaChanged(QRect)));
    InputMethod::instance()->setArea(area);

    QCOMPARE(InputMethod::instance()->area(), expectedArea);
    QCOMPARE(areaChanged.size(), areaChangedCount);
    if (areaChanged.size() > 0)
        QCOMPARE(areaChanged.first().at(0).toRect(), expectedArea);
}

void Ut_Maliit_InputMethod::testSetArea_data()
{
    QTest::addColumn<QRect>("area");
    QTest::addColumn<QRect>("expectedArea");
    QTest::addColumn<int>("areaChangedCount");

    QTest::newRow("change to different area") << TestArea << TestArea << 1;
    QTest::newRow("change to same area (should not Q_EMIT a signal)") << DefaultArea << DefaultArea << 0;
}

void Ut_Maliit_InputMethod::testStartOrientationAngleChange()
{
    resetInputMethodToDefault();
    InputMethod::instance()->startOrientationAngleChange(Maliit::Angle180);

    QCOMPARE(InputMethod::instance()->orientationAngle(), Maliit::Angle180);
}

void Ut_Maliit_InputMethod::testSetOrientationAngle()
{
    resetInputMethodToDefault();
    InputMethod::instance()->setOrientationAngle(Maliit::Angle180);

    QCOMPARE(InputMethod::instance()->orientationAngle(), Maliit::Angle180);
}

void Ut_Maliit_InputMethod::testSetOrientationAngleSignal()
{
    resetInputMethodToDefault();
    QFETCH(Maliit::OrientationAngle, startAngle);
    QFETCH(Maliit::OrientationAngle, setAngle);
    QFETCH(Maliit::OrientationAngle, expectedAngle);
    QFETCH(int, aboutToChangeCount);
    QFETCH(int, changedCount);

    QSignalSpy aboutToChange(InputMethod::instance(), SIGNAL(orientationAngleAboutToChange(Maliit::OrientationAngle)));
    QSignalSpy changed(InputMethod::instance(), SIGNAL(orientationAngleChanged(Maliit::OrientationAngle)));

    InputMethod::instance()->startOrientationAngleChange(startAngle);
    InputMethod::instance()->setOrientationAngle(setAngle);

    QCOMPARE(InputMethod::instance()->orientationAngle(), expectedAngle);
    QCOMPARE(aboutToChange.size(), aboutToChangeCount);
    QCOMPARE(changed.size(), changedCount);
}

void Ut_Maliit_InputMethod::testSetOrientationAngleSignal_data()
{
    QTest::addColumn<Maliit::OrientationAngle>("startAngle");
    QTest::addColumn<Maliit::OrientationAngle>("setAngle");
    QTest::addColumn<Maliit::OrientationAngle>("expectedAngle");
    QTest::addColumn<int>("aboutToChangeCount");
    QTest::addColumn<int>("changedCount");

    QTest::newRow("startAngle and setAngle to the same angle") << Maliit::Angle90 << Maliit::Angle90 << Maliit::Angle90 << 1 << 1;
    QTest::newRow("just setAngle to an angle") << DefaultAngle << Maliit::Angle90 << Maliit::Angle90 << 0 << 1;
    QTest::newRow("startAngle and setAngle to different angles") << Maliit::Angle90 << Maliit::Angle180 << Maliit::Angle180 << 1 << 1;
    QTest::newRow("startAngle and setAngle to the default angle") << DefaultAngle  << DefaultAngle  << DefaultAngle << 0 << 0;
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
    // API not available on Qt 5 anymore
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    EventSpyInputContext<QEvent::Type> *spy = new EventSpyInputContext<QEvent::Type>(std::mem_fun(&QEvent::type));
    qApp->setInputContext(spy);

    Maliit::requestInputMethodPanel();

    QVERIFY(spy->size() == 1);
    QCOMPARE(spy->at(0), QEvent::RequestSoftwareInputPanel);
#endif
}

void Ut_Maliit_InputMethod::testCloseInputMethodPanel()
{
    // API not available on Qt 5 anymore
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    QSKIP("Does not apply to Qt5", SkipSingle);

    EventSpyInputContext<QEvent::Type> *spy = new EventSpyInputContext<QEvent::Type>(std::mem_fun(&QEvent::type));
    qApp->setInputContext(spy);

    Maliit::closeInputMethodPanel();

    QVERIFY(spy->size() == 1);
    QCOMPARE(spy->at(0), QEvent::CloseSoftwareInputPanel);
#endif
}

QTEST_APPLESS_MAIN(Ut_Maliit_InputMethod)

#include "ut_maliit_inputmethod.moc"
