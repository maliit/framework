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

#ifndef UT_SELFCOMPOSITING_H
#define UT_SELFCOMPOSITING_H

#include "mimserveroptions.h"

#include <QtTest/QtTest>
#include <QtCore>
#include <QtGui>

class MImXApplication;
class MImXServerLogic;

class Ut_SelfCompositing : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void init();
    void cleanup();

private:
    MImXApplication *app;
    MImServerXOptions xOptions;
    MImXServerLogic *serverLogic;
};

#endif // UT_SELFCOMPOSITING_H
