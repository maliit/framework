/* * This file is part of Maliit framework *
 *
 * Copyright (C) 2013 Openismus GmbH
 *
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 */

#include <wayland-client.h>

#include <QDebug>
#include <QGuiApplication>
#include <QRegion>
#include <QWindow>
#include <LayerShellQt/Window>

#include "waylandplatform.h"
#include "windowdata.h"

using namespace LayerShellQt;

namespace Maliit
{

class WaylandPlatformPrivate
{
public:
    WaylandPlatformPrivate() = default;
    ~WaylandPlatformPrivate() = default;
};

WaylandPlatform::WaylandPlatform()
    : d_ptr(new WaylandPlatformPrivate)
{}

void WaylandPlatform::setupInputPanel(QWindow* window,
                                      Maliit::Position position)
{
    if (qgetenv("QT_WAYLAND_SHELL_INTEGRATION") == QByteArray("layer-shell")) {
        auto ls_window = Window::get(window);
        ls_window->setDesiredOutput(QGuiApplication::primaryScreen());
        ls_window->setLayer(Window::Layer::LayerOverlay);
        Window::Anchors anchors = Window::Anchor::AnchorBottom;
        switch (position) {
            case PositionLeftBottom: {
                anchors |= Window::Anchor::AnchorLeft;
                break;
            }
            case PositionRightBottom: {
                anchors |= Window::Anchor::AnchorRight;
                break;
            }
            default: {
                break;
            }
        }
        ls_window->setAnchors(anchors);
    }
}

void WaylandPlatform::setInputRegion(QWindow* window,
                                     const QRegion& region)
{
    if (not window) {
        return;
    }

    window->setMask(region);
}

WaylandPlatform::~WaylandPlatform()
{

}

} // namespace Maliit
