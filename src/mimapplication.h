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

#ifndef MIM_APPLICATION_H
#define MIM_APPLICATION_H

#include <MApplication>

#include <QPointer>

#include "mimxextension.h"

class MIMApplication;
#if defined(mApp)
#undef mApp
#endif
#define mApp (static_cast<MIMApplication *>(QCoreApplication::instance()))

class MImRemoteWindow;

//! \internal
/*! \brief A helper class to filter X11 events
 *
 *  This class allows the meego-im-uiserver to make itself transient to the
 *  remote application window.
 */
class MIMApplication
    : public MApplication
{
    Q_OBJECT

public:
    /*
     * The command line arguments need to stay valid for the whole life-time of
     * the application.
     * That's why it is important to pass argc by reference, otherwise
     * QApplication and QCoreApplication *will* break (e.g., when using
     * QCoreApplication::arguments(),  in the case of filtered command line
     * arguments).
     */
    explicit MIMApplication(int &argc, char** argv, bool initializeComposite = false);
    virtual ~MIMApplication();

    //! Requires a valid remoteWinId and a valid passThruWindow before it'll
    //! start to work.
    //! \sa setTransientHint(), setPassThruWindow();
    bool x11EventFilter(XEvent *ev);

    void setTransientHint(WId remoteWinId);
    void setPassThruWindow(QWidget *passThruWindow);

    static MIMApplication *instance();

    bool selfComposited() const;

    const MImXCompositeExtension& compositeExtension() { return composite_extension; }
    const MImXDamageExtension& damageExtension() { return damage_extension; }
signals:
    //! After a map window request (e.g., via show()) this signal gets emitted
    //! as soon as X mapped our passthru window.
    void passThruWindowMapped();

    //! After an unmap window request (e.g., via hide()) this signal gets
    //! emitted as soon as X unmapped our passthru window.
    void passThruWindowUnmapped();

    //! This signal is emitted when remote window is changed
    void remoteWindowChanged(MImRemoteWindow *newWindow);

    //! This signal is emitted when remote window is gone
    void remoteWindowGone();
private:
    void handleMapNotifyEvents(XEvent *ev);
    void handleTransientEvents(XEvent *ev);
    void handleDamageEvents(XEvent *ev);

    bool initializeComposite();

    bool wasPassThruWindowMapped(XEvent *ev) const;
    bool wasPassThruWindowUnmapped(XEvent *ev) const;

    QPointer<QWidget> passThruWindow;
    MImRemoteWindow *remoteWindow;
    MImXCompositeExtension composite_extension;
    MImXDamageExtension damage_extension;
    bool self_composited;
    friend class Ut_PassthroughServer;
};
//! \internal_end

#endif // MIM_APPLICATION_H
