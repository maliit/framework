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
#include "mimabstractserverlogic.h"
#include "windowedsurfacegroup.h"

using namespace std::tr1;

class MImServerPrivate
{
public:
    explicit MImServerPrivate();

    // Manager for loading and handling all plugins
    MIMPluginManager *pluginManager;

    // Connection to application side (input-context)
    shared_ptr<MInputContextConnection> icConnection;

    // Platform/deployment specific server logic
    QSharedPointer<MImAbstractServerLogic> serverLogic;

private:
    Q_DISABLE_COPY(MImServerPrivate)
};

MImServerPrivate::MImServerPrivate()
{}

MImServer::MImServer(const QSharedPointer<MImAbstractServerLogic> &serverLogic,
                     const shared_ptr<MInputContextConnection> &icConnection,
                     QObject *parent)
  : QObject(parent)
  , d_ptr(new MImServerPrivate)
{
    Q_D(MImServer);

    d->icConnection = icConnection;
    d->serverLogic = serverLogic;
    QSharedPointer<Maliit::Server::AbstractSurfaceGroupFactory> surfaceFactory(new Maliit::Server::WindowedSurfaceGroupFactory);
    d->pluginManager = new MIMPluginManager(d->icConnection, surfaceFactory);

    connectComponents();

    // Notify server logic about plugins loaded during MIMPluginManager construction
    d->serverLogic->pluginLoaded();
}

MImServer::~MImServer()
{
    // FIXME: MIMPluginManager is never deleted
}

void MImServer::connectComponents()
{
    Q_D(MImServer);

    // Configure widgets trees of plugins after loading
    QObject::connect(d->pluginManager, SIGNAL(pluginLoaded()),
                     d->serverLogic.data(), SLOT(pluginLoaded()));

    // Tracking of application window
    QObject::connect(d->icConnection.get(), SIGNAL(focusChanged(WId)),
                     d->serverLogic.data(), SLOT(applicationFocusChanged(WId)));

    // Rotation handling
    QObject::connect(d->icConnection.get(), SIGNAL(contentOrientationAboutToChange(int)),
                     d->serverLogic.data(), SLOT(appOrientationAboutToChange(int)));
    QObject::connect(d->icConnection.get(), SIGNAL(contentOrientationChanged(int)),
                     d->serverLogic.data(), SLOT(appOrientationChangeFinished(int)));

    // Hide active plugins when the application window is gone or iconified.
    QObject::connect(d->serverLogic.data(), SIGNAL(applicationWindowGone()),
                     d->pluginManager, SLOT(hideActivePlugins()));
}

QWidget *MImServer::pluginsWidget()
{
    Q_D(MImServer);
    return d->serverLogic->pluginsProxyWidget();
}
