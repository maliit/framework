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
#include "minputcontextglibdbusconnection.h"
#include "mimpluginsproxywidget.h"

#if defined(Q_WS_X11)
#include "mimxapplication.h"
#include "mimremotewindow.h"
#include "mpassthruwindow.h"
#elif defined(Q_WS_QPA)
#include "mimqpaplatform.h"
#endif

#include <QApplication>
#include <QtDebug>
#include <QWidget>
#include <QGraphicsView>
#include <QPalette>
#include <QCommonStyle>
#include <stdlib.h>

using namespace std::tr1;

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

#if defined(Q_WS_X11)
    MImXApplication app(argc, argv);
    qDebug() << (app.selfComposited() ? "Use self composition" : "Use system compositor");
#else
    MIMApplication app(argc, argv);
#endif

    // Set a dummy input context so that Qt does not create a default input
    // context (qimsw-multi) which is expensive and not required by
    // meego-im-uiserver.
    app.setInputContext(new MIMDummyInputContext);

    // DBus Input Context Connection
    shared_ptr<MInputContextConnection> icConnection(new MInputContextGlibDBusConnection);

#if defined(Q_WS_X11)
    // For X11 the toplevel window is a MPassThruWindow (managed by MImXApplication. It
    // contains a MImPluginsProxyWidget as container for all plugin widgets and a
    // MImRotationAnimation, which is used to display the rotation animation. MImXApplication
    // also manages MImRemoteWindows representing the current application window.

    QObject::connect(icConnection.get(), SIGNAL(focusChanged(WId)),
                     &app, SLOT(setTransientHint(WId)));

    QObject::connect(icConnection.get(), SIGNAL(appOrientationAboutToChange(int)),
                     &app, SLOT(appOrientationAboutToChange(int)));
    QObject::connect(icConnection.get(), SIGNAL(appOrientationChanged(int)),
                     &app, SLOT(appOrientationChangeFinished(int)));

    // Container for all plugin widgets
    QWidget *pluginsProxyWidget = app.pluginsProxyWidget();

    // Manager for loading and handling all plugins
    MIMPluginManager *pluginManager = new MIMPluginManager(icConnection,
                                                           pluginsProxyWidget);

    // Hide active plugins when the application window is gone or iconified.
    QObject::connect(&app, SIGNAL(remoteWindowGone()),
                     pluginManager, SLOT(hideActivePlugins()));

    // Handle changed used area by plugins
    QObject::connect(pluginManager, SIGNAL(regionUpdated(const QRegion &)),
                     app.passThruWindow(), SLOT(inputPassthrough(const QRegion &)));

    // Configure widgets for self compositing after loading
    QObject::connect(pluginManager, SIGNAL(pluginLoaded()),
                     &app, SLOT(configureWidgetsForCompositing()));

    // Configure widgets loaded during MIMPluginManager construction
    app.configureWidgetsForCompositing();

#elif defined(Q_WS_QPA)
    // For QPA the toplevel window is a MImPluginsProxyWidget containing all plugin
    // widgets. The MImQPAPlatform is used to show/hide that toplevel window when
    // required.

    std::auto_ptr<MImQPAPlatform> platform(new MImQPAPlatform());

    MIMPluginManager *pluginManager = new MIMPluginManager(icConnection,
                                                           platform.get()->pluginsProxyWidget());

    QObject::connect(pluginManager, SIGNAL(regionUpdated(const QRegion &)),
                     platform.get(), SLOT(inputPassthrough(const QRegion &)));
#endif

    return app.exec();
}

