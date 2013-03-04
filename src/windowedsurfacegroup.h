/* * This file is part of Maliit framework *
 *
 * Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * Copyright (C) 2012 Canonical Ltd
 *
 * Contact: maliit-discuss@lists.maliit.org
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 */

#ifndef MALIIT_PLUGINS_WINDOWEDSURFACEGROUP_H
#define MALIIT_PLUGINS_WINDOWEDSURFACEGROUP_H

#include "abstractsurfacegroupfactory.h"

#include <QList>
#include <QWeakPointer>
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QWindow>
#else
#include <QWidget> // For WId
#endif

namespace Maliit {
namespace Server {

class WindowedSurfaceGroup;

class WindowedSurfaceGroupFactory : public QObject, public AbstractSurfaceGroupFactory
{
    Q_OBJECT

public:
    QSharedPointer<AbstractSurfaceGroup> createSurfaceGroup();

    void applicationFocusChanged(WId winId);

Q_SIGNALS:
    void surfaceWidgetCreated(QWidget *widget, int options);

private:
    QList<QWeakPointer<WindowedSurfaceGroup> > mGroups;
};

} // namespace Server
} // namespace Maliit

#endif // MALIIT_PLUGINS_WINDOWEDSURFACEGROUP
