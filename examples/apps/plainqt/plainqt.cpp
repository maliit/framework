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

#include <QtCore>
#include <QtGui>

#include <cstdlib>

int main(int argc, char** argv)
{
    // Set input method to MInputContext
#ifdef HAVE_LEGACY_NAMES
    setenv("QT_IM_MODULE", "MInputContext", 1);
#else
    setenv("QT_IM_MODULE", "Maliit", 1);
#endif

    QApplication kit(argc, argv);

#ifdef Q_WS_QPA
    // Workaround for lighthouse Qt
    kit.setInputContext(QInputContextFactory::create("Maliit", &kit));
#endif

    MainWindow window;
    return kit.exec();
}
