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
    const int MImSubViewIdentifierRole = Qt::UserRole;
    const int MImPluginNameRole = Qt::UserRole + 1;

    //!object name for settings' widgets
    const QString ObjectNameActiveInputMethodWidget("ActiveInputMethodWidget");
};

MIMSettingsDialog::MIMSettingsDialog(MIMPluginManagerPrivate *p, const QString &title,
                                   M::StandardButtons buttons)
    : MDialog(title, buttons),
      activeSubViewItem(0),
      availableSubViewList(0),
      imPluginManagerPrivate(p)
{
    Q_ASSERT(imPluginManagerPrivate);

    MWidget *settingsWidget = new MWidget(this);
    QGraphicsLinearLayout *layout = new QGraphicsLinearLayout(Qt::Vertical, settingsWidget);
    activeSubViewItem = new MContentItem(MContentItem::TwoTextLabels, settingsWidget);
    activeSubViewItem->setObjectName(ObjectNameActiveInputMethodWidget);

    updateActiveSubViewTitle();
    connect(activeSubViewItem, SIGNAL(clicked()), this, SLOT(showAvailableSubViewList()));
    layout->addItem(activeSubViewItem);

    foreach (MInputMethodPlugin *plugin, imPluginManagerPrivate->plugins.keys()) {
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

    setCentralWidget(settingsWidget);
    connect(this, SIGNAL(languageChanged()), this, SLOT(retranslateSettingsUi()));
    connect(this, SIGNAL(disappeared()), this, SLOT(handleDialogDisappeared()));
    retranslateSettingsUi();
}

MIMSettingsDialog::~MIMSettingsDialog()
{
    delete availableSubViewList;
}

void MIMSettingsDialog::refreshUi()
{
    updateActiveSubViewTitle();
    updateActiveSubViewIndex();
}

void MIMSettingsDialog::retranslateUi()
{
    emit languageChanged();
    MDialog::retranslateUi();
}

void MIMSettingsDialog::updateActiveSubViewTitle()
{
    if (!activeSubViewItem)
        return;

    QString subViewId = imPluginManagerPrivate->activeSubView(OnScreen);
    QMap<QString, QString> subViews =
        imPluginManagerPrivate->availableSubViews(imPluginManagerPrivate->activePluginsName(OnScreen), OnScreen);
    QString subViewTitle = subViews.value(subViewId);

    activeSubViewItem->setSubtitle(subViewTitle);
}

void MIMSettingsDialog::showAvailableSubViewList()
{
    if (!availableSubViewList) {
        availableSubViewList = new MPopupList();
        QStandardItemModel *model = new QStandardItemModel(availableSubViewList);
        model->sort(0);
        availableSubViewList->setItemModel(model);
        connect(availableSubViewList, SIGNAL(clicked(const QModelIndex &)),
                this, SLOT(setActiveSubView(const QModelIndex &)));
    }
    // always update available subview model
    updateAvailableSubViewModel();
    MPlainWindow::instance()->sceneManager()->execDialog(availableSubViewList);
}

void MIMSettingsDialog::updateActiveSubViewIndex()
{
    if (availableSubViewList) {
        QString subViewId = imPluginManagerPrivate->activeSubViewIdOnScreen;
        QStandardItemModel *model = static_cast<QStandardItemModel*> (availableSubViewList->itemModel());
        QList<QStandardItem *> items = model->findItems(activeSubViewItem->subtitle());
        foreach (const QStandardItem *item, items) {
            if (subViewId == item->data(MImSubViewIdentifierRole).toString()) {
                availableSubViewList->setCurrentIndex(item->index());
                availableSubViewList->scrollTo(item->index());
                break;
            }
        }
    }
}

void MIMSettingsDialog::updateAvailableSubViewModel()
{
    if (!availableSubViewList)
        return;

    QStandardItemModel *model = static_cast<QStandardItemModel *> (availableSubViewList->itemModel());
    model->clear();

    // get all subviews from plugins which support OnScreen
    foreach (const QString &pluginName, imPluginManagerPrivate->loadedPluginsNames(OnScreen)) {
        QMap<QString, QString> subViews = imPluginManagerPrivate->availableSubViews(pluginName, OnScreen);
        QMap<QString, QString>::const_iterator i = subViews.constBegin();
        while (i != subViews.constEnd()) {
            QStandardItem *item = new QStandardItem(i.value());
            // set the title of subview as the display role,
            // ID as the MImSubViewIdentifierRole role.
            // pluging name as MImPluginNameRole role.
            item->setData(i.value(), Qt::DisplayRole);
            item->setData(i.key(), MImSubViewIdentifierRole);
            item->setData(pluginName, MImPluginNameRole);
            model->appendRow(item);
            ++i;
        }
    }

    updateActiveSubViewIndex();
}

void MIMSettingsDialog::setActiveSubView(const QModelIndex &index)
{
    if (!index.isValid() || !availableSubViewList
        || !availableSubViewList->selectionModel()->isSelected(index))
        return;

    availableSubViewList->setCurrentIndex(index);
    QStandardItemModel *model = static_cast<QStandardItemModel *> (availableSubViewList->itemModel());

    // check whether OnScreen plugin is changed.
    QString pluginName = model->itemFromIndex(index)->data(MImPluginNameRole).toString();
    imPluginManagerPrivate->setActivePlugin(pluginName, OnScreen);

    // check whether active subview is changed.
    QString subViewId = model->itemFromIndex(index)->data(MImSubViewIdentifierRole).toString();
    imPluginManagerPrivate->_q_setActiveSubView(subViewId, OnScreen);
}

void MIMSettingsDialog::retranslateSettingsUi()
{
    //% "Text Input"
    setTitle(qtTrId("qtn_txts_text_input"));

    if (activeSubViewItem) {
        //% "Active input method"
        activeSubViewItem->setTitle(qtTrId("qtn_txts_active_input_method"));
    }

    foreach (MContainer *container, settingsContainerMap.values()) {
        container->setTitle(settingsContainerMap.key(container)->title());
    }
}

void MIMSettingsDialog::handleDialogDisappeared()
{
    // hide availableSubViewList when keyboard is disappeared.
    // availableSubViewList is shown by execDialog/appear
    // means its visibility is controlled by scenemanager,
    // it won't be hidden together with MIMSettingsDialog
    // if we don't call reject/disappear for it.
    if (availableSubViewList) {
        availableSubViewList->reject();
    }
}
