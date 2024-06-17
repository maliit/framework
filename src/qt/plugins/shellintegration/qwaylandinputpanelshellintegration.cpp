/*
 * Copyright (c) 2017 Jan Arne Petersen
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 */

#include "qwaylandinputpanelshellintegration.h"

#include <QtWaylandClient/private/qwaylandwindow_p.h>

#include "qwaylandinputpanelsurface.h"

QWaylandInputPanelShellIntegration::QWaylandInputPanelShellIntegration()
    : QWaylandShellIntegrationTemplate(1) {}

QWaylandInputPanelShellIntegration::~QWaylandInputPanelShellIntegration()
{
}

bool QWaylandInputPanelShellIntegration::initialize(QWaylandDisplay *display)
{
    auto result = QWaylandShellIntegrationTemplate::initialize(display);
    const auto globals = display->globals();
    for (auto global: globals) {
        if (global.interface == QLatin1String("zwp_input_panel_v1")) {
            m_panel.reset(new QtWayland::zwp_input_panel_v1(display->wl_registry(), global.id, 1));
            break;
        }
    }
    return result;
}

QWaylandShellSurface *
QWaylandInputPanelShellIntegration::createShellSurface(QWaylandWindow *window)
{
    struct zwp_input_panel_surface_v1 *ip_surface = m_panel->get_input_panel_surface(window->wlSurface());

    return new QWaylandInputPanelSurface(ip_surface, window);
}
