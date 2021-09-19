/* * This file is part of Maliit framework *
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 *
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 */

#ifndef FT_EXAMPLEPLUGIN_H
#define FT_EXAMPLEPLUGIN_H

#include <QtTest/QtTest>
#include <QtCore>

class Ft_ExamplePlugin 
    : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void init();
    void cleanup();

    void testFunction_data();
    void testFunction();
};

#endif // FT_EXAMPLEPLUGIN_H
