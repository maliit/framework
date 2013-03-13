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

#include "connectionfactory.h"
#include "mimserver.h"
#include "mimserveroptions.h"
#include "mimstandaloneserverlogic.h"

#include <QApplication>
#include <QtDebug>

namespace {
    void disableMInputContextPlugin()
    {
        // none is a special value for QT_IM_MODULE, which disables loading of any
        // input method module in Qt 5.
        setenv("QT_IM_MODULE", "none", true);
    }

    bool isDebugEnabled()
    {
        static int debugEnabled = -1;

        if (debugEnabled == -1) {
            QByteArray debugEnvVar = qgetenv("MALIIT_DEBUG");
            if (!debugEnvVar.isEmpty() && debugEnvVar != "0") {
                debugEnabled = 1;
            } else {
                debugEnabled = 0;
            }
        }

        return debugEnabled == 1;
    }

    void outputMessageToStdErr(QtMsgType type,
                               const char *msg)
    {
        switch (type) {
        case QtDebugMsg:
            if (isDebugEnabled()) {
                fprintf(stderr, "DEBUG: %s\n", msg);
            }
            break;
        case QtWarningMsg:
            fprintf(stderr, "WARNING: %s\n", msg);
            break;
        case QtCriticalMsg:
            fprintf(stderr, "CRITICAL: %s\n", msg);
            break;
        case QtFatalMsg:
            fprintf(stderr, "FATAL: %s\n", msg);
            abort();
        }
    }


#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    void outputMessagesToStdErr(QtMsgType type,
                                const char *msg)
    {
        outputMessageToStdErr(type, msg);
    }
#else
    void outputMessagesToStdErr(QtMsgType type,
                                const QMessageLogContext &context,
                                const QString &msg)
    {
        Q_UNUSED(context);

        outputMessageToStdErr(type, msg.toLatin1().data());
    }
#endif

QSharedPointer<MInputContextConnection> createConnection(const MImServerConnectionOptions &options)
{
#ifdef HAVE_WAYLAND
    if (QGuiApplication::platformName() == "wayland") {
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

}

#define Q_WS_QWS

int main(int argc, char **argv)
{
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    qInstallMsgHandler(outputMessagesToStdErr);
#else
    qInstallMessageHandler(outputMessagesToStdErr);
#endif

    // QT_IM_MODULE, MApplication and QtMaemo5Style all try to load
    // MInputContext, which is fine for the application. For the passthrough
    // server itself, we absolutely need to prevent that.
    disableMInputContextPlugin();

    MImServerCommonOptions serverCommonOptions;
    MImServerConnectionOptions connectionOptions;

    const bool allRecognized = parseCommandLine(argc, argv);
    if (serverCommonOptions.showHelp) {
        printHelpMessage();
        return 1;
    } else if (not allRecognized) {
        printHelpMessage();
    }

    QApplication app(argc, argv);
    QSharedPointer<MImAbstractServerLogic> serverLogic(new MImStandaloneServerLogic);

    // Input Context Connection
    QSharedPointer<MInputContextConnection> icConnection(createConnection(connectionOptions));

    // The actual server
    MImServer::configureSettings(MImServer::PersistentSettings);
    MImServer imServer(serverLogic, icConnection);
    Q_UNUSED(imServer);

    return app.exec();
}
