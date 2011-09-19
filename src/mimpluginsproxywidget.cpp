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

#include "mimpluginsproxywidget.h"
#include "mimapplication.h"

#include <QDesktopWidget>

MImPluginsProxyWidget::MImPluginsProxyWidget(QWidget *parent) :
    QWidget(parent)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_X11DoNotAcceptFocus);
    setAutoFillBackground(false);
    setBackgroundRole(QPalette::NoRole);

    if (MIMApplication::instance() && MIMApplication::instance()->selfComposited()) {
        setAttribute(Qt::WA_OpaquePaintEvent);
        setAttribute(Qt::WA_NoSystemBackground);
    } else {
        setAttribute(Qt::WA_TranslucentBackground);
    }

    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    QDesktopWidget* desktop = QApplication::desktop();
    setMinimumSize(desktop->screenGeometry().size());
}
