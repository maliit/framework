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


#include "mimscene.h"
#include <QEvent>

MImScene::MImScene(QObject *parent)
    : MScene(parent)
{
}

bool MImScene::event(QEvent *event)
{
    // This is another workaround to fix NB#220440
    // We should only filter the TouchBegin and MousePress
    // events to QGraphicsScene, other events should be still
    // sent to MScene.
    switch (event->type()) {
    case QEvent::TouchBegin:
    case QEvent::GraphicsSceneMousePress:
        return QGraphicsScene::event(event);
    default:
        return MScene::event(event);
    }
}

