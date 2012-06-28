/* * This file is part of maliit-framework *
 *
 * Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef MALIIT_SERVER_ABSTRACTSURFACEGROUP_H
#define MALIIT_SERVER_ABSTRACTSURFACEGROUP_H

#include <maliit/namespace.h>
#include <maliit/plugins/abstractsurfacefactory.h>

#include <QObject>

namespace Maliit {
namespace Server {

class AbstractSurfaceGroup : public QObject
{
    Q_OBJECT

public:
    AbstractSurfaceGroup();

    virtual Maliit::Plugins::AbstractSurfaceFactory *factory() = 0;

    virtual void activate() = 0;
    virtual void deactivate() = 0;

    virtual void setRotation(Maliit::OrientationAngle angle) = 0;

Q_SIGNALS:
    void inputMethodAreaChanged(const QRegion &inputMethodArea);
};

} // namespace Server
} // namespace Maliit

#endif // MALIIT_SERVER_ABSTRACTSURFACEGROUP_H
