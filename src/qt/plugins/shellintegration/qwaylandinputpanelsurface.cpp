/* 
 * Copyright (c) 2017 Jan Arne Petersen
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 */

#include "qwaylandinputpanelsurface.h"

#include <QtWaylandClient/private/qwaylandwindow_p.h>
#include <QtWaylandClient/private/qwaylandscreen_p.h>

QT_BEGIN_NAMESPACE

Q_LOGGING_CATEGORY(qLcQpaShellIntegration, "qt.qpa.wayland.shell")

namespace QtWaylandClient
{

QWaylandInputPanelSurface::QWaylandInputPanelSurface(struct ::zwp_input_panel_surface_v1 *object,
                                                     QWaylandWindow *window)
    : QWaylandShellSurface(window)
    , QtWayland::zwp_input_panel_surface_v1(object)
{
    qCDebug(qLcQpaShellIntegration) << Q_FUNC_INFO;
}

QWaylandInputPanelSurface::~QWaylandInputPanelSurface()
{
    qCDebug(qLcQpaShellIntegration) << Q_FUNC_INFO;
}

void QWaylandInputPanelSurface::applyConfigure()
{
    set_toplevel(window()->waylandScreen()->output(), position_center_bottom);
    window()->resizeFromApplyConfigure(m_pendingSize);
}

bool QWaylandInputPanelSurface::isExposed() const
{
    return m_configured;
}

void QWaylandInputPanelSurface::zwp_input_panel_surface_v1_configure(uint32_t serial, uint32_t width, uint32_t height)
{
    ack_configure(serial);

    m_pendingSize = QSize(width, height);

    if (!m_configured) {
        m_configured = true;
        window()->resizeFromApplyConfigure(m_pendingSize);
        window()->handleExpose(QRect(QPoint(), m_pendingSize));
    } else {
        // Later configures are resizes, so we have to queue them up for a time when we
        // are not painting to the window.
        window()->applyConfigureWhenPossible();
    }
}

}

QT_END_NAMESPACE
