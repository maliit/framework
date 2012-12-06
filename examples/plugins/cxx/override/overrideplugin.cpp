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

MAbstractInputMethod *
OverridePlugin::createInputMethod(MAbstractInputMethodHost *host)
{
    return new OverrideInputMethod(host);
}

QSet<Maliit::HandlerState> OverridePlugin::supportedStates() const
{
    return allowedStates;
}

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
Q_EXPORT_PLUGIN2(cxxoverrideplugin, OverridePlugin)
#endif
