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

#include "mimpluginmanager.h"
#include "mimpluginmanageradaptor.h"


MIMPluginManagerAdaptor::MIMPluginManagerAdaptor(MIMPluginManager *parent)
    : QDBusAbstractAdaptor(parent),
      owner(parent)
{
    if (!parent) {
        qFatal("Creating MIMPluginManagerAdaptor without a parent");
    }
}


MIMPluginManagerAdaptor::~MIMPluginManagerAdaptor()
{
    // nothing
}

QStringList MIMPluginManagerAdaptor::queryAvailablePlugins()
{
    return owner->loadedPluginsNames();
}

QStringList MIMPluginManagerAdaptor::queryAvailablePlugins(int state)
{
    return owner->loadedPluginsNames(static_cast<MInputMethod::HandlerState>(state));
}

QString MIMPluginManagerAdaptor::queryActivePlugin(int state)
{
    return owner->activePluginsName(static_cast<MInputMethod::HandlerState>(state));
}

QMap<QString, QVariant> MIMPluginManagerAdaptor::queryAvailableSubViews(const QString &plugin,
                                                                        int state)
{
    QMap<QString, QVariant> vSubViews;

    QMap<QString, QString> subViews
        = owner->availableSubViews(plugin, static_cast<MInputMethod::HandlerState>(state));
    QMapIterator<QString, QString> subView(subViews);

    while (subView.hasNext()) {
        subView.next();
        vSubViews.insert(subView.key(), subView.value());
    }

    return vSubViews;
}

QMap<QString, QVariant> MIMPluginManagerAdaptor::queryActiveSubView(int state)
{
    QMap<QString, QVariant> activeSubbView;
    activeSubbView.insert(owner->activeSubView(static_cast<MInputMethod::HandlerState>(state)),
                          owner->activePluginsName(static_cast<MInputMethod::HandlerState>(state)));
    return activeSubbView;
}

void MIMPluginManagerAdaptor::setActivePlugin(const QString &pluginName, int state,
                                              const QString &subViewId)
{
    owner->setActivePlugin(pluginName, static_cast<MInputMethod::HandlerState>(state));

    if (!subViewId.isEmpty()) {
        owner->setActiveSubView(subViewId, static_cast<MInputMethod::HandlerState>(state));
    }
}

void MIMPluginManagerAdaptor::setActiveSubView(const QString &subViewId, int state)
{
    owner->setActiveSubView(subViewId, static_cast<MInputMethod::HandlerState>(state));
}

