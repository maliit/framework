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

#include "mdirectinputcontextplugin.h"

#include "mimserver.h"
#include "mimapphostedserverlogic.h"

#include "connectionfactory.h"
#include "mimdirectserverconnection.h"
#include "miminputcontextdirectconnection.h"

#include <maliit/inputmethod.h>

using namespace std::tr1;

#include <minputcontext.h>
#include <QString>
#include <QStringList>

namespace {
    const QString MaliitDirectInputContextName(MALIIT_INPUTCONTEXT_NAME"Direct");
}

MDirectInputContextPlugin::MDirectInputContextPlugin(QObject *parent)
    : QInputContextPlugin(parent)
{
    // nothing
}


MDirectInputContextPlugin::~MDirectInputContextPlugin()
{
    // nothing
}


QInputContext *MDirectInputContextPlugin::create(const QString &key)
{
    QInputContext *ctx = NULL;

    if (key == MaliitDirectInputContextName) {

        QSharedPointer<MImDirectServerConnection> serverConnection =
                qSharedPointerObjectCast<MImDirectServerConnection>(Maliit::createServerConnection(MaliitDirectInputContextName));
        MImInputContextDirectConnection *icConnection = new MImInputContextDirectConnection;
        serverConnection->connectTo(icConnection);

        shared_ptr<MInputContextConnection> icConn(icConnection);
        QSharedPointer<MImAbstractServerLogic> serverLogic(new MImAppHostedServerLogic);
        MImServer *imServer = new MImServer(serverLogic, icConn);

        Maliit::InputMethod::instance()->setWidget(imServer->pluginsWidget());

        ctx = new MInputContext(serverConnection, MaliitDirectInputContextName, this);
        imServer->setParent(ctx);
    } else {
        qCritical() << "Unknown plugin name";
    }

    return ctx;
}


QString MDirectInputContextPlugin::description(const QString &s)
{
    Q_UNUSED(s);

    return "Maliit input context plugin";
}


QString MDirectInputContextPlugin::displayName(const QString &s)
{
    Q_UNUSED(s);

    // TODO: want this translated?
    return "Input context for Maliit input methods";
}


QStringList MDirectInputContextPlugin::keys() const
{
    return QStringList(MaliitDirectInputContextName);
}


QStringList MDirectInputContextPlugin::languages(const QString &)
{
    return QStringList("EN"); // FIXME
}


Q_EXPORT_PLUGIN2(mdirectinputcontext, MDirectInputContextPlugin)

