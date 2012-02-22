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

#include "ut_selfcompositing.h"
#include "mimgraphicsview.h"
#include "mimwidget.h"
#include "mimxapplication.h"
#include "utils.h"

#if defined(Q_WS_X11)
#include <QX11Info>
#endif

namespace
{
    const QSize windowSize(200, 200);

    QWidget *createGraphicsView(QWidget *parent)
    {
        MImGraphicsView *subject = new MImGraphicsView(new QGraphicsScene(parent), parent);
        subject->setSceneRect(QRect(QPoint(), windowSize));
        subject->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        subject->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        subject->setFrameStyle(0);
        subject->setAttribute(Qt::WA_OpaquePaintEvent);
        subject->setAttribute(Qt::WA_NoSystemBackground);
        subject->viewport()->setAttribute(Qt::WA_OpaquePaintEvent);
        subject->viewport()->setAttribute(Qt::WA_NoSystemBackground);
        subject->resize(parent->size());
        subject->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
        subject->setCacheMode(QGraphicsView::CacheNone);
        return subject;
    }

    QWidget *createWidget(QWidget *parent)
    {
        MImWidget *subject = new MImWidget(parent);
        subject->setAttribute(Qt::WA_OpaquePaintEvent);
        subject->setAttribute(Qt::WA_NoSystemBackground);
        subject->resize(parent->size());
        return subject;
    }
}

typedef QWidget * (* WidgetCreator)(QWidget *parent);
Q_DECLARE_METATYPE(WidgetCreator);

void Ut_SelfCompositing::initTestCase()
{
#if !defined(Q_WS_X11)
    app = 0;
    QSKIP("Test is useless for your platform", SkipAll);
#endif

    static char *argv[1] = { (char *) "Ut_SelfCompositing" };
    static int argc = 1;

    // Enforcing raster GS to make test reliable:
    QApplication::setGraphicsSystem("raster");

#if defined(Q_WS_X11)
    xOptions.selfComposited = true;
#endif

    app = new MImXApplication(argc, argv, xOptions);
}

void Ut_SelfCompositing::cleanupTestCase()
{
    delete app;
}

void Ut_SelfCompositing::init()
{}

void Ut_SelfCompositing::cleanup()
{}

void Ut_SelfCompositing::testSelfCompositing_data()
{
    QTest::addColumn<WidgetCreator>("widgetCreator");
    QTest::newRow("MImWidget") << &createWidget;
    QTest::newRow("MImGraphicsView") << &createGraphicsView;
}

void Ut_SelfCompositing::testSelfCompositing()
{
#if defined(Q_WS_X11)
    if (not QX11Info::isCompositingManagerRunning()) {
        QSKIP("Not running a compositing windowmanager", SkipSingle);
    }
#endif


    QFETCH(WidgetCreator, widgetCreator);

    MaliitTestUtils::RemoteWindow remote;
    remote.setGeometry(0, 0, windowSize.width(), windowSize.height());

    QWidget *passthru = app->passThruWindow();
    passthru->setGeometry(remote.geometry().right() + 10, 0,
                          windowSize.width(), windowSize.height());

    QWidget *subject = widgetCreator(passthru);

    remote.show();
    app->setTransientHint(remote.window()->effectiveWinId());

    passthru->show();
    QTest::qWaitForWindowShown(remote.window());

    subject->show();
    QTest::qWaitForWindowShown(passthru->window());
    passthru->raise();
    app->remoteWindow()->redirect();

    QApplication::setActiveWindow(passthru);

    remote.update(); // Not strictly required, due to our window attributes.
    QCoreApplication::processEvents();

    QGraphicsView *view = qobject_cast<QGraphicsView *>(subject);
    QImage subjectImage = QPixmap::grabWidget(view  ? view->viewport() : subject).toImage();
    QImage remoteImage = QPixmap::grabWidget(&remote).toImage();
    QCOMPARE(subjectImage, remoteImage);

    app->remoteWindow()->unredirect();
}

QTEST_APPLESS_MAIN(Ut_SelfCompositing)
