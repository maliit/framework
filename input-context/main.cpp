/* * This file is part of Maliit framework *
 *
 * Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
 *
 * All rights reserved.
 *
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 */

#include <qpa/qplatforminputcontextplugin_p.h>
#include <QtCore/QStringList>
#include <QDebug>

#include "minputcontext.h"

class MaliitPlatformInputContextPlugin: public QPlatformInputContextPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QPlatformInputContextFactoryInterface_iid FILE "maliit.json")

public:
    QPlatformInputContext *create(const QString&, const QStringList&);
};

QPlatformInputContext *MaliitPlatformInputContextPlugin::create(const QString &system, const QStringList &paramList)
{
    Q_UNUSED(paramList);

    if (system.compare(system, QStringLiteral("maliit"), Qt::CaseInsensitive) == 0) {
        return new MInputContext;
    }
    return 0;
}

#include "main.moc"
