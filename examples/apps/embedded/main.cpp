/* * This file is part of meego-im-framework *
 *
 * Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#include "embedded.h"

#include <QApplication>
#if (QT_VERSION < 0x050000)
#include <QInputContextFactory>
#endif

#include <cstdlib>
#include <QDebug>

// Required for including treat-as-internal-you've-been-warned library:
#ifndef MALIIT_FRAMEWORK_USE_INTERNAL_API
#define MALIIT_FRAMEWORK_USE_INTERNAL_API
#endif
#include <mimsettings.h>

namespace {
const char * const PluginPathsKey = "maliit/paths";
const char * const EnabledPluginsKey = "/maliit/onscreen/enabled";
const char * const ActivePluginKey = "/maliit/onscreen/active";
}

int main(int argc, char** argv)
{
    QApplication kit(argc, argv);

    // Find plugin settings from command line:
    const QStringList &arguments(qApp->arguments());
    QString value(arguments.last());
    QString path;
    QString plugin;
    QString subview;

    for (int index = arguments.count() - 2; index > -1; --index) {
        const QString &arg(arguments.at(index));
        if (arg == "-path" || arg == "--path") {
            path = value;
        } else if (arg == "-plugin" || arg == "--plugin") {
            plugin = value;
        } else if (arg == "-subview" || arg == "--subview") {
            subview = value;
        }

        value = arg;
    }

    // Store plugin settings:
    if (not path.isEmpty()) {
        QStringList list;
        list.append(path);

        MImSettings(PluginPathsKey).set(list);
    }

    if (not plugin.isEmpty()) {
        QStringList list;
        list.append(plugin);

        if (not subview.isEmpty()) {
            list.append(subview);
        } else {
            qWarning() << "No subview specified, plugin may not load!";
        }

        MImSettings(EnabledPluginsKey).set(list);
        MImSettings(ActivePluginKey).set(list);
    }

#if (QT_VERSION < 0x050000)
    kit.setInputContext(QInputContextFactory::create("MaliitDirect", &kit));
#endif

    MainWindow window;
    return kit.exec();
}
