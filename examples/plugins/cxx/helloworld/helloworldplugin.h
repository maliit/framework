/* * This file is part of Maliit framework *
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 *
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 */

#ifndef HELLO_WORLD_PLUGIN_H
#define HELLO_WORLD_PLUGIN_H

#include <maliit/plugins/inputmethodplugin.h>

#include <QObject>
#include <QWidget>

// TODO: Licence. We need a license that leaves no doubt this code can be used for any purpose

//! Example input method plugin that can be used as a base when developing new input method plugins
class HelloWorldPlugin: public QObject,
    public Maliit::Plugins::InputMethodPlugin
{
    Q_OBJECT
    Q_INTERFACES(Maliit::Plugins::InputMethodPlugin)
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    Q_PLUGIN_METADATA(IID  "org.maliit.examples.cxx.helloworldplugin"
                      FILE "helloworldplugin.json")
#endif

public:
    HelloWorldPlugin();

    //! \reimp
    virtual QString name() const;

    virtual MAbstractInputMethod *createInputMethod(MAbstractInputMethodHost *host);

    virtual QSet<Maliit::HandlerState> supportedStates() const;
    //! \reimp_end

private:
    QSet<Maliit::HandlerState> allowedStates;
};

#endif // HELLO_WORLD_PLUGIN_H
