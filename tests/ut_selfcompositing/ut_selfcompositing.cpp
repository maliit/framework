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
#include "mimapplication.h"

namespace
{
    const QSize windowSize(200, 200);

    class Remote
        : public QWidget
    {
    public:
        explicit Remote(QWidget *p = 0,
                        Qt::WindowFlags f = 0)
            : QWidget(p, f)
        {
            setWindowFlags(Qt::FramelessWindowHint | Qt::X11BypassWindowManagerHint);
        }

        void paintEvent(QPaintEvent *)
        {
            QPainter p(this);
            p.setBrush(QBrush(QColor(Qt::green)));
            p.drawRect(QRect(QPoint(), size()));
            QFont f;
            f.setPointSize(32);
            p.setFont(f);
            p.drawText(QRect(QPoint(), size()).adjusted(16, 16, -16, -16),
                       QString("Maliit"));
        }
    };

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
    static char *argv[2] = { (char *) "Ut_SelfCompositing",
                             (char *) "-use-self-composition" };
    static int argc = 2;

    app = new MIMApplication(argc, argv);
    QVERIFY(app->selfComposited());
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
    QFETCH(WidgetCreator, widgetCreator);

    Remote remote;
    remote.setGeometry(0, 0, windowSize.width(), windowSize.height());

    QWidget *passthru = app->passThruWindow();
    passthru->setGeometry(remote.geometry().right() + 10, 0,
                          windowSize.width(), windowSize.height());

    QWidget *subject = widgetCreator(passthru);

    remote.show();
    passthru->show();
    subject->show();

    QTest::qWaitForWindowShown(remote.window());
    QTest::qWaitForWindowShown(passthru->window());
    passthru->raise();

    app->setTransientHint(remote.window()->effectiveWinId());
    app->remoteWindow()->redirect();

    remote.update(); // Not strictly required, due to our window attributes.
    QCoreApplication::processEvents();

    QGraphicsView *view = qobject_cast<QGraphicsView *>(subject);
    QImage subjectImage = QPixmap::grabWidget(view  ? view->viewport() : subject).toImage();
    QImage remoteImage = QPixmap::grabWidget(&remote).toImage();
    QCOMPARE(subjectImage, remoteImage);

    app->remoteWindow()->unredirect();
}

QTEST_APPLESS_MAIN(Ut_SelfCompositing)
