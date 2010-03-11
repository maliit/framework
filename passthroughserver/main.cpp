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


// Input method overlay window

#include <QtDebug>
#include <QWidget>
#include <QGLWidget>
#include <QPalette>

#include <duireactionmap.h>

#include <DuiApplication>
#include <DuiScene>
#include <duiplainwindow.h>

#include "duiimpluginmanager.h"
#include "duipassthruwindow.h"


int main(int argc, char **argv)
{
    bool bypassWMHint = false;

    for (int i = 1; i < argc; i++) {
        QString s(argv[i]);
        if (s == "-bypass-wm-hint") {
            bypassWMHint = true;
        }
    }

    // prevent loading of duiinputcontext because we don't need it and starting
    // it might trigger starting of this service by the d-bus. not nice if that is
    // already happening :)
    DuiApplication::setLoadDuiInputContext(false);

    DuiApplication app(argc, argv);

    DuiPassThruWindow widget(bypassWMHint);
    widget.setFocusPolicy(Qt::NoFocus);

    // Must be declared after creation of top level window.
    DuiReactionMap reactionMap(&widget);
    DuiPlainWindow *view = new DuiPlainWindow(&widget);

#ifndef DUI_IM_DISABLE_TRANSLUCENCY
    // enable translucent in hardware rendering
    view->setTranslucentBackground(!DuiApplication::softwareRendering());
#endif

    // No auto fill in software rendering
    if (DuiApplication::softwareRendering())
        view->viewport()->setAutoFillBackground(false);

    QSize sceneSize = view->visibleSceneSize(Dui::Landscape);
    int w = sceneSize.width();
    int h = sceneSize.height();
    view->scene()->setSceneRect(0, 0, w, h);

    widget.resize(sceneSize);

    view->setMinimumSize(1, 1);
    view->setMaximumSize(w, h);

    DuiIMPluginManager *pluginManager = new DuiIMPluginManager();

    QObject::connect(pluginManager, SIGNAL(regionUpdated(const QRegion &)),
                     &widget, SLOT(inputPassthrough(const QRegion &)));
#ifdef DUI_IM_DISABLE_TRANSLUCENCY
    QObject::connect(pluginManager, SIGNAL(regionUpdated(const QRegion &)),
                     view, SLOT(updatePosition(const QRegion &)));
#endif

    return app.exec();
}
