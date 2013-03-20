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

#include "inputmethodquickplugin.h"
#include "inputmethodquick.h"
#include "maliitquick.h"
#include "keyoverridequick.h"
#include "abstractplatform.h"

#include <QFileInfo>
#include <QQmlComponent>

namespace Maliit
{

class InputMethodQuickPluginPrivate
{
public:
    QSharedPointer<Maliit::AbstractPlatform> m_platform;
    const QString m_filename;
    const QString m_basename;
    QSet<Maliit::HandlerState> m_supported_states;

    InputMethodQuickPluginPrivate(const QString &filename,
                                  const QSharedPointer<Maliit::AbstractPlatform> &platform)
        : m_platform (platform),
          m_filename(filename),
          m_basename(QFileInfo(filename).baseName()),
          m_supported_states()
    {
        m_supported_states << Maliit::OnScreen << Maliit::Hardware;
    }
};

InputMethodQuickPlugin::InputMethodQuickPlugin(const QString &filename,
                                               const QSharedPointer<Maliit::AbstractPlatform> &platform)
    : d_ptr(new InputMethodQuickPluginPrivate(filename, platform))
{
    qmlRegisterUncreatableType<MaliitQuick>("com.meego.maliitquick", 1, 0, "Maliit",
                                            "This is the class used to export Maliit Enums");

    // this do not have to be included to use it, but it have to be
    // registered.
    qmlRegisterUncreatableType<KeyOverrideQuick>
        ( "com.meego.maliitquick.keyoverridequick", 1, 0, "KeyOverrideQuick",
          "This registers KeyOverrideQuick" );
}

InputMethodQuickPlugin::~InputMethodQuickPlugin()
{}

MAbstractInputMethod *InputMethodQuickPlugin::createInputMethod(MAbstractInputMethodHost *host)
{
    Q_D(InputMethodQuickPlugin);

    return new InputMethodQuick(host, d->m_filename, d->m_platform);
}

QSet<Maliit::HandlerState> InputMethodQuickPlugin::supportedStates() const
{
    Q_D(const InputMethodQuickPlugin);

    return d->m_supported_states;
}

QString InputMethodQuickPlugin::name() const
{
    Q_D(const InputMethodQuickPlugin);

    return d->m_basename;
}

} // namespace Maliit
