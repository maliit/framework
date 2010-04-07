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
}

DuiPlainWindow::~DuiPlainWindow()
{
    m_instance = 0;
}

#if defined(DUI_IM_DISABLE_TRANSLUCENCY) && !defined(DUI_IM_USE_SHAPE_WINDOW)
void DuiPlainWindow::updatePosition(const QRegion &region)
{
    //update view's parameters to compensate movement of DuiPasstroughWindow
    //this update allows plugin to use screen coordinates
    const QRect rect = region.boundingRect();
    const QSize sceneSize = visibleSceneSize();
    const int size = region.rects().size();

    if (!size) {
        setSceneRect(0, 0, sceneSize.width(), sceneSize.height());
        resize(sceneSize);
        move(0, 0);
        return;
    }

    switch (orientationAngle())
    {
    case Dui::Angle0:
        setSceneRect(0, sceneSize.height() - rect.height(),
                     sceneSize.width(), rect.height());
        resize(rect.width(), rect.height());
        move(0, 0);
        break;
    case Dui::Angle90:
        setSceneRect(0, 0, rect.width(), rect.height());
        resize(rect.width(), rect.height());
        move(sceneSize.height() - rect.width(), 0);
        break;
    case Dui::Angle180:
        setSceneRect(0, 0, rect.width(), sceneSize.height());
        resize(rect.width(), rect.height());
        move(0, sceneSize.height() - rect.height());
        break;
    case Dui::Angle270:
        setSceneRect(sceneSize.height() - rect.width(), rect.top(),
                     rect.width(), sceneSize.width());
        resize(rect.width(), rect.height());
        move(0, 0);
        break;
    }
}
#endif

