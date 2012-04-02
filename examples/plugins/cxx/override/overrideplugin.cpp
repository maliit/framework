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

#include "overrideplugin.h"
#include "overrideinputmethod.h"

#include <QtPlugin>
#include <QtDebug>

OverridePlugin::OverridePlugin()
{
    /* This override plugin is an on screen input method */
    allowedStates << Maliit::OnScreen;
}

QString OverridePlugin::name() const
{
    return "OverridePlugin";
}

QStringList OverridePlugin::languages() const
{
    return QStringList("EN");
}

MAbstractInputMethod *
OverridePlugin::createInputMethod(MAbstractInputMethodHost *host,
                                  QWidget *mainWindow)
{
    return new OverrideInputMethod(host, mainWindow);
}

MAbstractInputMethodSettings *OverridePlugin::createInputMethodSettings()
{
    /* This plugin does not have a settings widget */
    return 0;
}
QSet<Maliit::HandlerState> OverridePlugin::supportedStates() const
{
    return allowedStates;
}

Q_EXPORT_PLUGIN2(cxxoverrideplugin, OverridePlugin)

