/* * This file is part of meego-keyboard *
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

#ifndef UT_MATTRIBUTEEXTENSIONMANAGER_H
#define UT_MATTRIBUTEEXTENSIONMANAGER_H

#include <QObject>
#include <QtTest/QTest>

class MAttributeExtensionManager;

class Ut_MAttributeExtensionManager : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
    void testSetExtendedAttribute();

private:
    MAttributeExtensionManager *subject;
};

#endif
