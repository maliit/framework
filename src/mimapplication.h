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

#include <memory>
#include <tr1/functional>

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

    virtual QWidget *toplevel() const;
    virtual QWidget *pluginsProxyWidget() const;

    virtual bool selfComposited() const;

    virtual const QPixmap &remoteWindowPixmap();

    //! Visits all widgets in the hierarchy of widget, using visitor.
    //! Defaults to passthru window, if no widget is specified.
    static void visitWidgetHierarchy(WidgetVisitor visitor,
                                     QWidget *widget = 0);

    //! Configures a widget (and its widget hierarchy) for (self) compositing.
    //! Defaults to passthru window, if no widget is specified.
    static void configureWidgetsForCompositing(QWidget *widget = 0);

Q_SIGNALS:
    void applicationWindowGone();

private Q_SLOTS:
    virtual void finalize();

private:
    bool initializeComposite();
};
//! \internal_end

#endif // MIM_APPLICATION_H
