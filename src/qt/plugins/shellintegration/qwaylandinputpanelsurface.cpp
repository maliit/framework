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

void QWaylandInputPanelSurface::setType(Qt::WindowType type, QWaylandWindow *transientParent)
{
    Q_UNUSED(type)
    Q_UNUSED(transientParent)

    set_toplevel(window()->screen()->output(), position_center_bottom);
}

}

QT_END_NAMESPACE
