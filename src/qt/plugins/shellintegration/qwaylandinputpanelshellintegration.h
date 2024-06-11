/*
 * Copyright (c) 2017 Jan Arne Petersen
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 */

#ifndef QWAYLANDINPUTPANELSHELLINTEGRATION_H
#define QWAYLANDINPUTPANELSHELLINTEGRATION_H

#include "qwayland-input-method-unstable-v1.h"
#include <QtWaylandClient/private/qwayland-text-input-unstable-v1.h>
#include <QtWaylandClient/private/qwaylandclientshellapi_p.h>

using namespace QtWaylandClient;

class QWaylandInputPanelShellIntegration
    : public QWaylandShellIntegrationTemplate<
          QWaylandInputPanelShellIntegration>,
      public QtWayland::zwp_text_input_manager_v1 {
public:
    QWaylandInputPanelShellIntegration();
    ~QWaylandInputPanelShellIntegration() override;

    bool initialize(QWaylandDisplay *display) override;
    QWaylandShellSurface *createShellSurface(QWaylandWindow *window) override;

private:
    QScopedPointer<QtWayland::zwp_input_panel_v1> m_panel;
};

#endif //QWAYLANDINPUTPANELSHELLINTEGRATION_H
