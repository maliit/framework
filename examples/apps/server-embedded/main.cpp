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

#include <QtGlobal>

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
#include "mimdummyinputcontext.h"
#endif

#include "connectionfactory.h"
#include "mimserver.h"
#include "mimstandaloneserverlogic.h"

#include <QApplication>
#include <QtDebug>
#include <QPalette>
#include <QCommonStyle>

#if QT_VERSION <= QT_VERSION_CHECK(5, 0, 0)
#include <stdlib.h>
#endif

namespace {
    void disableMInputContextPlugin()
    {
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
        // none is a special value for QT_IM_MODULE, which disables loading of any
        // input method module in Qt 5.
        setenv("QT_IM_MODULE", "none", true);
#else
        // prevent loading of minputcontext because we don't need it and starting
        // it might trigger starting of this service by the d-bus. not nice if that is
        // already happening :)
        if (-1 == unsetenv("QT_IM_MODULE")) {
            qWarning("meego-im-uiserver: unable to unset QT_IM_MODULE.");
        }
#endif
    }
}

int main(int argc, char **argv)
{
    // QT_IM_MODULE, MApplication and QtMaemo5Style all try to load
    // MInputContext, which is fine for the application. For the maliit
    // server, we absolutely need to prevent that.
    disableMInputContextPlugin();

    QApplication app(argc, argv);
    QSharedPointer<MImAbstractServerLogic> serverLogic(new MImStandaloneServerLogic);

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    // Set a dummy input context so that Qt does not create a default input
    // context (qimsw-multi) which is expensive and not required by
    // a maliit server.
    app.setInputContext(new MIMDummyInputContext);
#endif

    QSharedPointer<MInputContextConnection> icConnection(Maliit::DBus::createInputContextConnectionWithDynamicAddress());

    // The actual server
    MImServer::configureSettings(MImServer::PersistentSettings);
    MImServer imServer(serverLogic, icConnection);
    Q_UNUSED(imServer);

    return app.exec();
}

