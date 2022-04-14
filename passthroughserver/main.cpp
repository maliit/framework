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

#include <QtGlobal>

#include "connectionfactory.h"
#include "mimserver.h"
#include "mimserveroptions.h"
#ifdef HAVE_XCB
#include "xcbplatform.h"
#endif
#ifdef HAVE_WAYLAND
#include "waylandplatform.h"
#endif // HAVE_WAYLAND
#include "unknownplatform.h"
#include "logging.h"

#include <QGuiApplication>
#include <QtDebug>

namespace {
QLoggingCategory::CategoryFilter defaultLoggingFilter;

void loggingCategoryFilter(QLoggingCategory *category)
{
    if (defaultLoggingFilter) {
        defaultLoggingFilter(category);
    }

    if (qstrcmp(category->categoryName(), "org.maliit.framework") == 0) {
        QByteArray debugEnvVar = qgetenv("MALIIT_DEBUG");

        if (!debugEnvVar.isEmpty() && debugEnvVar != "0") {
            category->setEnabled(QtDebugMsg, true);
        }
    }
}

QSharedPointer<MInputContextConnection> createConnection(const MImServerConnectionOptions &options)
{
#ifdef HAVE_WAYLAND
    auto forceDbus = qgetenv("MALIIT_FORCE_DBUS_CONNECTION");
    if (QGuiApplication::platformName().startsWith("wayland") && (forceDbus.isEmpty() || forceDbus == "0")) {
        return QSharedPointer<MInputContextConnection>(Maliit::createWestonIMProtocolConnection());
    } else
#endif
    if (options.overriddenAddress.isEmpty()) {
        return QSharedPointer<MInputContextConnection>(Maliit::DBus::createInputContextConnectionWithDynamicAddress());
    } else {
        return QSharedPointer<MInputContextConnection>(Maliit::DBus::createInputContextConnectionWithFixedAddress(options.overriddenAddress,
                                                                                                                  options.allowAnonymous));
    }
}

} // unnamed namespace

int main(int argc, char **argv)
{
    // compatibility with MALIIT_DEBUG
    defaultLoggingFilter = QLoggingCategory::installFilter(loggingCategoryFilter);

    // MInputContext is needed for the applications, but for the passthrough
    // server itself, we absolutely need to prevent loading that.
    // none is a special value for QT_IM_MODULE, which disables loading of any
    // input method module in Qt 5.
    setenv("QT_IM_MODULE", "none", true);

    MImServerCommonOptions serverCommonOptions;
    MImServerConnectionOptions connectionOptions;

    const bool allRecognized = parseCommandLine(argc, argv);
    if (serverCommonOptions.showHelp) {
        printHelpMessage();
        return 1;
    } else if (not allRecognized) {
        printHelpMessage();
    }

    QGuiApplication app(argc, argv);

    // Input Context Connection
    QSharedPointer<MInputContextConnection> icConnection(createConnection(connectionOptions));

    QSharedPointer<Maliit::AbstractPlatform> platform(Maliit::createPlatform().release());

    // The actual server
    MImServer::configureSettings(MImServer::PersistentSettings);
    MImServer imServer(icConnection, platform);
    Q_UNUSED(imServer);

    return app.exec();
}
