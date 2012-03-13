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

#include "embedded.h"

#include <QApplication>
#if (QT_VERSION < 0x050000)
#include <QInputContextFactory>
#endif

int main(int argc, char** argv)
{
    QApplication kit(argc, argv);

#if (QT_VERSION < 0x050000)
    kit.setInputContext(QInputContextFactory::create("MaliitDirect", &kit));
#endif

    MainWindow window;
    return kit.exec();
}
