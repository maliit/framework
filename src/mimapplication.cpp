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

#include "mimapplication.h"

#include <QDebug>

#include <deque>

namespace
{
    bool configureForCompositing(QWidget *w)
    {
        if (not w) {
            return false;
        }

        w->setAttribute(Qt::WA_OpaquePaintEvent);
        w->setAttribute(Qt::WA_NoSystemBackground);
        w->setAutoFillBackground(false);
        // Be aware that one cannot verify whether the background role *is*
        // QPalette::NoRole - see QTBUG-17924.
        w->setBackgroundRole(QPalette::NoRole);

        if (MIMApplication::instance() && not MIMApplication::instance()->selfComposited()) {
            // Careful: This flag can trigger a call to
            // qt_x11_recreateNativeWidgetsRecursive
            // - which will crash when it tries to get the effective WId
            // (as none of widgets have been mapped yet).
            w->setAttribute(Qt::WA_TranslucentBackground);
        }

        return true;
    }
}

MIMApplication::MIMApplication(int &argc, char **argv)
    : QApplication(argc, argv)

{
    connect(this, SIGNAL(aboutToQuit()),
            this, SLOT(finalize()),
            Qt::UniqueConnection);
}

MIMApplication::~MIMApplication()
{
}

void MIMApplication::finalize()
{
}

void MIMApplication::setTransientHint(WId)
{
}

QWidget* MIMApplication::toplevel() const
{
    return 0;
}

QWidget* MIMApplication::pluginsProxyWidget() const
{
    return 0;
}

MIMApplication *MIMApplication::instance()
{
    return static_cast<MIMApplication *>(QCoreApplication::instance());
}

bool MIMApplication::selfComposited() const
{
    return false;
}

const QPixmap &MIMApplication::remoteWindowPixmap()
{
    static const QPixmap empty;
    return empty;
}

void MIMApplication::visitWidgetHierarchy(WidgetVisitor visitor,
                                          QWidget *widget)
{
    if (not MIMApplication::instance()) {
        return;
    }

    std::deque<QWidget *> unvisited;
    unvisited.push_back(widget ? widget : MIMApplication::instance()->toplevel());

    // Breadth-first traversal of widget hierarchy, until no more
    // unvisited widgets remain. Will find viewports of QGraphicsViews,
    // as QAbstractScrollArea reparents the viewport to itself.
    while (not unvisited.empty()) {
        QWidget *current = unvisited.front();
        unvisited.pop_front();

        // If true, then continue walking the hiearchy of current widget.
        if (visitor(current)) {
            // Mark children of current widget as unvisited:
            Q_FOREACH (QObject *obj, current->children()) {
                if (QWidget *w = qobject_cast<QWidget *>(obj)) {
                    unvisited.push_back(w);
                }
            }
        }
    }
}

void MIMApplication::configureWidgetsForCompositing(QWidget *widget)
{
    MIMApplication::visitWidgetHierarchy(configureForCompositing, widget);
}

