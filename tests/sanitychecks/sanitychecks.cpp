/* * This file is part of meego-im-framework *
 *
 * Copyright (C) 2010, 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#include "sanitychecks.h"

#include "core-utils.h"

void DeferredSignalEmitter::emitSignal()
{
    Q_EMIT deferredSignal();
}

void SanityChecks::initTestCase()
{
}

void SanityChecks::cleanupTestCase()
{
}

void SanityChecks::init()
{
}

void SanityChecks::cleanup()
{
}

/* Verify that it is possible for asyncronous work to happen using the Qt eventloop:
 * A callback registered with QTimer:singleShot should be called from the
 * event loop when creating a QEventloop and executing it. */
void SanityChecks::testAsyncronousCallbacks()
{
    DeferredSignalEmitter emitter;
    QSignalSpy signalSpy(&emitter, SIGNAL(deferredSignal()));

    QTimer::singleShot(0, &emitter, SLOT(emitSignal()));
    QCOMPARE(signalSpy.count(), 0);

    // Utilty function that executes a QEventLoop
    MaliitTestUtils::waitForSignal(&emitter, SIGNAL(deferredSignal()), 1000);
    QCOMPARE(signalSpy.count(), 1);
}

QTEST_MAIN(SanityChecks)
