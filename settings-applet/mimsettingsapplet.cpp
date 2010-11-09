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

#include "mimsettingsapplet.h"

#include <MAction>
#include <MLocale>

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
    MImSettingsConf::createInstance();
}

DcpWidget* MImSettingsApplet::constructWidget(int widgetId)
{
    DcpWidget *widget = NULL;

    // Create the main settings page
    if (widgetId == 0)
        widget = new MImSettingsWidget();
    // widgetId tells the settings page number for a certain settings plug-in.
    // If we will have multiple settings pages in the future, they must
    // be constructed according to the widgetId. So far we have only one page.
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

