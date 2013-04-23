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

#include "abstractplatform.h"
#include "windowgroup.h"

namespace Maliit
{

WindowGroup::WindowGroup(const QSharedPointer<AbstractPlatform> &platform)
    : m_platform(platform),
      m_active(false)
{
    m_hideTimer.setSingleShot(true);
    m_hideTimer.setInterval(2000);
    connect(&m_hideTimer, SIGNAL(timeout()), this, SLOT(hideWindows()));
}

WindowGroup::~WindowGroup()
{}

void WindowGroup::activate()
{
    m_active = true;
    m_hideTimer.stop();
}

void WindowGroup::deactivate(HideMode mode)
{
    if (m_active) {
        m_active = false;

        if (mode == HideImmediate) {
            hideWindows();
        } else {
            m_hideTimer.start();
        }
    }
}

void WindowGroup::setupWindow(QWindow *window, Maliit::Position position)
{
    if (window) {
        if (not containsWindow(window)) {
            QWindow *parent = window->parent ();

            if (parent and not containsWindow(parent)) {
                qWarning () << "Plugin is misbehaving - tried to register a window with yet-unregistered parent!";
                return;
            }
            m_window_list.append (WindowData(window, position));

            window->setFlags (Qt::FramelessWindowHint |
                              Qt::WindowStaysOnTopHint |
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
            m_platform->setupInputPanel(window, position);
            updateInputMethodArea();
        }
    }
}

void WindowGroup::setScreenRegion(const QRegion &region, QWindow *window)
{
    if (window == 0 && m_window_list.size() > 0) {
        window = m_window_list.at(0).m_window.data();
    }
    m_platform->setInputRegion(window, region);
}

void WindowGroup::setInputMethodArea(const QRegion &region, QWindow *window)
{
    if (window == 0 && m_window_list.size() > 0) {
        window = m_window_list.at(0).m_window.data();
    }

    for (int i = 0; i < m_window_list.size(); ++i) {
        WindowData &data = m_window_list[i];
        if (data.m_window == window) {
            data.m_inputMethodArea = region;
            break;
        }
    }

    if (m_active) {
        updateInputMethodArea();
    }
}

void WindowGroup::setApplicationWindow(WId id)
{
    Q_FOREACH (const WindowData &data, m_window_list) {
        if (data.m_window and not data.m_window->parent()) {
            m_platform->setApplicationWindow(data.m_window, id);
        }
    }
}

void WindowGroup::onVisibleChanged(bool visible)
{
    if (m_active) {
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
    QRegion new_area;

    Q_FOREACH (const WindowData &data, m_window_list) {
        if (data.m_window and not data.m_window->parent() and
            data.m_window->isVisible() and
            not data.m_inputMethodArea.isEmpty()) {
            new_area |= data.m_inputMethodArea.translated(data.m_window->position());
        }
    }

    if (new_area != m_last_im_area) {
        m_last_im_area = new_area;
        Q_EMIT inputMethodAreaChanged(m_last_im_area);
    }
}

bool WindowGroup::containsWindow(QWindow *window)
{
    Q_FOREACH (const WindowData &data, m_window_list) {
        if (data.m_window == window) {
            return true;
        }
    }

    return false;
}

void WindowGroup::hideWindows()
{
    m_hideTimer.stop();

    Q_FOREACH (const WindowData &data, m_window_list) {
        if (data.m_window) {
            data.m_window->setVisible (false);
        }
    }
    updateInputMethodArea();
}

} // namespace Maliit
