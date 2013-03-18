/* * This file is part of Maliit framework *
 *
 * Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#include "minputmethodquickplugin.h"
#include "minputmethodquick.h"
#include "maliitquick.h"
#include "mkeyoverridequick.h"
#include "abstractplatform.h"

#include <QQmlComponent>

namespace {
QStringList gQmlImportPaths;
}

class MInputMethodQuickPluginPrivate
{
public:
    QSet<Maliit::HandlerState> supportedStates;
    QSharedPointer<Maliit::AbstractPlatform> m_platform;

    MInputMethodQuickPluginPrivate(const QSharedPointer<Maliit::AbstractPlatform> &platform)
        : m_platform (platform)
    {
        supportedStates << Maliit::OnScreen << Maliit::Hardware;
    }
};

MInputMethodQuickPlugin::MInputMethodQuickPlugin(const QSharedPointer<Maliit::AbstractPlatform> &platform)
    : d_ptr(new MInputMethodQuickPluginPrivate(platform))
{
    qmlRegisterUncreatableType<MaliitQuick>("com.meego.maliitquick", 1, 0, "Maliit",
                                            "This is the class used to export Maliit Enums");

    // this do not have to be included to use it, but it have to be
    // registered.
    qmlRegisterUncreatableType<MKeyOverrideQuick>
        ( "com.meego.maliitquick.keyoverridequick", 1, 0, "KeyOverrideQuick",
          "This registers MKeyOverrideQuick" );
}

MInputMethodQuickPlugin::~MInputMethodQuickPlugin()
{
    delete d_ptr;
}

void MInputMethodQuickPlugin::setQmlImportPaths(const QStringList &paths)
{
    gQmlImportPaths = paths;
}

QStringList MInputMethodQuickPlugin::qmlImportPaths()
{
    return gQmlImportPaths;
}

MAbstractInputMethod *MInputMethodQuickPlugin::createInputMethod(MAbstractInputMethodHost *host)
{
    Q_D(MInputMethodQuickPlugin);

    return new MInputMethodQuick(host, qmlFileName(), d->m_platform);
}

QSet<Maliit::HandlerState> MInputMethodQuickPlugin::supportedStates() const
{
    Q_D(const MInputMethodQuickPlugin);

    return d->supportedStates;
}
