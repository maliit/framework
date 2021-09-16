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

#ifndef CYCLE_KEYS_PLUGIN_H
#define CYCLE_KEYS_PLUGIN_H

#include <minputmethodquickplugin.h>
#include <QObject>

class CycleKeys
     : public QObject, public MInputMethodQuickPlugin
{
    Q_OBJECT
    Q_INTERFACES(MInputMethodQuickPlugin Maliit::Plugins::InputMethodPlugin)

public:
    CycleKeys();
    //! \reimpl
    virtual QString name() const;
    virtual QString qmlFileName() const;
    //! \reimpl_end
};

#endif // CYCLE_KEYS_PLUGIN_H
