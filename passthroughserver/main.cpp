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


#include "mimdummyinputcontext.h"
#include "minputcontextglibdbusconnection.h"
#include "mimserver.h"
#include "mimserveroptions.h"

#if defined(Q_WS_X11)
#include "mimxapplication.h"
#endif

#include <QApplication>
#include <QtDebug>
#include <QPalette>
#include <QCommonStyle>
#include <stdlib.h>

using namespace std::tr1;

namespace {
    void disableMInputContextPlugin()
    {
        // prevent loading of minputcontext because we don't need it and starting
        // it might trigger starting of this service by the d-bus. not nice if that is
        // already happening :)
        if (-1 == unsetenv("QT_IM_MODULE")) {
            qWarning("meego-im-uiserver: unable to unset QT_IM_MODULE.");
        }

        // TODO: Check if hardwiring the QStyle can be removed at a later stage.
        QApplication::setStyle(new QCommonStyle);
    }

    bool isDebugEnabled()
    {
        static int debugEnabled = -1;

        if (debugEnabled == -1) {
            QByteArray debugEnvVar = qgetenv("MALIIT_DEBUG");
            if (debugEnvVar.toLower() == "enabled") {
                debugEnabled = 1;
            } else {
                debugEnabled = 0;
            }
        }

        return debugEnabled == 1;
    }

    void ouputMessagesToStdErr(QtMsgType type, const char *msg)
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
}

int main(int argc, char **argv)
{
    qInstallMsgHandler(ouputMessagesToStdErr);

    // QT_IM_MODULE, MApplication and QtMaemo5Style all try to load
    // MInputContext, which is fine for the application. For the passthrough
    // server itself, we absolutely need to prevent that.
    disableMInputContextPlugin();

#if defined(Q_WS_X11)
    MImServerXOptions serverXOptions;
#endif
    MImServerCommonOptions serverCommonOptions;

    const bool allRecognized = parseCommandLine(argc, argv);
    if (serverCommonOptions.showHelp)
    {
        printHelpMessage();
        return 1;
    } else if (not allRecognized)
    {
        printHelpMessage();
    }

#if defined(Q_WS_X11)
    MImXApplication app(argc, argv, serverXOptions);
#elif defined(Q_WS_QPA) || defined(Q_WS_QWS)
    QApplication app(argc, argv);
#endif

    // Set a dummy input context so that Qt does not create a default input
    // context (qimsw-multi) which is expensive and not required by
    // meego-im-uiserver.
    app.setInputContext(new MIMDummyInputContext);

    // DBus Input Context Connection
    shared_ptr<MInputContextConnection> icConnection(new MInputContextGlibDBusConnection);

    MImServer imServer(icConnection);
    Q_UNUSED(imServer);

    return app.exec();
}

