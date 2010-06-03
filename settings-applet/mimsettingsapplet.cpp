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

#include <MAction>
#include <MLocale>

#include "mimsettingsapplet.h"
#include "mimsettingswidget.h"
#include "mimsettingsbrief.h"
#include "mimsettingsconf.h"
#include <DcpWidget>

Q_EXPORT_PLUGIN2(mimsettingsapplet, MImSettingsApplet)

MImSettingsApplet::~MImSettingsApplet()
{
    MImSettingsConf::destroyInstance();
}

void MImSettingsApplet::init()
{
    MLocale locale;
    // add text-input-settings catalog for the settings translation.
    locale.installTrCatalog("text-input-settings");
    MLocale::setDefault(locale);
    MImSettingsConf::createInstance();
}

DcpWidget* MImSettingsApplet::constructWidget(int widgetId)
{
    Q_UNUSED(widgetId);
    MImSettingsWidget *widget = new MImSettingsWidget();
    return widget;
}

QString MImSettingsApplet::title() const
{
    //% "Text Input"
    return QString(qtTrId("qtn_txts_text_input"));
}

QVector<MAction*> MImSettingsApplet::viewMenuItems()
{
    QVector<MAction*> vector;

    return vector;
}

DcpBrief* MImSettingsApplet::constructBrief(int partId)
{
    Q_UNUSED(partId);
    return new MImSettingsBrief();
}

