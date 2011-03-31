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



#include "mimplugindescription.h"
#include "minputmethodplugin.h"

//! \internal
class MImPluginDescriptionPrivate
{
public:
    //! Construct new instance.
    //! \param plugin Reference to loaded plugin.
    MImPluginDescriptionPrivate(const MInputMethodPlugin &plugin);

public:
    //! Plugin name.
    QString pluginName;
};
//! \internal_end


MImPluginDescriptionPrivate::MImPluginDescriptionPrivate(const MInputMethodPlugin &plugin)
    : pluginName(plugin.name())
{
}

MImPluginDescription::MImPluginDescription(const MInputMethodPlugin &plugin)
    : d_ptr(new MImPluginDescriptionPrivate(plugin))
{
}

MImPluginDescription::MImPluginDescription(const MImPluginDescription &other)
    : d_ptr(new MImPluginDescriptionPrivate(*other.d_ptr))
{
}

MImPluginDescription::~MImPluginDescription()
{
    delete d_ptr;
}

QString MImPluginDescription::name() const
{
    Q_D(const MImPluginDescription);

    return d->pluginName;
}

