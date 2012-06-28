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

#ifndef OVERRIDE_PLUGIN_H
#define OVERRIDE_PLUGIN_H

#include <maliit/plugins/inputmethodplugin.h>

#include <QObject>
#include <QWidget>

// TODO: Licence. We need a license that leaves no doubt this code can be used for any purpose

//! Override input method plugin that can be used as a base when developing new input method plugins
class OverridePlugin: public QObject,
    public Maliit::Plugins::InputMethodPlugin
{
    Q_OBJECT
    Q_INTERFACES(Maliit::Plugins::InputMethodPlugin)

public:
    OverridePlugin();

    //! \reimp
    virtual QString name() const;

    virtual MAbstractInputMethod *createInputMethod(MAbstractInputMethodHost *host);

    virtual QSet<Maliit::HandlerState> supportedStates() const;
    //! \reimp_end

private:
    QSet<Maliit::HandlerState> allowedStates;
};

#endif // OVERRIDE_PLUGIN_H
