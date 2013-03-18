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

#ifdef HAVE_WAYLAND
#include <QGuiApplication>
#include <qpa/qplatformnativeinterface.h>

#include <wayland-client.h>
#include "wayland-input-method-client-protocol.h"
#endif // HAVE_WAYLAND

#include "windowgroup.h"
#include "windowdata.h"

namespace Maliit
{

namespace
{

#ifdef HAVE_WAYLAND
input_panel_surface_position maliitToWestonPosition (Maliit::Position position)
{
    switch (position) {
    case Maliit::PositionCenterBottom:
        return INPUT_PANEL_SURFACE_POSITION_CENTER_BOTTOM;
    default:
        qWarning() << "Weston only supports center bottom position for top-level surfaces.";

        return INPUT_PANEL_SURFACE_POSITION_CENTER_BOTTOM;
    }
}
#endif // HAVE_WAYLAND

} // anonymous namespace

class WindowGroupPrivate
{
public:
    WindowGroupPrivate();
    ~WindowGroupPrivate();

    bool containsWindow(QWindow *window);
    Maliit::Position getPosition(QWindow *window, bool &found);
    QWindow *getTopLevelWindow();

#ifdef HAVE_WAYLAND
    void handleRegistryGlobal(uint32_t name,
                              const char *interface,
                              uint32_t version);
    void handleRegistryGlobalRemove(uint32_t name);

    bool setupInputSurface(QWindow *window, bool avoid_crash = false);

#endif // HAVE_WAYLAND

    QVector<WindowData> m_window_list;
    QRegion m_last_im_area;
    bool m_active;
#ifdef HAVE_WAYLAND
    struct wl_registry *m_registry;
    struct input_panel *m_panel;
#endif // HAVE_WAYLAND
};

#ifdef HAVE_WAYLAND
namespace {

void registryGlobal(void *data,
                    wl_registry *registry,
                    uint32_t name,
                    const char *interface,
                    uint32_t version)
{
    qDebug() << __PRETTY_FUNCTION__;
    WindowGroupPrivate *d = static_cast<WindowGroupPrivate *>(data);

    Q_UNUSED(registry);
    d->handleRegistryGlobal(name, interface, version);
}

void registryGlobalRemove(void *data,
                          wl_registry *registry,
                          uint32_t name)
{
    qDebug() << __PRETTY_FUNCTION__;
    WindowGroupPrivate *d = static_cast<WindowGroupPrivate *>(data);

    Q_UNUSED(registry);
    d->handleRegistryGlobalRemove(name);
}

const wl_registry_listener maliit_registry_listener = {
    registryGlobal,
    registryGlobalRemove
};

} // unnamed namespace

#endif // HAVE_WAYLAND

WindowGroupPrivate::WindowGroupPrivate()
    : m_window_list()
    , m_last_im_area()
    , m_active(false)
#ifdef HAVE_WAYLAND
    , m_registry(0)
    , m_panel(0)
#endif // HAVE_WAYLAND
{
#ifdef HAVE_WAYLAND
    wl_display *display = static_cast<wl_display *>(QGuiApplication::platformNativeInterface()->nativeResourceForIntegration("display"));
    if (!display) {
        qCritical() << __PRETTY_FUNCTION__ << "Failed to get a display.";
        return;
    }
    m_registry = wl_display_get_registry(display);
    wl_registry_add_listener(m_registry, &maliit_registry_listener, this);
    // QtWayland will do dispatching for us.
#endif // HAVE_WAYLAND
}

WindowGroupPrivate::~WindowGroupPrivate()
{
#ifdef HAVE_WAYLAND
    if (m_panel) {
        input_panel_destroy (m_panel);
    }
    if (m_registry) {
        wl_registry_destroy (m_registry);
    }
#endif // HAVE_WAYLAND
}

bool WindowGroupPrivate::containsWindow(QWindow *window)
{
    Q_FOREACH (const WindowData &data, m_window_list) {
        if (data.m_window == window) {
            return true;
        }
    }

    return false;
}

Maliit::Position WindowGroupPrivate::getPosition(QWindow *window, bool &found)
{
    Q_FOREACH (const WindowData &data, m_window_list) {
        if (data.m_window == window) {
            found = true;
            return data.m_position;
        }
    }

    found = false;
    return Maliit::PositionCenterBottom;
}

QWindow *WindowGroupPrivate::getTopLevelWindow()
{
    Q_FOREACH (const WindowData &data, m_window_list) {
        if (data.m_window and not data.m_window->parent()) {
            return data.m_window;
        }
    }

    return 0;
}

#ifdef HAVE_WAYLAND
void WindowGroupPrivate::handleRegistryGlobal(uint32_t name,
                                              const char *interface,
                                              uint32_t version)
{
    Q_UNUSED(version);

    qDebug() << __PRETTY_FUNCTION__ << "Name:" << name << "Interface:" << interface;
    if (!strcmp(interface, "input_panel")) {
        m_panel = static_cast<input_panel *>(wl_registry_bind(m_registry, name, &input_panel_interface, 1));

        QWindow *window = getTopLevelWindow ();

        if (window) {
            setupInputSurface(window, true);
        }
    }
}

void WindowGroupPrivate::handleRegistryGlobalRemove(uint32_t name)
{
    qDebug() << __PRETTY_FUNCTION__ << "Name:" << name;
    // TODO: We should destroy bound globals here.
}

bool WindowGroupPrivate::setupInputSurface(QWindow *window,
                                           bool avoid_crash)
{
    struct wl_surface *surface = avoid_crash ? 0 : static_cast<struct wl_surface *>(QGuiApplication::platformNativeInterface()->nativeResourceForWindow("surface", window));

    if (not surface) {
        if (avoid_crash) {
            qDebug() << "Creating surface to avoid crash in QtWayland";
        } else {
            qDebug() << "No wl_surface, calling create.";
        }
        qDebug() << __PRETTY_FUNCTION__ << "WId:" << window->winId();
        window->create();
    }

    surface = static_cast<struct wl_surface *>(QGuiApplication::platformNativeInterface()->nativeResourceForWindow("surface", window));
    qDebug() << __PRETTY_FUNCTION__ << "WId:" << window->winId();
    if (surface) {
        input_panel_surface *ip_surface = input_panel_get_input_panel_surface(m_panel, surface);
        bool found = false;
        Maliit::Position position (getPosition(window, found));

        if (found) {
            input_panel_surface_position weston_position = maliitToWestonPosition (position);

            input_panel_surface_set_toplevel(ip_surface, weston_position);
            return true;
        } else {
            qCritical() << __PRETTY_FUNCTION__ << "Looks like that window is not on ourlist.";
        }
    } else {
        qDebug() << __PRETTY_FUNCTION__ << "Still no surface, giving up.";
    }
    return false;
}

#endif // HAVE_WAYLAND

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
#ifdef HAVE_WAYLAND
            if (not parent and d->m_panel) {
                d->setupInputSurface(window);
            }
#endif // HAVE_WAYLAND
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
