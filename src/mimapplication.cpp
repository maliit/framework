/* * This file is part of meego-im-framework *
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "mimapplication.h"

#include <QDebug>

MIMApplication::MIMApplication(int &argc, char **argv)
    : QApplication(argc, argv)

{
    connect(this, SIGNAL(aboutToQuit()),
            this, SLOT(finalize()),
            Qt::UniqueConnection);
}

MIMApplication::~MIMApplication()
{
}

void MIMApplication::finalize()
{
}

QWidget* MIMApplication::toplevel() const
{
    return 0;
}

QWidget* MIMApplication::pluginsProxyWidget() const
{
    return 0;
}

MIMApplication *MIMApplication::instance()
{
    return static_cast<MIMApplication *>(QCoreApplication::instance());
}

bool MIMApplication::selfComposited() const
{
    return false;
}

const QPixmap &MIMApplication::remoteWindowPixmap()
{
    static const QPixmap empty;
    return empty;
}
