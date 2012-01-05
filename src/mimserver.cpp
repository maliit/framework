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

#include "mimserver.h"

#include "mimpluginmanager.h"

#if defined(Q_WS_X11)
#include "mimxapplication.h"
#elif defined(Q_WS_QPA) || defined(Q_WS_QWS)
#include "mimqpaplatform.h"
#endif

using namespace std::tr1;

class MImServerPrivate
{
public:
    explicit MImServerPrivate();

    // Manager for loading and handling all plugins
    MIMPluginManager *pluginManager;

    // Connection to application side (input-context)
    shared_ptr<MInputContextConnection> icConnection;

#if defined(Q_WS_QPA) || defined(Q_WS_QWS)
    std::auto_ptr<MImQPAPlatform> platform;
#endif

private:
    Q_DISABLE_COPY(MImServerPrivate)
};

MImServerPrivate::MImServerPrivate()
{}

// X11:
// For X11 the toplevel window is a MPassThruWindow (managed by MImXApplication. It
// contains a MImPluginsProxyWidget as container for all plugin widgets and a
// MImRotationAnimation, which is used to display the rotation animation. MImXApplication
// also manages MImRemoteWindows representing the current application window.
//
// QPA:
// For QPA the toplevel window is a MImPluginsProxyWidget containing all plugin
// widgets. The MImQPAPlatform is used to show/hide that toplevel window when
// required.
MImServer::MImServer(shared_ptr<MInputContextConnection> icConnection, QObject *parent)
  : QObject(parent)
  , d_ptr(new MImServerPrivate)
{
    Q_D(MImServer);

    d->icConnection = icConnection;

#if defined(Q_WS_X11)
    MImXApplication *app = MImXApplication::instance();
    qDebug() << (app->selfComposited() ? "Use self composition" : "Use system compositor");
#elif defined(Q_WS_QPA) || defined(Q_WS_QWS)
    d->platform.reset(new MImQPAPlatform);
#endif

    d->pluginManager = new MIMPluginManager(d->icConnection, pluginsWidget());

    connectComponents();

#if defined(Q_WS_X11)
    // Configure widgets loaded during MIMPluginManager construction
    // only needed on X11 for self-compositing
    app->configureWidgetsForCompositing();
#endif
}

MImServer::~MImServer()
{

}

void MImServer::connectComponents()
{
    Q_D(MImServer);

#if defined(Q_WS_X11)
    MImXApplication *app = MImXApplication::instance();

    QObject::connect(d->icConnection.get(), SIGNAL(focusChanged(WId)),
                     app, SLOT(setTransientHint(WId)));

    QObject::connect(d->icConnection.get(), SIGNAL(appOrientationAboutToChange(int)),
                     app, SLOT(appOrientationAboutToChange(int)));
    QObject::connect(d->icConnection.get(), SIGNAL(appOrientationChanged(int)),
                     app, SLOT(appOrientationChangeFinished(int)));

    // Hide active plugins when the application window is gone or iconified.
    QObject::connect(app, SIGNAL(remoteWindowGone()),
                     d->pluginManager, SLOT(hideActivePlugins()));

    // Handle changed used area by plugins
    QObject::connect(d->pluginManager, SIGNAL(regionUpdated(const QRegion &)),
                     app->passThruWindow(), SLOT(inputPassthrough(const QRegion &)));

    // Configure widgets for self compositing after loading
    QObject::connect(d->pluginManager, SIGNAL(pluginLoaded()),
                     app, SLOT(configureWidgetsForCompositing()));

#elif defined(Q_WS_QPA) || defined(Q_WS_QWS)
    QObject::connect(d->pluginManager, SIGNAL(regionUpdated(const QRegion &)),
                     d->platform.get(), SLOT(inputPassthrough(const QRegion &)));
#endif
}

QWidget *MImServer::pluginsWidget()
{
    Q_D(MImServer);

#if defined(Q_WS_X11)
    Q_UNUSED(d);
    return MImXApplication::instance()->pluginsProxyWidget();
#elif defined(Q_WS_QPA) || defined(Q_WS_QWS)
    return d->platform.get()->pluginsProxyWidget();
#endif
}
