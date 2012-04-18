/* * This file is part of meego-im-framework *
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

#ifndef MALIIT_SERVER_ABSTRACTSURFACEGROUPFACTORY_H
#define MALIIT_SERVER_ABSTRACTSURFACEGROUPFACTORY_H

#include <QSharedPointer>

namespace Maliit {
namespace Server {

class AbstractSurfaceGroup;

class AbstractSurfaceGroupFactory
{
public:
    virtual ~AbstractSurfaceGroupFactory();

    virtual QSharedPointer<AbstractSurfaceGroup> createSurfaceGroup() = 0;
};

} // namespace Server
} // namespace Maliit

#endif // MALIIT_SERVER_ABSTRACTSURFACEGROUPFACTORY_H
