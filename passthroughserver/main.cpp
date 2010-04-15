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


// Input method overlay window

#include <QtDebug>
#include <QWidget>
#include <QGLWidget>
#include <QPalette>

#include <duireactionmap.h>

#include <MApplication>
#include <MScene>
#include <mplainwindow.h>
#include <QCommonStyle>

#include "mimpluginmanager.h"
#include "mpassthruwindow.h"


int main(int argc, char **argv)
{
    bool bypassWMHint = false;

    for (int i = 1; i < argc; i++) {
        QString s(argv[i]);
        if (s == "-bypass-wm-hint") {
            bypassWMHint = true;
        }
    }

    // prevent loading of minputcontext because we don't need it and starting
    // it might trigger starting of this service by the d-bus. not nice if that is
    // already happening :)
    MApplication::setLoadMInputContext(false);

    MApplication app(argc, argv);

    // TODO: Check if hardwiring the QStyle can be removed at a later state.
    app.setStyle(new QCommonStyle);

    MPassThruWindow widget(bypassWMHint);
    widget.setFocusPolicy(Qt::NoFocus);

    // Must be declared after creation of top level window.
    DuiReactionMap reactionMap(&widget);
    MPlainWindow *view = new MPlainWindow(&widget);

#ifndef M_IM_DISABLE_TRANSLUCENCY
    // enable translucent in hardware rendering
    view->setTranslucentBackground(!MApplication::softwareRendering());
#endif

    // No auto fill in software rendering
    if (MApplication::softwareRendering())
        view->viewport()->setAutoFillBackground(false);

    QSize sceneSize = view->visibleSceneSize(M::Landscape);
    int w = sceneSize.width();
    int h = sceneSize.height();
    view->scene()->setSceneRect(0, 0, w, h);

    widget.resize(sceneSize);

    view->setMinimumSize(1, 1);
    view->setMaximumSize(w, h);

    MIMPluginManager *pluginManager = new MIMPluginManager();

    QObject::connect(pluginManager, SIGNAL(regionUpdated(const QRegion &)),
                     &widget, SLOT(inputPassthrough(const QRegion &)));
#if defined(M_IM_DISABLE_TRANSLUCENCY) && !defined(M_IM_USE_SHAPE_WINDOW)
    QObject::connect(pluginManager, SIGNAL(regionUpdated(const QRegion &)),
                     view, SLOT(updatePosition(const QRegion &)));
#endif

    return app.exec();
}
