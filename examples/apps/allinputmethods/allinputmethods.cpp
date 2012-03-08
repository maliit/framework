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

#include "mainwindow.h"

#include <cstdlib>
#include <QApplication>

int main(int argc, char** argv)
{
    // Set input method to MInputContext
    setenv("QT_IM_MODULE", "Maliit", 1);

    QApplication kit(argc, argv);

    MainWindow window;
    return kit.exec();
}
