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
#include <QRegion>
#include <QVector>
#include <QWindow>

#include "abstractplatform.h"
#include "windowgroup.h"
#include "windowdata.h"

namespace Maliit
{

class WindowGroupPrivate
{
public:
    WindowGroupPrivate(const QSharedPointer<AbstractPlatform> &platform);

    bool containsWindow(QWindow *window);

    QSharedPointer<AbstractPlatform> m_platform;
    QVector<WindowData> m_window_list;
    QRegion m_last_im_area;
    bool m_active;
};

WindowGroupPrivate::WindowGroupPrivate(const QSharedPointer<AbstractPlatform> &platform)
    : m_platform(platform),
      m_window_list(),
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

WindowGroup::WindowGroup(const QSharedPointer<AbstractPlatform> &platform)
    : d_ptr(new WindowGroupPrivate(platform))
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

    if (window) {
        if (not d->containsWindow(window)) {
            QWindow *parent = window->parent ();

            if (parent and not d->containsWindow(parent)) {
                qWarning () << "Plugin is misbehaving - tried to register a window with yet-unregistered parent!";
                return;
            }
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
            d->m_platform->setupInputPanel(window, position);
            updateInputMethodArea();
        }
    }
}

void WindowGroup::setScreenRegion(const QRegion &region, QWindow *window)
{
    Q_D(WindowGroup);
    if (window == 0 && d->m_window_list.size() > 0) {
        window = d->m_window_list.at(0).m_window.data();
    }
    d->m_platform->setInputRegion(window, region);
}

void WindowGroup::setInputMethodArea(const QRegion &region, QWindow *window)
{
    Q_D(WindowGroup);

    if (window == 0 && d->m_window_list.size() > 0) {
        window = d->m_window_list.at(0).m_window.data();
    }

    for (int i = 0; i < d->m_window_list.size(); ++i) {
        WindowData &data = d->m_window_list[i];
        if (data.m_window == window) {
            data.m_inputMethodArea = region;
            break;
        }
    }

    if (d->m_active) {
        updateInputMethodArea();
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
            data.m_window->isVisible() and
            not data.m_inputMethodArea.isEmpty()) {
            new_area |= data.m_inputMethodArea.translated(data.m_window->position());
        }
    }

    if (new_area != d->m_last_im_area) {
        d->m_last_im_area = new_area;
        Q_EMIT inputMethodAreaChanged(d->m_last_im_area);
    }
}

} // namespace Maliit
