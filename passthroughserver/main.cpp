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

#include "mimpluginmanager.h"
#include "mimapplication.h"
#include "mimdummyinputcontext.h"
#include "mimremotewindow.h"
#include "mimrotationanimation.h"
#include "mpassthruwindow.h"

#include <QApplication>
#include <QtDebug>
#include <QWidget>
#include <QGraphicsView>
#include <QPalette>
#include <QCommonStyle>
#include <stdlib.h>

namespace {
    void disableMInputContextPlugin()
    {
        // prevent loading of minputcontext because we don't need it and starting
        // it might trigger starting of this service by the d-bus. not nice if that is
        // already happening :)
        if (-1 == unsetenv("QT_IM_MODULE")) {
            qWarning("meego-im-uiserver: unable to unset QT_IM_MODULE.");
        }

        // TODO: Check if hardwiring the QStyle can be removed at a later stage.
        QApplication::setStyle(new QCommonStyle);
    }
}

int main(int argc, char **argv)
{
    // QT_IM_MODULE, MApplication and QtMaemo5Style all try to load
    // MInputContext, which is fine for the application. For the passthrough
    // server itself, we absolutely need to prevent that.
    disableMInputContextPlugin();

    MIMApplication app(argc, argv);
    // Set a dummy input context so that Qt does not create a default input
    // context (qimsw-multi) which is expensive and not required by
    // meego-im-uiserver.
    app.setInputContext(new MIMDummyInputContext);

    qDebug() << (app.selfComposited() ? "Use self composition" : "Use system compositor");

    MImRotationAnimation *rotationAnimation = new MImRotationAnimation(app.pluginsProxyWidget(), app.passThruWindow());

    // Pass the rotationAnimation so that we can receive the orientation
    // signals from the dbus connection.
    // TODO: This should probably be done in a cleaner way.
    MIMPluginManager *pluginManager = new MIMPluginManager(rotationAnimation);

    QObject::connect(pluginManager, SIGNAL(regionUpdated(const QRegion &)),
                     app.passThruWindow(), SLOT(inputPassthrough(const QRegion &)));
#if defined(M_IM_DISABLE_TRANSLUCENCY) && !defined(M_IM_USE_SHAPE_WINDOW)
    QObject::connect(pluginManager, SIGNAL(regionUpdated(const QRegion &)),
                     view, SLOT(updatePosition(const QRegion &)));
#endif
    // hide active plugins when remote input window is gone or iconified.
    QObject::connect(&app, SIGNAL(remoteWindowChanged(MImRemoteWindow*)),
                     pluginManager, SLOT(hideActivePluginsIfWindowGone(MImRemoteWindow*)));

    return app.exec();
}

