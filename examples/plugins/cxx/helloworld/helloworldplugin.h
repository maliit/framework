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

#ifndef HELLO_WORLD_PLUGIN_H
#define HELLO_WORLD_PLUGIN_H

#include "minputmethodplugin.h"

#include <QObject>
#include <QWidget>

// TODO: Licence. We need a license that leaves no doubt this code can be used for any purpose

//! Example input method plugin that can be used as a base when developing new input method plugins
class HelloWorldPlugin: public QObject,
    public Maliit::Plugins::InputMethodPlugin
{
    Q_OBJECT
    Q_INTERFACES(Maliit::Plugins::InputMethodPlugin)

public:
    HelloWorldPlugin();

    //! \reimp
    virtual QString name() const;

    virtual MAbstractInputMethod *createInputMethod(MAbstractInputMethodHost *host,
                                                    QWidget *mainWindow);

    virtual QSet<MInputMethod::HandlerState> supportedStates() const;
    //! \reimp_end

private:
    QSet<MInputMethod::HandlerState> allowedStates;
};

#endif // HELLO_WORLD_PLUGIN_H
