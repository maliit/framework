/* * This file is part of Maliit framework *
 *
 * Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef UT_MIMROTATIONANIMATION_H
#define UT_MIMROTATIONANIMATION_H

#include <QtTest/QtTest>
#include <QObject>
#include "mimserveroptions.h"
#include "gui-utils.h"

class MImXApplication;

class Ut_MImRotationAnimation : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void testPassthruHiddenDuringRotation();

private:
    MImXApplication *app;
    MaliitTestUtils::RemoteWindow *remote;
    MImServerXOptions xOptions;
};

#endif // UT_MIMROTATIONANIMATION_H
