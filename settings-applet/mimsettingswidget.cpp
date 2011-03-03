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

#include "mimsettingswidget.h"

#include <MContainer>
#include <MBasicListItem>
#include <MLocale>
#include <MPopupList>
#include <MLabel>
#include <QGraphicsLinearLayout>
#include <QStandardItemModel>
#include <QItemSelectionModel>
#include <QDebug>

#include <mabstractinputmethodsettings.h>
#include <minputmethodplugin.h>

#include "mimsettingsconf.h"

namespace {
    const int MImSubViewIdentifierRole = Qt::UserRole;
    const int MImPluginNameRole = Qt::UserRole + 1;

    //!object name for settings' widgets
    const QString ObjectNameActiveInputMethodWidget("ActiveInputMethodWidget");
};

MImSettingsWidget::MImSettingsWidget()
    : DcpWidget(),
      headerLabel(0),
      activeSubViewItem(0),
      availableSubViewList(0)
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
    delete availableSubViewList;
}

void MImSettingsWidget::initWidget()
{
    QGraphicsLinearLayout* mainLayout = new QGraphicsLinearLayout(Qt::Vertical, this);

    // Header
    headerLabel = new MLabel(this);
    headerLabel->setStyleName("CommonXLargeHeaderInverted");
    mainLayout->addItem(headerLabel);
    mainLayout->setStretchFactor(headerLabel, 0);

    // Active input method selector
    // We are using MBasicListItem instead of MContentItem because
    // the latter is not supported by theme
    activeSubViewItem = new MBasicListItem(MBasicListItem::TitleWithSubtitle, this);
    activeSubViewItem->setObjectName(ObjectNameActiveInputMethodWidget);
    activeSubViewItem->setStyleName("CommonBasicListItemInverted");
    mainLayout->addItem(activeSubViewItem);
    mainLayout->setStretchFactor(activeSubViewItem, 0);
    connect(activeSubViewItem, SIGNAL(clicked()), this, SLOT(showAvailableSubViewList()));

    // Plugin settings
    foreach (MAbstractInputMethodSettings *settings, MImSettingsConf::instance().settings()) {
        if (settings) {
            QGraphicsWidget *contentWidget = settings->createContentWidget(this);
            if (contentWidget) {
                QGraphicsLinearLayout *layout = new QGraphicsLinearLayout(Qt::Vertical);
                MContainer *container = new MContainer(this);
                container->setStyleName("CommonLargePanel");
                container->setHeaderVisible(false);
                container->centralWidget()->setLayout(layout);

                MLabel *header = new MLabel(settings->title(), this);
                header->setStyleName("CommonGroupHeaderInverted");
                //TODO: icon for the settings.
                layout->addItem(header);
                layout->addItem(contentWidget);

                mainLayout->addItem(container);
                mainLayout->setStretchFactor(container, 0);
                settingsLabelMap.insert(settings, header);
            }
        }
    }
    setLayout(mainLayout);
    mainLayout->addStretch();
    retranslateUi();
    connect(&MImSettingsConf::instance(), SIGNAL(activeSubViewChanged()), this, SLOT(syncActiveSubView()));
}

void MImSettingsWidget::retranslateUi()
{
    if (!activeSubViewItem || !headerLabel)
        return;

    //% "Text input"
    headerLabel->setText(qtTrId("qtn_txts_text_input"));
    //% "Active input method"
    activeSubViewItem->setTitle(qtTrId("qtn_txts_active_input_method"));
    updateActiveSubViewTitle();

    foreach (MLabel *label, settingsLabelMap.values()) {
        label->setText(settingsLabelMap.key(label)->title());
    }
    if (availableSubViewList) {
        //% "Active input method"
        availableSubViewList->setTitle(qtTrId("qtn_txts_active_input_method"));
    }
}

void MImSettingsWidget::syncActiveSubView()
{
    // update setting
    updateActiveSubViewTitle();
    updateActiveSubViewIndex();
}

void MImSettingsWidget::updateActiveSubViewTitle()
{
    if (!activeSubViewItem)
        return;

    activeSubViewItem->setSubtitle(MImSettingsConf::instance().activeSubView().subViewTitle);
}

void MImSettingsWidget::showAvailableSubViewList()
{
    if (!availableSubViewList) {
        availableSubViewList = new MPopupList();
        //% "Active input method"
        availableSubViewList->setTitle(qtTrId("qtn_txts_active_input_method"));
        QStandardItemModel *model = new QStandardItemModel(availableSubViewList);
        model->sort(0);
        availableSubViewList->setItemModel(model);
        connect(availableSubViewList, SIGNAL(clicked(const QModelIndex &)),
                this, SLOT(setActiveSubView(const QModelIndex &)));
    }
    // always update available subview model avoid missing some updated subviews
    updateAvailableSubViewModel();
    availableSubViewList->exec();
}

void MImSettingsWidget::updateActiveSubViewIndex()
{
    if (availableSubViewList) {
        QString subViewId = MImSettingsConf::instance().activeSubView().subViewId;
        QStandardItemModel *model = static_cast<QStandardItemModel*> (availableSubViewList->itemModel());
        QList<QStandardItem *> items = model->findItems(activeSubViewItem->subtitle());
        foreach (const QStandardItem *item, items) {
            if (subViewId == item->data(Qt::UserRole).toString()) {
                availableSubViewList->setCurrentIndex(item->index());
                availableSubViewList->scrollTo(item->index());
                break;
            }
        }
    }
}

void MImSettingsWidget::updateAvailableSubViewModel()
{
    if (!availableSubViewList)
        return;

    QStandardItemModel *model = static_cast<QStandardItemModel *> (availableSubViewList->itemModel());
    model->clear();

    // get all subviews from plugins which support OnScreen
    foreach (const MImSettingsConf::MImSubView &subView, MImSettingsConf::instance().subViews()) {
        QStandardItem *item = new QStandardItem(subView.subViewTitle);
        // set the title of subview as the display role, and ID as the Qt::UserRole role.
        item->setData(subView.subViewTitle, Qt::DisplayRole);
        item->setData(subView.subViewId, MImSubViewIdentifierRole);
        item->setData(subView.pluginName, MImPluginNameRole);
        model->appendRow(item);
    }
    updateActiveSubViewIndex();
}

void MImSettingsWidget::setActiveSubView(const QModelIndex &index)
{
    if (!index.isValid() || !availableSubViewList
        || !availableSubViewList->selectionModel()->isSelected(index))
        return;

    availableSubViewList->setCurrentIndex(index);
    QStandardItemModel *model = static_cast<QStandardItemModel *> (availableSubViewList->itemModel());

    QString pluginName = model->itemFromIndex(index)->data(Qt::UserRole + 1).toString();
    QString subViewId = model->itemFromIndex(index)->data(Qt::UserRole).toString();
    MImSettingsConf::instance().setActivePlugin(pluginName, subViewId);
}
