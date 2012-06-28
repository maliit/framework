/* * This file is part of maliit-framework *
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

#ifndef OVERRIDE_PLUGIN_H
#define OVERRIDE_PLUGIN_H

#include <minputmethodquickplugin.h>
#include <QObject>

class OverridePlugin
     : public QObject, public MInputMethodQuickPlugin
{
    Q_OBJECT
    Q_INTERFACES(MInputMethodQuickPlugin Maliit::Plugins::InputMethodPlugin)
public:
    OverridePlugin();
    //! \reimpl
    virtual QString name() const;
    virtual QString qmlFileName() const;
    //! \reimpl_end
};

#endif // OVERRIDE_PLUGIN_H
