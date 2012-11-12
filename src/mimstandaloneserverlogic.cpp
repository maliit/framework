/* * This file is part of Maliit framework *
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 *
 * Contact: maliit-discuss@lists.maliit.org
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 */

#include "mimstandaloneserverlogic.h"

#include "mimpluginsproxywidget.h"

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include "quickviewsurfacegroup.h"
#else
#include "windowedsurfacegroup.h"
#endif

#include <QDebug>
#include <QWidget>

MImStandaloneServerLogic::MImStandaloneServerLogic() :
    MImAbstractServerLogic(0),
    mProxyWidget(new MImPluginsProxyWidget()),
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    mSurfaceGroupFactory(new Maliit::Server::QuickViewSurfaceGroupFactory)
#else
    mSurfaceGroupFactory(new Maliit::Server::WindowedSurfaceGroupFactory)
#endif
{
}

MImStandaloneServerLogic::~MImStandaloneServerLogic()
{
}


void MImStandaloneServerLogic::inputPassthrough(const QRegion &region)
{
    if (region.isEmpty())
        mProxyWidget->hide();
    else
        mProxyWidget->show();
}

QSharedPointer<Maliit::Server::AbstractSurfaceGroupFactory> MImStandaloneServerLogic::surfaceGroupFactory() const
{
    return mSurfaceGroupFactory;
}

void MImStandaloneServerLogic::pluginLoaded()
{
}

void MImStandaloneServerLogic::appOrientationAboutToChange(int toAngle)
{
    Q_UNUSED(toAngle);
}

void MImStandaloneServerLogic::appOrientationChangeFinished(int toAngle)
{
    Q_UNUSED(toAngle);
}

void MImStandaloneServerLogic::applicationFocusChanged(WId newRemoteWinId)
{
    Q_UNUSED(newRemoteWinId);
}
