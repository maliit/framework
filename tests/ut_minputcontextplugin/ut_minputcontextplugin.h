/* * This file is part of Maliit framework *
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
#ifndef __UT_MINPUTCONTEXTPLUGIN_H__
#define __UT_MINPUTCONTEXTPLUGIN_H__

#include <QtTest/QtTest>
#include <QObject>

class MInputContextPlugin;
class QApplication;

class Ut_MInputContextPlugin : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void init();
    void cleanup();

    void testCreate();
    void testOther();

private:
    MInputContextPlugin *subject;
};

#endif // __UT_MINPUTCONTEXTPLUGIN_H__
