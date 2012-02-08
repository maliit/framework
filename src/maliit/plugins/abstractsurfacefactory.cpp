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
#include "abstractsurfacefactory.h"

#include <QApplication>
#include <QDesktopWidget>

namespace Maliit {
namespace Plugins {

AbstractSurfaceFactory::~AbstractSurfaceFactory()
{
}

QSize AbstractSurfaceFactory::screenLandscapeSize() const
{
    return QApplication::desktop()->size();
}

QSize AbstractSurfaceFactory::screenPortraitSize() const
{
    QSize size(QApplication::desktop()->size());

    size.transpose();

    return size;
}

} // namespace Plugins
} // namespace Maliit
