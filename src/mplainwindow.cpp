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

#include "mplainwindow.h"

#include <MSceneManager>
#include <MGConfItem>
#include <QDebug>

namespace
{
    // This GConf item defines whether multitouch is enabled or disabled
    const char * const MultitouchSettings = "/meegotouch/inputmethods/multitouch/enabled";
}

MPlainWindow *MPlainWindow::m_instance = 0;

MPlainWindow *MPlainWindow::instance()
{
    return m_instance;
}

MPlainWindow::MPlainWindow(QWidget *parent) :
    MWindow(new MSceneManager, parent)
{
    if (m_instance)
        qFatal("There can be only one instance of MPlainWindow");

    m_instance = this;

    if (MGConfItem(MultitouchSettings).value().toBool()) {
        setAttribute(Qt::WA_AcceptTouchEvents);
    }
}

MPlainWindow::~MPlainWindow()
{
    m_instance = 0;
}

#if defined(M_IM_DISABLE_TRANSLUCENCY) && !defined(M_IM_USE_SHAPE_WINDOW)
void MPlainWindow::updatePosition(const QRegion &region)
{
    //update view's parameters to compensate movement of MPasstroughWindow
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
    case M::Angle0:
        setSceneRect(0, sceneSize.height() - rect.height(),
                     sceneSize.width(), rect.height());
        resize(rect.width(), rect.height());
        move(0, 0);
        break;
    case M::Angle90:
        setSceneRect(0, 0, rect.width(), rect.height());
        resize(rect.width(), rect.height());
        move(sceneSize.height() - rect.width(), 0);
        break;
    case M::Angle180:
        setSceneRect(0, 0, rect.width(), sceneSize.height());
        resize(rect.width(), rect.height());
        move(0, sceneSize.height() - rect.height());
        break;
    case M::Angle270:
        setSceneRect(sceneSize.height() - rect.width(), rect.top(),
                     rect.width(), sceneSize.width());
        resize(rect.width(), rect.height());
        move(0, 0);
        break;
    }
}
#endif

