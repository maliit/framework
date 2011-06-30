/* * This file is part of meego-im-framework *
 *
 * Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 *
 * Contact: Kimmo Surakka <kimmo.surakka@nokia.com>
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

#include "minputmethodquickplugin.h"
#include "minputmethodquick.h"
#include "maliitquick.h"

#include <QDeclarativeComponent> // needed for qmlRegisterUncreatableType

class MInputMethodQuickPluginPrivate
{
public:
    QStringList languages;  // supported languages
    QSet<MInputMethod::HandlerState> supportedStates;

    MInputMethodQuickPluginPrivate()
    {
        languages << "en" << "fi_FI" << "en_US" << "en_GB" << "ru_RU";
        supportedStates << MInputMethod::OnScreen;
    }
};

MInputMethodQuickPlugin::MInputMethodQuickPlugin()
    : d_ptr(new MInputMethodQuickPluginPrivate)
{
    qmlRegisterUncreatableType<MInputMethodQuick>
        ( "com.meego.keyboard-quick", 1, 0, "MInputMethodQuick",
          "There's only one controller and it is in the C++ side" );
    qmlRegisterUncreatableType<MaliitQuick>
        ( "com.meego.maliitquick", 1, 0, "MaliitQuick",
          "This is the class used to export Maliit Enums" );
}

MInputMethodQuickPlugin::~MInputMethodQuickPlugin()
{
    delete d_ptr;
}

QStringList MInputMethodQuickPlugin::languages() const
{
    Q_D(const MInputMethodQuickPlugin);

    return d->languages;
}

MAbstractInputMethod *MInputMethodQuickPlugin::createInputMethod(MAbstractInputMethodHost *host,
                                                                 QWidget *mainWindow)
{
    return new MInputMethodQuick(host, mainWindow, qmlFileName());
}

MAbstractInputMethodSettings *MInputMethodQuickPlugin::createInputMethodSettings()
{
    // not supported
    return 0;
}

QSet<MInputMethod::HandlerState> MInputMethodQuickPlugin::supportedStates() const
{
    Q_D(const MInputMethodQuickPlugin);

    return d->supportedStates;
}
