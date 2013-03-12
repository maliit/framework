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
#include "tooltip_p.h"

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QQmlComponent>
#else
#include <QDeclarativeComponent> // needed for qmlRegisterUncreatableType
#endif

namespace {
QStringList gQmlImportPaths;
}

class MInputMethodQuickPluginPrivate
{
public:
    QSet<Maliit::HandlerState> supportedStates;

    MInputMethodQuickPluginPrivate()
    {
        supportedStates << Maliit::OnScreen << Maliit::Hardware;
    }
};

MInputMethodQuickPlugin::MInputMethodQuickPlugin()
    : d_ptr(new MInputMethodQuickPluginPrivate)
{
    qmlRegisterUncreatableType<MaliitQuick>("com.meego.maliitquick", 1, 0, "Maliit",
                                            "This is the class used to export Maliit Enums");

    qmlRegisterType<Tooltip>("com.meego.maliitquick", 1, 0, "Tooltip");

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
    return new MInputMethodQuick(host, qmlFileName());
}

QSet<Maliit::HandlerState> MInputMethodQuickPlugin::supportedStates() const
{
    Q_D(const MInputMethodQuickPlugin);

    return d->supportedStates;
}
