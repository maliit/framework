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
#ifndef MPASSTHRUWINDOW_H
#define MPASSTHRUWINDOW_H

#include <QWidget>
#include <QDebug>
#include <QRegion>
#include <QTimer>

/*!
 * \brief MPassThruWindow uses XFixes to redirect mouse events to VKB
 */
class MPassThruWindow: public QWidget
{
    Q_OBJECT

public:
    /*!
     * \brief Constructor
     * \param p QWidget* parent widget
     */
    explicit MPassThruWindow(bool bypassWMHint = false, QWidget *p = 0);

    //! Destructor
    ~MPassThruWindow();


public slots:
    //! Set window ID for given region
    void inputPassthrough(const QRegion &region);

private slots:
    //! Cancels window show retry loop
    void cancelShowRequest();

    //! Tries to show this window
    void showRequest();

private:
    Q_DISABLE_COPY(MPassThruWindow);

    int displayWidth;
    int displayHeight;
    QTimer waitForMapNotify;
    int showRetryCount;

    friend class Ut_PassthroughServer;
};

#endif
