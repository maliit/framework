/* * This file is part of Maliit framework *
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 *
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 */

#include "overrideplugin.h"

#include <minputmethodquickplugin.h>

#include <QString>
#include <QDebug>

namespace
{
    const char * const filePath = "qrc:/override.qml";
    const char * const pluginName = "OverridePlugin";
}

OverridePlugin::OverridePlugin()
{
    Q_INIT_RESOURCE(override);
}

QString OverridePlugin::name() const
{
    return pluginName;
}

QString OverridePlugin::qmlFileName() const
{
    return filePath;
}

Q_EXPORT_PLUGIN2(qmloverrideplugin, OverridePlugin)
