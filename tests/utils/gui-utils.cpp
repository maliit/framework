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

#include "gui-utils.h"

#include <QtDebug>
#include <QtCore>
#include <QtGui>
#include <QtTest>

namespace MaliitTestUtils {

RemoteWindow::RemoteWindow(QWidget *p, Qt::WindowFlags f)
    : QWidget(p, f)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::X11BypassWindowManagerHint);
}

void RemoteWindow::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setBrush(QBrush(QColor(Qt::green)));
    p.drawRect(QRect(QPoint(), size()));
    QFont f;
    f.setPointSize(32);
    p.setFont(f);
    p.drawText(QRect(QPoint(), size()).adjusted(16, 16, -16, -16),
               QString("Maliit"));
}

}

