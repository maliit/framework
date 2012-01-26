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
#include "inputcontextdbusaddress.h"

#include "mimserver.h"
#include "mimapphostedserverlogic.h"

#include "mimdirectserverconnection.h"
#include "miminputcontextdirectconnection.h"

#include "inputmethod.h"

using namespace std::tr1;

#include <minputcontext.h>
#include <QString>
#include <QStringList>

namespace {
    const char * const ServerAddressEnv("MALIIT_SERVER_ADDRESS");
    const QString MaliitInputContextName(MALIIT_INPUTCONTEXT_NAME);
    const QString MaliitDirectInputContextName(MALIIT_INPUTCONTEXT_NAME"Direct");
}

MInputContextPlugin::MInputContextPlugin(QObject *parent)
    : QInputContextPlugin(parent)
{
    // nothing
}


MInputContextPlugin::~MInputContextPlugin()
{
    // nothing
}


// TODO: split the two different cases into different classes
// and build separately, so that the non-direct IC does not have to link against server code
QInputContext *MInputContextPlugin::create(const QString &key)
{
    QInputContext *ctx = NULL;

    if (key == MaliitInputContextName) {
        std::tr1::shared_ptr<Maliit::InputContext::DBus::Address> address;

        const QByteArray overriddenAddress = qgetenv(ServerAddressEnv);

        if (overriddenAddress.isEmpty()) {
            address.reset(new Maliit::InputContext::DBus::DynamicAddress);
        } else {
            address.reset(new Maliit::InputContext::DBus::FixedAddress(overriddenAddress));
        }

        MImServerConnection *serverConnection = new GlibDBusIMServerProxy(address, 0);

        ctx = new MInputContext(serverConnection, MaliitInputContextName, this);
        serverConnection->setParent(ctx); // Tie lifetime of server connection to the inputcontext

    } else if (key == MaliitDirectInputContextName) {

        MImDirectServerConnection *serverConnection = new MImDirectServerConnection(0);
        MImInputContextDirectConnection *icConnection = new MImInputContextDirectConnection(0);
        serverConnection->connectTo(icConnection);

        shared_ptr<MInputContextConnection> icConn(icConnection);
        QSharedPointer<MImAbstractServerLogic> serverLogic(new MImAppHostedServerLogic);
        MImServer *imServer = new MImServer(serverLogic, icConn);

        Maliit::InputMethod::instance()->setWidget(imServer->pluginsWidget());

        ctx = new MInputContext(serverConnection, MaliitDirectInputContextName, this);
        serverConnection->setParent(ctx);
        imServer->setParent(ctx);
    } else {
        qCritical() << "Unknown plugin name";
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
    QStringList list = QStringList(MaliitInputContextName);
    list << QString(MaliitDirectInputContextName);
    return list;
}


QStringList MInputContextPlugin::languages(const QString &)
{
    return QStringList("EN"); // FIXME
}


Q_EXPORT_PLUGIN2(minputcontext, MInputContextPlugin)

