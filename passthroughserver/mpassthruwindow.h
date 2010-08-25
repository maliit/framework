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
#include <QPointer>

class MPassThruWindow;

// Helper class to work around NB#173434 and Qt/X11 problems wrt hide/show.
class MIMWindowManager
    : public QObject
{
    Q_OBJECT

public:
    //! The MIMWindowManager processes hide/show requests of MPassThruWindow,
    //! depending on the RequestType.
    enum RequestType
    {
        NONE,   //!< Default state, does nothing.
        RETRY,  //!< Use whatever request type is stored in state.
                //!< *Not* a valid value for state itself, though.
        SHOW,   //!< Tries aggressively to show the parent widget.
        HIDE    //!< Tries aggressively to hide the parent widget.
    };

    //! Our WM can only manage one window - the passthru window:
    explicit MIMWindowManager(MPassThruWindow *parent = 0);

    //! Returns the current request state.
    RequestType requestState();

public slots:
    //! Tries aggressively to show the parent widget.
    void showRequest();

    //! Tries aggressively to hide the parent widget.
    void hideRequest();

private:
    RequestType state;

    int retryCount;
    QTimer waitForNotify;

private slots:
    //! Depending on the state, either tries to show or hide the parent
    void showHideRequest(RequestType rt = RETRY);

    //! Cancels window show/hide retry loop
    void cancelRequest();
};

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

    //! Set the MIMWindowManager instance, MPassThruWindow takes ownership
    void setMIMWindowManager(const QPointer<MIMWindowManager> &wm);

public slots:
    //! Set window ID for given region
    void inputPassthrough(const QRegion &region);

private:
    Q_DISABLE_COPY(MPassThruWindow);

    int displayWidth;
    int displayHeight;

    QPointer<MIMWindowManager> wm;

    friend class Ut_PassthroughServer;
};

#endif
