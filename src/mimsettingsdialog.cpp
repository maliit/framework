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

#include "mimsettingsdialog.h"

#include <MGConfItem>
#include <MKeyboardStateTracker>
#include <MWidget>
#include <MDialog>
#include <MContainer>
#include <MContentItem>
#include <MSceneManager>
#include <MLocale>
#include <MPopupList>

#include "minputmethodplugin.h"
#include "minputmethodsettingsbase.h"
#include "mimpluginmanager_p.h"
#include "mplainwindow.h"

#include <QGraphicsLinearLayout>
#include <QStandardItemModel>
#include <QDebug>


namespace
{
    const QString PluginRoot = "/meegotouch/inputmethods/plugins/";
    const QString MImHandlerToPlugin = PluginRoot + "handler";
};

MIMSettingsDialog::MIMSettingsDialog(MIMPluginManagerPrivate *p, const QString &title,
                                   M::StandardButtons buttons)
    : MDialog(title, buttons),
      imPluginManagerPrivate(p)
{
    Q_ASSERT(imPluginManagerPrivate);

    MWidget *settingsWidget = new MWidget(this);
    QGraphicsLinearLayout *layout = new QGraphicsLinearLayout(Qt::Vertical, settingsWidget);

    foreach (MInputMethodPlugin *plugin, imPluginManagerPrivate->plugins.keys()) {
        if (imPluginManagerPrivate->blacklist.contains(imPluginManagerPrivate->plugins[plugin].fileName))
            continue;
        MInputMethodSettingsBase *settings = plugin->createInputMethodSettings();
        if (settings) {
            QGraphicsWidget *contentWidget = settings->createContentWidget(settingsWidget);
            if (contentWidget) {
                MContainer *container = new MContainer(settings->title(), settingsWidget);
                container->setCentralWidget(contentWidget);
                //TODO: icon for the setting.
                layout->addItem(container);
                settingsContainerMap.insert(settings, container);
            }
        }
    }
    settingsWidget->setLayout(layout);
    //hack way to avoid dialog does not set size restriction for its central widget
    settingsWidget->setPreferredWidth(preferredWidth());

    setCentralWidget(settingsWidget);
    connect(this, SIGNAL(languageChanged()), this, SLOT(retranslateSettingsUi()));
    retranslateSettingsUi();
}

MIMSettingsDialog::~MIMSettingsDialog()
{
}

void MIMSettingsDialog::retranslateUi()
{
    emit languageChanged();
    MDialog::retranslateUi();
}

void MIMSettingsDialog::orientationChangeEvent(MOrientationChangeEvent *event)
{
    //hack way to avoid dialog does not set size restriction for its central widget
    MDialog::orientationChangeEvent(event);
    centralWidget()->setPreferredWidth(preferredWidth());
}

void MIMSettingsDialog::retranslateSettingsUi()
{
    //% "Text Input"
    setTitle(qtTrId("qtn_txts_text_input"));

    foreach (MContainer *container, settingsContainerMap.values()) {
        container->setTitle(settingsContainerMap.key(container)->title());
    }
}

