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

namespace
{

struct WindowData
{
    // apparently QVector<WindowData>::append() needs it...
    WindowData();
    WindowData(QWindow *window, Maliit::Position position);

    QPointer<QWindow> m_window;
    Maliit::Position m_position;
};

WindowData::WindowData()
    : m_window(),
      m_position(Maliit::PositionCenterBottom)
{}

WindowData::WindowData(QWindow *window, Maliit::Position position)
    : m_window(window),
      m_position(position)
{}

} // anonymous namespace

class WindowGroupPrivate
{
public:
    WindowGroupPrivate();

    bool containsWindow(QWindow *window);

    QVector<WindowData> m_window_list;
    QRegion m_last_im_area;
    bool m_active;
};

WindowGroupPrivate::WindowGroupPrivate()
    : m_window_list(),
      m_last_im_area(),
      m_active(false)
{}

bool WindowGroupPrivate::containsWindow(QWindow *window)
{
    Q_FOREACH (const WindowData &data, m_window_list) {
        if (data.m_window == window) {
            return true;
        }
    }

    return false;
}

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

        Q_FOREACH (const WindowData &data, d->m_window_list) {
            if (data.m_window) {
                data.m_window->setVisible (false);
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
        if (not d->containsWindow(window)) {
            d->m_window_list.append (WindowData(window, position));

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

    Q_FOREACH (const WindowData &data, d->m_window_list) {
        if (data.m_window and not data.m_window->parent() and
            data.m_window->isVisible()) {
            new_area |= data.m_window->geometry();
        }
    }

    if (new_area != d->m_last_im_area) {
        d->m_last_im_area = new_area;
        Q_EMIT inputMethodAreaChanged(d->m_last_im_area);
    }
}

} // namespace Maliit
