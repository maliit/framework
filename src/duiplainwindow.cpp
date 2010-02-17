/* * This file is part of dui-im-framework *
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

#include "duiplainwindow.h"

#include <DuiSceneManager>
#include <QDebug>

DuiPlainWindow *DuiPlainWindow::m_instance = 0;

DuiPlainWindow *DuiPlainWindow::instance()
{
    return m_instance;
}

DuiPlainWindow::DuiPlainWindow(QWidget *parent) :
    DuiWindow(new DuiSceneManager, parent)
{
    if (m_instance)
        qFatal("There can be only one instance of DuiPlainWindow");

    m_instance = this;
    setViewportUpdateMode(DuiWindow::MinimalViewportUpdate);
}

DuiPlainWindow::~DuiPlainWindow()
{
    m_instance = 0;
}
