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

#ifndef MIMSERVERLOGIC_H
#define MIMSERVERLOGIC_H

#include <QObject>
#include <QRegion>

class MImAbstractServerLogic : public QObject
{
    Q_OBJECT

public:
    explicit MImAbstractServerLogic(QObject *parent = 0);
    virtual ~MImAbstractServerLogic() = 0;

public Q_SLOTS:
    virtual void inputPassthrough(const QRegion &region) = 0;
    virtual void appOrientationAboutToChange(int toAngle) = 0;
    virtual void appOrientationChangeFinished(int toAngle) = 0;
    virtual void applicationFocusChanged(WId remoteWinId) = 0;
    virtual void pluginLoaded() = 0;

Q_SIGNALS:
    void applicationWindowGone();
};

#endif // MIMSERVERLOGIC_H
