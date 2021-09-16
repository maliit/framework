/* * This file is part of Maliit framework *
 *
 * Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 *
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 */

#ifndef UT_MIMSETTINGS_H
#define UT_MIMSETTINGS_H

#include <QtTest/QtTest>
#include <QObject>

class Ut_MImSettings : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void init();
    void cleanup();

    void testValue();
    void testModifyValue();
    void testUnsetValue();
    void testModifyValueNotification();
    void testListDirs();
    void testListEntries();
};

#endif
