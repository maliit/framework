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

#include "ut_mimrotationanimation.h"

#include "mimapplication.h"
#include "mimrotationanimation.h"

#include "utils.h"

namespace {
    const QSize windowSize(200, 200);
}

void Ut_MImRotationAnimation::initTestCase()
{
    static char *argv[1] = { (char *) "Ut_MImRotationAnimation" };
    static int argc = 1;

    // Enforcing raster GS to make test reliable:
    QApplication::setGraphicsSystem("raster");

    app = new MIMApplication(argc, argv);

    remote = new MaliitTestUtils::RemoteWindow;
    remote->setGeometry(0, 0, windowSize.width(), windowSize.height());

    remote->show();
    QTest::qWaitForWindowShown(remote->window());
}

void Ut_MImRotationAnimation::cleanupTestCase()
{
    delete remote;
    delete app;
}

void Ut_MImRotationAnimation::testPassthruHiddenDuringRotation()
{
    MImRotationAnimation subject(app->passThruWindow());
    app->setTransientHint(remote->window()->effectiveWinId());

    subject.appOrientationChangeFinished(0);

    QMetaObject::invokeMethod(app->passThruWindow(), "inputPassthrough", Qt::DirectConnection,
                              Q_ARG(QRegion, QRegion(QRect(QPoint(), QSize(600, 400)))));
    subject.appOrientationAboutToChange(270);
    QVERIFY(subject.isVisible());

    QMetaObject::invokeMethod(app->passThruWindow(), "inputPassthrough", Qt::DirectConnection,
                              Q_ARG(QRegion, QRegion()));
    subject.appOrientationChangeFinished(270);
    QVERIFY(!subject.isVisible());
}

QTEST_APPLESS_MAIN(Ut_MImRotationAnimation);
