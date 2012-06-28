/* * This file is part of Maliit framework *
 *
 * Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef MIMREMOTEWINDOW_H
#define MIMREMOTEWINDOW_H

#include <QObject>
#include <QWidget>

struct MImServerXOptions;
class MImXServerLogic;

/*! \internal
 * \ingroup maliitserver
 * \brief A helper class to handle remote application windows
 */
class MImRemoteWindow : public QObject
{
    Q_OBJECT
public:
    //! Constructs a MImRemoteWindow from a X window.

    explicit MImRemoteWindow(WId window,
                             MImXServerLogic *serverLogic,
                             const MImServerXOptions &options);
    virtual ~MImRemoteWindow();

    //! Sets the \a widget transient to the remote application window
    void setIMWidget(const QWidget *widget);

    //! Returns whether the \a event changed the state of the remote application
    //! window to Iconic
    bool wasIconified(XEvent *event) const;
    //! Returns whether the the state of the remote application window is Iconic
    bool isIconified() const;
    //! Returns whether the \a event unmapped the remote application window
    bool wasUnmapped(XEvent *event) const;

    //! Enables redirection of the remote application window and starts listening
    //! to damage events.
    //! By default this function assumes that the window is already redirected by
    //! the window manager else meego-im-uiserver needs to be started with the
    //! -manual-redirection option.
    void redirect();
    //! Disables redirection of the remote application window
    void unredirect();

    //! Handle the XEvent \a event
    void handleEvent(XEvent *event);

    //! Returns the window pixmap while the remote application window is redirected
    virtual const QPixmap &windowPixmap() const;

    //! Returns the X11 window Id of the redirected window.
    WId id() const { return wid; }

    //! Returns the current redirection status
    bool isRedirected() { return redirected; }

    //! Resets the window pixmap
    void resetPixmap();

    //! Schedules screen update for given \a region.
    void update(const QRegion &region);

Q_SIGNALS:
    //! Signals when the content (pixmap) of the remote application window is updated
    void contentUpdated(const QRegion &region);

private:
    //! Creates the window pixmap
    void setupPixmap();
    //! Destroys the window pixmap
    void destroyPixmap();

    //! Start listening for damage events
    void setupDamage();
    //! Stops listening for damage events
    void destroyDamage();

    //! Handle the configure \a event
    void handleConfigureNotifyEvent(XEvent *event);

    //! Handle the damage \a event
    void handleDamageEvent(XEvent *event);

    WId wid;

    Qt::HANDLE xpixmap;
    Qt::HANDLE damage;

    QPixmap pixmap;
    bool redirected;

    const MImServerXOptions &xOptions;
    MImXServerLogic *mServerLogic;
};
//! \internal_end

#endif // MIMREMOTEWINDOW_H
