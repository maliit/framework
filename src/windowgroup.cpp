/* * This file is part of Maliit framework *
 *
 * Copyright (C) 2013 Openismus GmbH
 *
 * Contact: maliit-discuss@lists.maliit.org
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 */

#include <QDebug>
#include <QPointer>
#include <QRegion>
#include <QVector>
#include <QWindow>

#include "windowgroup.h"

namespace Maliit
{

class WindowGroupPrivate
{
public:
    WindowGroupPrivate();

    QVector<QPointer<QWindow> > m_window_list;
    QRegion m_last_im_area;
    bool m_active;
};

WindowGroupPrivate::WindowGroupPrivate()
    : m_window_list(),
      m_last_im_area(),
      m_active(false)
{}

WindowGroup::WindowGroup()
    : d_ptr(new WindowGroupPrivate)
{}

WindowGroup::~WindowGroup()
{}

void WindowGroup::activate()
{
    Q_D(WindowGroup);

    d->m_active = true;
}

void WindowGroup::deactivate()
{
    Q_D(WindowGroup);

    if (d->m_active) {
        d->m_active = false;

        Q_FOREACH (const QPointer<QWindow> &window, d->m_window_list) {
            if (window) {
                window->setVisible (false);
            }
        }
        updateInputMethodArea();
    }
}

void WindowGroup::setupWindow(QWindow *window, Maliit::Position position)
{
    Q_D(WindowGroup);

    Q_UNUSED (position);

    if (window) {
        if (d->m_window_list.indexOf (window) == -1) {
            d->m_window_list.append (window);

            window->setFlags (Qt::Dialog |
                              Qt::FramelessWindowHint |
                              Qt::WindowStaysOnTopHint |
                              Qt::X11BypassWindowManagerHint |
                              Qt::WindowDoesNotAcceptFocus);

            connect (window, SIGNAL (visibleChanged(bool)),
                     this, SLOT (onVisibleChanged(bool)));
            connect (window, SIGNAL (heightChanged(int)),
                     this, SLOT (updateInputMethodArea()));
            connect (window, SIGNAL (widthChanged(int)),
                     this, SLOT (updateInputMethodArea()));
            connect (window, SIGNAL (xChanged(int)),
                     this, SLOT (updateInputMethodArea()));
            connect (window, SIGNAL (yChanged(int)),
                     this, SLOT (updateInputMethodArea()));
            updateInputMethodArea();
        }
    }
}

void WindowGroup::onVisibleChanged(bool visible)
{
    Q_D(WindowGroup);

    if (d->m_active) {
        updateInputMethodArea();
    } else if (visible) {
        QWindow *window = qobject_cast<QWindow*>(sender());

        if (window) {
            qWarning () << "An inactive plugin is misbehaving - tried to show a window!";
            window->setVisible (false);
        }
    }
}

void WindowGroup::updateInputMethodArea()
{
    Q_D(WindowGroup);
    QRegion new_area;

    Q_FOREACH (const QPointer<QWindow> &window, d->m_window_list) {
        if (window and window->parent() and window->isVisible()) {
            new_area |= window->geometry();
        }
    }

    if (new_area != d->m_last_im_area) {
        d->m_last_im_area = new_area;
        Q_EMIT inputMethodAreaChanged(d->m_last_im_area);
    }
}

} // namespace Maliit
