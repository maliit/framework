/* * This file is part of meego-im-framework *
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

#include "minputcontextplugin.h"

#include "glibdbusimserverproxy.h"

#include <minputcontext.h>
#include <QString>
#include <QStringList>

MInputContextPlugin::MInputContextPlugin(QObject *parent)
    : QInputContextPlugin(parent)
{
    // nothing
}


MInputContextPlugin::~MInputContextPlugin()
{
    // nothing
}


QInputContext *MInputContextPlugin::create(const QString &key)
{
    QInputContext *ctx = NULL;

    if (!key.isEmpty()) {
        // currently there is only one type of inputcontext
        MImServerConnection *imServer = new GlibDBusIMServerProxy(0);
        ctx = new MInputContext(imServer, this);
        imServer->setParent(ctx); // Tie lifetime of server connection to the inputcontext
    }
    return ctx;
}


QString MInputContextPlugin::description(const QString &s)
{
    Q_UNUSED(s);

    return "Maliit input context plugin";
}


QString MInputContextPlugin::displayName(const QString &s)
{
    Q_UNUSED(s);

    // TODO: want this translated?
    return "Input context for Maliit input methods";
}


QStringList MInputContextPlugin::keys() const
{
    return QStringList(MALIIT_INPUTCONTEXT_NAME);
}


QStringList MInputContextPlugin::languages(const QString &)
{
    return QStringList("EN"); // FIXME
}


Q_EXPORT_PLUGIN2(minputcontext, MInputContextPlugin)

