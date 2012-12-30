/* * This file is part of Maliit framework *
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 *
 * Contact: maliit-discuss@lists.maliit.org
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
#include <QRegion>
#include <QCloseEvent>

class MImRemoteWindow;
struct MImServerXOptions;
class MImXServerLogic;

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

    explicit MPassThruWindow(MImXServerLogic *serverLogic,
                             const MImServerXOptions &options);

    //! Destructor
    ~MPassThruWindow();

    virtual bool event(QEvent *e);
    void closeEvent(QCloseEvent *ev);

    //! Return region occupied by current plugin
    const QRegion &region();

public Q_SLOTS:
    //! Set window ID for given region
    void inputPassthrough(const QRegion &region = QRegion());

    //! Set the new remote window (application window)
    void setRemoteWindow(MImRemoteWindow *remoteWindow = 0);

    //! Schedules repainting for widget's background from remote window
    void updateFromRemoteWindow(const QRegion &region);

private:
    Q_DISABLE_COPY(MPassThruWindow)

    void updateInputRegion();
    void updateWindowType();

    MImRemoteWindow *remoteWindow;
    QRegion mRegion;
    MImXServerLogic *mServerLogic;
    const MImServerXOptions &xOptions;

    friend class Ut_PassthroughServer;
};

#endif
