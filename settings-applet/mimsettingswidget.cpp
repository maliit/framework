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

#include <MContainer>
#include <MContentItem>
#include <MLocale>
#include <MPopupList>
#include <QGraphicsLinearLayout>
#include <QStandardItemModel>
#include <QItemSelectionModel>
#include <QDebug>

#include <minputmethodsettingsbase.h>
#include <minputmethodplugin.h>

#include "mimsettingswidget.h"
#include "mimsettingsconf.h"


MImSettingsWidget::MImSettingsWidget()
    : DcpWidget()
{
    initWidget();
}


// pressing the back button means accept for us
bool MImSettingsWidget::back()
{
    return true;
}

MImSettingsWidget::~MImSettingsWidget()
{
    qDebug() << __PRETTY_FUNCTION__;
}

void MImSettingsWidget::initWidget()
{
    QGraphicsLinearLayout* layout = new QGraphicsLinearLayout(Qt::Vertical, this);

    foreach (MInputMethodSettingsBase *settings, MImSettingsConf::instance().settings()) {
        if (settings) {
            QGraphicsWidget *contentWidget = settings->createContentWidget(this);
            if (contentWidget) {
                MContainer *container = new MContainer(settings->title(), this);
                container->setCentralWidget(contentWidget);
                //TODO: icon for the settings.
                layout->addItem(container);
                settingsContainerMap.insert(settings, container);
            }
        }
    }
    setLayout(layout);
    retranslateUi();
}

void MImSettingsWidget::retranslateUi()
{
    foreach (MContainer *container, settingsContainerMap.values()) {
        container->setTitle(settingsContainerMap.key(container)->title());
    }
}
