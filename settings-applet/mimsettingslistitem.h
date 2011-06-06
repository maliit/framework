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

#ifndef MIMSETTINGSLISTITEM_H
#define MIMSETTINGSLISTITEM_H

#include <QObject>
#include <MBasicListItem>

class QGraphicsGridLayout;
class QGraphicsWidget;

/*!
 * A simple list item widget that shows icon in the right side of the labels.
 */
class MImSettingsListItem : public MBasicListItem {
Q_OBJECT

public:
    MImSettingsListItem(MBasicListItem::ItemStyle style = IconWithTitleAndSubtitle,
                        QGraphicsItem *parent = 0);

protected:
    //! reimp
    virtual QGraphicsLayout *createLayout();
    //! reimp_end

private:
    void clearLayout();

    QGraphicsGridLayout *layout;
    QGraphicsWidget *stretchItem;
};

#endif
