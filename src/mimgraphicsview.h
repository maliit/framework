/* * This file is part of meego-im-framework *
 *
 * Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef MIMGRAPHICSVIEW_H
#define MIMGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QWidget>
#include <QPixmap>
#include <QRegion>
#include <QPainter>
#include <QRectF>

//! A QGraphicsView implementation that can be used for input methods
//! using MAbstractInputMethod.
class MImGraphicsViewPrivate;

class MImGraphicsView
    : public QGraphicsView
{
    Q_OBJECT
public:
    //! C'tor
    //! \param parent the parent widget
    explicit MImGraphicsView(QWidget *parent = 0);

    //! C'tor
    //! \param scene the scene for this view
    //! \param parent the parent widget
    explicit MImGraphicsView(QGraphicsScene * scene,
                             QWidget *parent = 0);

    //! D'tor
    virtual ~MImGraphicsView();

protected:
    //! \reimp
    virtual void drawBackground(QPainter *painter,
                                const QRectF &rect);
    //! \reimp_end

    //! Protected c'tor that allows to re-use private class
    //! \param dd the PIMPL instance
    //! \param scene the scene for this view
    //! \param widget the parent widget
    MImGraphicsView(MImGraphicsViewPrivate *dd,
                    QGraphicsScene *scene,
                    QWidget *parent);

    MImGraphicsViewPrivate *const d_ptr; //!< PIMPL

private:
    void init();

    Q_DISABLE_COPY(MImGraphicsView);
    Q_DECLARE_PRIVATE(MImGraphicsView);
};

#endif // MIMGRAPHICSVIEW_H
