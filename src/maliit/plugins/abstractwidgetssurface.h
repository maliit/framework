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

#ifndef MALIIT_PLUGINS_ABSTRACTWIDGETSSURFACE_H
#define MALIIT_PLUGINS_ABSTRACTWIDGETSSURFACE_H

#include <maliit/plugins/abstractsurface.h>

class QGraphicsItem;
class QGraphicsScene;
class QGraphicsView;
class QWidget;

namespace Maliit {
namespace Plugins {

class AbstractWidgetSurface : public virtual AbstractSurface
{
public:
    virtual QWidget *widget() const = 0;
};

class AbstractGraphicsViewSurface : public virtual AbstractSurface
{
public:
    virtual QGraphicsScene *scene() const = 0;
    virtual QGraphicsView *view() const = 0;

    virtual QGraphicsItem *root() const = 0;
    virtual void clear() = 0;
};

} // namespace Plugins
} // namespace Maliit

#endif // MALIIT_PLUGINS_ABSTRACTWIDGETSSURFACE_H
