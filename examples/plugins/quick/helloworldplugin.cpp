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

#include "helloworldplugin.h"

#include <minputmethodquickplugin.h>

#include <QString>
#include <QDebug>

namespace
{
    const char * const filePath = "qrc:/helloworld.qml";
    const char * const pluginName = "HelloWorldPlugin";
}

HelloWorldPlugin::HelloWorldPlugin()
{
    Q_INIT_RESOURCE(helloworld);
}

QString HelloWorldPlugin::name() const
{
    return pluginName;
}

QString HelloWorldPlugin::qmlFileName() const
{
    return filePath;
}

Q_EXPORT_PLUGIN2(helloworldplugin, HelloWorldPlugin)
