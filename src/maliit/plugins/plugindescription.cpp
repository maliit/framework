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

#include <maliit/plugins/plugindescription.h>
#include <maliit/plugins/inputmethodplugin.h>

//! \internal
class MImPluginDescriptionPrivate
{
public:
    //! Construct new instance.
    //! \param plugin Reference to loaded plugin.
    MImPluginDescriptionPrivate(const Maliit::Plugins::InputMethodPlugin &plugin);

public:
    //! Plugin name.
    QString pluginName;

    //! Contains true if this plugin is enabled onscreen plugin.
    bool enabled;
};
//! \internal_end


MImPluginDescriptionPrivate::MImPluginDescriptionPrivate(const Maliit::Plugins::InputMethodPlugin &plugin)
    : pluginName(plugin.name()),
    enabled(true)
{
}

MImPluginDescription::MImPluginDescription(const Maliit::Plugins::InputMethodPlugin &plugin)
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

void MImPluginDescription::operator=(const MImPluginDescription &other)
{
    *d_ptr = *other.d_ptr;
}

QString MImPluginDescription::name() const
{
    Q_D(const MImPluginDescription);

    return d->pluginName;
}

bool MImPluginDescription::enabled() const
{
    Q_D(const MImPluginDescription);

    return d->enabled;
}

void MImPluginDescription::setEnabled(bool newEnabled)
{
    Q_D(MImPluginDescription);

    d->enabled = newEnabled;
}

