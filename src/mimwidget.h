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

#ifndef MIMWIDGET_H
#define MIMWIDGET_H

#include <QWidget>
#include <QPixmap>
#include <QRegion>
#include <QPaintEvent>
#include <QRectF>

class MImWidgetPrivate;

//! A QWidget implementation that can be used for input methods
//! using MAbstractInputMethod.
//! Need to call MImWidget::paintEvent when overriding paintEvent.
class MImWidget
    : public QWidget
{
    Q_OBJECT
public:
    //! C'tor
    //! \param parent the parent widget
    //! \param f the window flags
    explicit MImWidget(QWidget *parent = 0,
                       Qt::WindowFlags f = 0);

    //! D'tor
    virtual ~MImWidget();

protected:
    //! \reimp
    virtual void paintEvent(QPaintEvent *event);
    //! \reimp_end

    //! Protected c'tor that allows to re-use private class
    //! \param dd the PIMPL instance
    //! \param parent the parent widget
    //! \param f the window flags
    MImWidget(MImWidgetPrivate *dd,
              QWidget *parent,
              Qt::WindowFlags f = 0);

    MImWidgetPrivate *const d_ptr; //!< PIMPL

private:
    void init();

    Q_DISABLE_COPY(MImWidget);
    Q_DECLARE_PRIVATE(MImWidget);
};

#endif // MIMWIDGET_H
