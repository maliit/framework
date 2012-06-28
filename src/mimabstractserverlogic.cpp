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

#include "mimabstractserverlogic.h"

#include <deque>
#include <QWidget>

void visitWidgetHierarchy(WidgetVisitor visitor,
                          QWidget *widget)
{
    std::deque<QWidget *> unvisited;
    unvisited.push_back(widget);

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

MImAbstractServerLogic::MImAbstractServerLogic(QObject *parent) :
    QObject(parent)
{
}

MImAbstractServerLogic::~MImAbstractServerLogic()
{
}
