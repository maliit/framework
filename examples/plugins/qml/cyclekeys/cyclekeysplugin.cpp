/* * This file is part of Maliit framework *
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 *
 * Contact: maliit-discuss@lists.maliit.org
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 */

#include "cyclekeysplugin.h"

#include <minputmethodquickplugin.h>
#include <QString>

namespace
{
    const char * const filePath = "qrc:/main.qml";
    const char * const pluginName = "CycleKeys";
}

CycleKeys::CycleKeys()
{
    Q_INIT_RESOURCE(cyclekeys);
}

QString CycleKeys::name() const
{
    return pluginName;
}

QString CycleKeys::qmlFileName() const
{
    return filePath;
}

Q_EXPORT_PLUGIN2(qmlcyclekeysplugin, CycleKeys)
