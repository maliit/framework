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

#include <QApplication>
#include <QWidget>
#include <QPixmap>
#include <QRect>
#include <QPointer>
#include <memory>
#include <tr1/functional>

#include "mpassthruwindow.h"
#include "mimremotewindow.h"
#include "mimxextension.h"

class MIMApplication;
#if defined(mApp)
#undef mApp
#endif
#define mApp (static_cast<MIMApplication *>(QCoreApplication::instance()))

//! \internal
/*! \brief A helper class to filter X11 events
 *
 *  This class allows the meego-im-uiserver to make itself transient to the
 *  remote application window.
 */
class MIMApplication
    : public QApplication
{
    Q_OBJECT

public:
    //! Walks over widget hierarchy, if used with
    //! MIMApplication::visitWidgetHierarchy. Return true if children of
    //! current widget shall be visited, too.
    typedef std::tr1::function<bool (QWidget *)> WidgetVisitor;

    /*
     * The command line arguments need to stay valid for the whole life-time of
     * the application.
     * That's why it is important to pass argc by reference, otherwise
     * QApplication and QCoreApplication *will* break (e.g., when using
     * QCoreApplication::arguments(),  in the case of filtered command line
     * arguments).
     */
    explicit MIMApplication(int &argc, char** argv);
    virtual ~MIMApplication();
    static MIMApplication *instance();

    //! Requires a valid remoteWinId and a valid passThruWindow before it'll
    //! start to work.
    //! \sa setTransientHint(), setPassThruWindow();
    bool x11EventFilter(XEvent *ev);

    void setTransientHint(WId remoteWinId);
    QWidget *passThruWindow() const;
    QWidget* pluginsProxyWidget() const;

    bool selfComposited() const;
    bool manualRedirection() const;
    bool bypassWMHint() const;

    //! Flag that is used to sync between MAbstractInputMethodHost and
    //! rotation animation in order to capture the VKB without
    //! self-composited background drawn.
    void setSuppressBackground(bool suppress);

#ifdef UNIT_TEST
    MImRemoteWindow *remoteWindow() const;
#endif

    static const QPixmap &remoteWindowPixmap();

    const MImXCompositeExtension& compositeExtension() { return mCompositeExtension; }
    const MImXDamageExtension& damageExtension() { return mDamageExtension; }

    //! Visits all widgets in the hierarchy of widget, using visitor.
    //! Defaults to passthru window, if no widget is specified.
    static void visitWidgetHierarchy(WidgetVisitor visitor,
                                     QWidget *widget = 0);

    //! Configures a widget (and its widget hierarchy) for (self) compositing.
    //! Defaults to passthru window, if no widget is specified.
    static void configureWidgetsForCompositing(QWidget *widget = 0);

    //! Flag that is used to mask the plugin region with 
    //! background
    void setMasked(bool masked); 

signals:
    //! This signal is emitted when remote window is changed.
    //! Parameter can be 0 if window is unmapped.
    void remoteWindowChanged(MImRemoteWindow *newWindow);

private slots:
    void updatePassThruWindow(const QRegion &region);
    void finalize();

private:
    void parseArguments(int &argc, char** argv);

    void handleTransientEvents(XEvent *ev);
    void handleRemoteWindowEvents(XEvent *ev);
    void handlePassThruMapEvent(XEvent *ev);

    bool initializeComposite();

    std::auto_ptr<MPassThruWindow> mPassThruWindow;
    std::auto_ptr<MImRemoteWindow> mRemoteWindow;
    std::auto_ptr<QWidget> mPluginsProxyWidget;
    MImXCompositeExtension mCompositeExtension;
    MImXDamageExtension mDamageExtension;
    bool mSelfComposited;
    bool mManualRedirection;
    bool mBypassWMHint;
    bool mBackgroundSuppressed;
    bool mMasked;

    friend class Ut_PassthroughServer;
};
//! \internal_end

#endif // MIM_APPLICATION_H
