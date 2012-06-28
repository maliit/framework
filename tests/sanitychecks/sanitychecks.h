/* * This file is part of Maliit framework *
 *
 * Copyright (C) 2010, 2011 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 *
 * Contact: maliit-discuss@lists.maliit.org
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 */

#ifndef SANITYCHECKS_H
#define SANITYCHECKS_H

#include <QtTest/QtTest>
#include <QObject>

// Will emit the signal from the Qt event loop after being fired.
class DeferredSignalEmitter : public QObject
{
    Q_OBJECT

public:
    Q_SIGNAL void deferredSignal();
    Q_SLOT void emitSignal();
};

// Tests the assumptions that the unittests rely on.
class SanityChecks : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void init();
    void cleanup();

    void testAsyncronousCallbacks();
};

#endif // SANITYCHECKS_H
