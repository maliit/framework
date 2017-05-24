/* 
 * Copyright (c) 2017 Jan Arne Petersen
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 */

#include <QtWaylandClient/private/qwaylandshellintegrationplugin_p.h>

#include "qwaylandinputpanelshellintegration.h"

QT_BEGIN_NAMESPACE

namespace QtWaylandClient
{

class QWaylandInputPanelShellIntegrationPlugin: public QWaylandShellIntegrationPlugin
{
Q_OBJECT
    Q_PLUGIN_METADATA(IID QWaylandShellIntegrationFactoryInterface_iid FILE "inputpanelshell.json")

public:
    virtual QWaylandShellIntegration *create(const QString &key, const QStringList &paramList) override;
};

QWaylandShellIntegration *QWaylandInputPanelShellIntegrationPlugin::create(const QString &key, const QStringList &paramList)
{
    Q_UNUSED(key);
    Q_UNUSED(paramList);
    return new QWaylandInputPanelShellIntegration();
}

}

QT_END_NAMESPACE

#include "inputpanelshellplugin.moc"