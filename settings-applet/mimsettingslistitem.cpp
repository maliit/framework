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

#include "mimsettingslistitem.h"

#include <MLabel>
#include <MImageWidget>
#include <QGraphicsGridLayout>
#include <QDebug>

MImSettingsListItem::MImSettingsListItem(MBasicListItem::ItemStyle style,
                                         QGraphicsItem *parent)
    : MBasicListItem(style, parent), layout(NULL), stretchItem(NULL)
{
}

QGraphicsLayout *MImSettingsListItem::createLayout()
{
    if (!layout) {
        layout = new QGraphicsGridLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(0);
        stretchItem = new QGraphicsWidget(this);
    } else {
        clearLayout();
    }

    switch (itemStyle()) {
        case IconWithTitleAndSubtitle:
            layout->addItem(titleLabelWidget(), 0, 0);
            layout->addItem(subtitleLabelWidget(), 1, 0);
            layout->addItem(stretchItem, 2, 0);
            layout->addItem(imageWidget(), 0, 1, 3, 1, Qt::AlignCenter);
            break;

        default:
            // Not supported
            qWarning() << __PRETTY_FUNCTION__ << "Unsupported item style";
            break;
    }

    return layout;
}

void MImSettingsListItem::clearLayout()
{
    for (int i = layout->count() - 1; i >= 0; i--) {
        layout->removeAt(i);
    }
}

