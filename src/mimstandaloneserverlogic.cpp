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

#include "mimstandaloneserverlogic.h"

#include "mimpluginsproxywidget.h"

#include "windowedsurfacegroup.h"

#include <QDebug>
#include <QWidget>

MImStandaloneServerLogic::MImStandaloneServerLogic() :
    MImAbstractServerLogic(0),
    mProxyWidget(new MImPluginsProxyWidget()),
    mSurfaceGroupFactory(new Maliit::Server::WindowedSurfaceGroupFactory)
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
