/* * This file is part of dui-im-framework *
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
#ifndef DUIPASSTHRUWINDOW_H
#define DUIPASSTHRUWINDOW_H

#include <QWidget>
#include <QDebug>

#include <X11/Xlib.h>

/*!
 * \brief DuiPassThruWindow uses XFixes to redirect mouse events to VKB
 */
class DuiPassThruWindow: public QWidget
{
    Q_OBJECT

public:
    /*!
     * \brief Constructor
     * \param p QWidget* parent widget
     */
    explicit DuiPassThruWindow(bool bypassWMHint = false, QWidget *p = 0);

    //! Destructor
    ~DuiPassThruWindow();

public slots:
    //! Set window ID for given region
    void inputPassthrough(const QRegion &region);

private:
    Q_DISABLE_COPY(DuiPassThruWindow);

    int displayWidth;
    int displayHeight;

    friend class Ut_PassthroughServer;
};

#endif
