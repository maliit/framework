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
#include "mimxapplication.h"
#include "mimremotewindow.h"
#include "gui-utils.h"

#if defined(Q_WS_X11)
#include <QX11Info>
#endif

namespace
{
}

void Ut_SelfCompositing::initTestCase()
{
    static char *argv[1] = { (char *) "Ut_SelfCompositing" };
    static int argc = 1;

    // Enforcing raster GS to make test reliable:
    QApplication::setGraphicsSystem("raster");

    xOptions.selfComposited = true;

    app = new MImXApplication(argc, argv, xOptions);
    serverLogic = app->serverLogic();
}

void Ut_SelfCompositing::cleanupTestCase()
{
    delete app;
}

void Ut_SelfCompositing::init()
{}

void Ut_SelfCompositing::cleanup()
{}

QTEST_APPLESS_MAIN(Ut_SelfCompositing)
