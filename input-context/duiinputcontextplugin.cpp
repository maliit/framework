/* * This file is part of dui-im-framework *
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * Contact: Nokia Corporation (directui@nokia.com)
 *
 * If you have questions regarding the use of this file, please contact
 * Nokia at directui@nokia.com.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 */

#include "duiinputcontextplugin.h"

#include <duiinputcontext.h>
#include <QString>
#include <QStringList>
#include <QDebug>



DuiInputContextPlugin::DuiInputContextPlugin(QObject *parent)
    : QInputContextPlugin(parent)
{
    // nothing
}


DuiInputContextPlugin::~DuiInputContextPlugin()
{
    // nothing
}


QInputContext *DuiInputContextPlugin::create(const QString &key)
{
    QInputContext *ctx = NULL;

    if (!key.isEmpty()) {
        // currently there is only one type of inputcontext
        ctx = new DuiInputContext(this);
    }
    return ctx;
}


QString DuiInputContextPlugin::description(const QString &s)
{
    Q_UNUSED(s);

    return "DuiInputContext plugin";
}


QString DuiInputContextPlugin::displayName(const QString &s)
{
    Q_UNUSED(s);

    // TODO: want this translated?
    return "input context for dui vkb";
}


QStringList DuiInputContextPlugin::keys() const
{
    return QStringList("DuiInputContext");
}


QStringList DuiInputContextPlugin::languages(const QString &)
{
    return QStringList("EN"); // FIXME
}


Q_EXPORT_PLUGIN2(duiinputcontext, DuiInputContextPlugin)

