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

#include <MAbstractCellCreator>
#include <MBanner>
#include <MContentItem>
#include <MList>

#include <mabstractinputmethodsettings.h>
#include <minputmethodplugin.h>

#include "mimsubviewmodel.h"

#include "mimsettingsconf.h"

namespace {
    //!object name for settings' widgets
    const QString ObjectNameActiveInputMethodWidget("ActiveInputMethodWidget");
    const QString ObjectNameSelectedKeyboardsItem("SelectedKeyboardsItem");

    const QString StyleHeader("CommonApplicationHeaderInverted");
    const QString StyleBasicListItem("CommonBasicListItemInverted");
    const QString StylePluginContainer("CommonLargePanel");
    const QString StylePluginHeader("CommonGroupHeaderInverted");

    const QString DefaultPlugin("MeegoKeyboard");
};

class MSubViewCellCreator: public MAbstractCellCreator<MContentItem>
{
public:
    /*! \reimp */
    virtual MWidget *createCell(const QModelIndex &index,
                                MWidgetRecycler &recycler) const;
    virtual void updateCell(const QModelIndex &index, MWidget *cell) const;
    /*! \reimp_end */
};

MWidget *MSubViewCellCreator::createCell(const QModelIndex &index,
                                         MWidgetRecycler &recycler) const
{
    MContentItem *cell = qobject_cast<MContentItem *>(recycler.take("MContentItem"));
    if (!cell) {
        cell = new MContentItem(MContentItem::SingleTextLabel);
    }
    updateCell(index, cell);
    return cell;
}

void MSubViewCellCreator::updateCell(const QModelIndex &index, MWidget *cell) const
{
    MContentItem *contentItem = qobject_cast<MContentItem *>(cell);

    contentItem->setTitle(index.data(Qt::DisplayRole).toString());
}

MImSettingsWidget::MImSettingsWidget()
    : DcpWidget(),
      mainLayout(0),
      headerLabel(0),
      activeSubViewItem(0),
      availableSubViewItem(0),
      availableSubViewList(),
      keyboardDialog(),
      keyboardList(),
      onscreenPlugins(),
      selectedSubViews(),
      settingsLabelMap(),
      settingsContainerMap()
{
    updateSelectedSubViews();
    initWidget();
}


// pressing the back button means accept for us
bool MImSettingsWidget::back()
{
    return true;
}

MImSettingsWidget::~MImSettingsWidget()
{
    delete availableSubViewList.data();
    delete keyboardDialog.data();
}

void MImSettingsWidget::initWidget()
{
    mainLayout = new QGraphicsLinearLayout(Qt::Vertical, this);
    mainLayout->setContentsMargins(0., 0., 0., 0.);

    // Header
    headerLabel = new MLabel(this);
    headerLabel->setStyleName(StyleHeader);
    mainLayout->addItem(headerLabel);
    mainLayout->setStretchFactor(headerLabel, 0);

    // Active input method selector
    // We are using MBasicListItem instead of MContentItem because
    // the latter is not supported by theme
    activeSubViewItem = new MBasicListItem(MBasicListItem::TitleWithSubtitle, this);
    activeSubViewItem->setObjectName(ObjectNameActiveInputMethodWidget);
    activeSubViewItem->setStyleName(StyleBasicListItem);
    mainLayout->addItem(activeSubViewItem);
    mainLayout->setStretchFactor(activeSubViewItem, 0);
    connect(activeSubViewItem, SIGNAL(clicked()), this, SLOT(showAvailableSubViewList()));

    // All available subviews.
    availableSubViewItem = new MBasicListItem(MBasicListItem::TitleWithSubtitle, this);
    availableSubViewItem->setObjectName(ObjectNameSelectedKeyboardsItem);
    availableSubViewItem->setStyleName(StyleBasicListItem);
    mainLayout->addItem(availableSubViewItem);
    mainLayout->setStretchFactor(availableSubViewItem, 0);
    connect(availableSubViewItem, SIGNAL(clicked()), this, SLOT(showSelectedKeyboardsDialog()));

    const QMap<QString, MAbstractInputMethodSettings *> &map = MImSettingsConf::instance().settings();

    if (map.contains(DefaultPlugin))
        addPluginSettings(DefaultPlugin, map[DefaultPlugin]);

    QMap<QString, MAbstractInputMethodSettings *>::const_iterator end = map.constEnd();
    for (QMap<QString, MAbstractInputMethodSettings *>::const_iterator iter(map.constBegin()); iter != end; ++iter) {
        if (iter.key() == DefaultPlugin)
            continue;

        addPluginSettings(iter.key(), iter.value());
    }

    setLayout(mainLayout);
    mainLayout->addStretch();
    retranslateUi();
    connect(&onscreenPlugins, SIGNAL(activeSubViewChanged()), this, SLOT(syncActiveSubView()));
}

void MImSettingsWidget::showAvailableSubViewList()
{
    if (!availableSubViewList) {
        availableSubViewList = new MPopupList();
        //% "Active input method"
        availableSubViewList.data()->setTitle(qtTrId("qtn_txts_active_input_method"));
        connect(availableSubViewList.data(), SIGNAL(clicked(const QModelIndex &)),
                this, SLOT(setActiveSubView(const QModelIndex &)));
    }
    // always update available subview model avoid missing some updated subviews
    updateAvailableSubViewModel();
    availableSubViewList.data()->appear(MSceneWindow::DestroyWhenDone);
}

void MImSettingsWidget::updateAvailableSubViewModel()
{
    if (!availableSubViewList)
        return;

    availableSubViewList.data()->setItemModel(new MImSubviewModel(selectedSubViews, availableSubViewList.data()));

    updateActiveSubViewIndex();
}

void MImSettingsWidget::updateActiveSubViewIndex()
{
    if (!availableSubViewList)
        return;

    const MImOnScreenPlugins::SubView &activeSubView = onscreenPlugins.activeSubView();

    QAbstractItemModel *model = availableSubViewList.data()->itemModel();

    for (int row = 0; row < model->rowCount(); ++row) {
        const QModelIndex &index = model->index(row, 0);
        const MImOnScreenPlugins::SubView &subview = index.data(MImSubviewModel::SubViewRole).value<MImOnScreenPlugins::SubView>();
        if (activeSubView == subview) {
            availableSubViewList.data()->setCurrentIndex(index);
            availableSubViewList.data()->scrollTo(index);
            break;
        }
    }
}

void MImSettingsWidget::setActiveSubView(const QModelIndex &index)
{
    if (!index.isValid() || !availableSubViewList
        || !availableSubViewList.data()->selectionModel()->isSelected(index))
        return;

    availableSubViewList.data()->setCurrentIndex(index);

    const QVariant &data = index.data(MImSubviewModel::SubViewRole);
    const MImOnScreenPlugins::SubView &subview = data.value<MImOnScreenPlugins::SubView>();
    onscreenPlugins.setActiveSubView(subview);
}

void MImSettingsWidget::showSelectedKeyboardsDialog()
{
    if (!keyboardDialog) {
        keyboardDialog = new MDialog;

        keyboardList = new MList(keyboardDialog.data());
        keyboardList.data()->setCellCreator(new MSubViewCellCreator);
        keyboardList.data()->setSelectionMode(MList::MultiSelection);

        keyboardDialog.data()->setCentralWidget(keyboardList.data());
        keyboardDialog.data()->addButton(M::DoneButton);

        connect(keyboardList.data(), SIGNAL(itemClicked(const QModelIndex &)),
                this, SLOT(updateSelectedKeyboards(const QModelIndex &)));
        connect(keyboardDialog.data(), SIGNAL(accepted()),
                this, SLOT(selectKeyboards()));
    }

    updateSelectedKeyboardsModel();
    updateSelectedKeyboards();

    keyboardDialog.data()->exec();
    delete keyboardDialog.data();
}

void MImSettingsWidget::updateSelectedKeyboardsModel()
{
    if (!keyboardList)
        return;

    QList<MImSubview> subViews;
    foreach (const MImSettingsConf::MImSubView &subview, MImSettingsConf::instance().subViews()) {
        subViews.push_back(MImSubview(subview.subViewId, subview.subViewTitle, subview.pluginName));
    }
    keyboardList.data()->setItemModel(new MImSubviewModel(subViews, keyboardList.data()));

    updateSelectedKeyboardsSelection();
}

void MImSettingsWidget::updateSelectedKeyboardsSelection()
{
    if (!keyboardList)
        return;

    QAbstractItemModel *model = keyboardList.data()->itemModel();
    QItemSelectionModel *selection = keyboardList.data()->selectionModel();
    selection->clear();

    for (int i = 0; i < model->rowCount(); i++) {
        const QModelIndex &index = model->index(i, 0);
        const QVariant &data = model->data(index, MImSubviewModel::SubViewRole);
        const MImOnScreenPlugins::SubView &subView = data.value<MImOnScreenPlugins::SubView>();

        if (onscreenPlugins.isSubViewEnabled(subView)) {
            selection->select(index, QItemSelectionModel::Select);
        }
    }
}

void MImSettingsWidget::updateSelectedKeyboards(const QModelIndex &)
{
    if (!keyboardList || !keyboardDialog)
        return;

    const QModelIndexList &indexList = keyboardList.data()->selectionModel()->selectedIndexes();

    //% “Installed keyboards (%1)”
    QString title = qtTrId("qtn_txts_installed_keyboards").arg(indexList.size());
    keyboardDialog.data()->setTitle(title);
}

void MImSettingsWidget::selectKeyboards()
{
    if (!keyboardList)
        return;

    QModelIndexList indexList = keyboardList.data()->selectionModel()->selectedIndexes();

    QList<MImOnScreenPlugins::SubView> selected;
    foreach (const QModelIndex &i, indexList) {
        selected.push_back(i.data(MImSubviewModel::SubViewRole).value<MImOnScreenPlugins::SubView>());
    }
    onscreenPlugins.setEnabledSubViews(selected);

    updateSelectedSubViews();

    // "No keyboard is selected" notification
    if (indexList.isEmpty()) {
        MBanner *noKeyboardsNotification = new MBanner;

        // It is needed to set the proper style name to have properly wrapped, multiple lines
        // with too much content. The MBanner documentation also emphasises to specify the
        // style name for the banners explicitly in the code.
        noKeyboardsNotification->setStyleName("InformationBanner");
        //% "Note: you have uninstalled all virtual keyboards"
        noKeyboardsNotification->setTitle(qtTrId("qtn_txts_no_keyboards_notification"));
        noKeyboardsNotification->appear(MSceneWindow::DestroyWhenDone);
    }
    //update titles
    retranslateUi();
}

void MImSettingsWidget::retranslateUi()
{
    if (!activeSubViewItem || !headerLabel || !availableSubViewItem)
        return;

    //% "Text input"
    headerLabel->setText(qtTrId("qtn_txts_text_input"));
    //% "Active input method"
    activeSubViewItem->setTitle(qtTrId("qtn_txts_active_input_method"));
    updateActiveSubViewTitle();

    //% “Installed keyboards (%1)”
    QString title = qtTrId("qtn_txts_installed_keyboards").arg(selectedSubViews.size());
    availableSubViewItem->setTitle(title);
    if (selectedSubViews.empty()) {
        //% "No keyboards installed"
        availableSubViewItem->setSubtitle(qtTrId("qtn_txts_no_keyboards"));
    } else {
        QStringList names;
        foreach (const MImSubview &subView, selectedSubViews) {
            names.push_back(subView.displayName);
        }
        availableSubViewItem->setSubtitle(names.join(", "));
    }

    foreach (MLabel *label, settingsLabelMap.values()) {
        label->setText(settingsLabelMap.key(label)->title());
    }
    if (!availableSubViewList.isNull()) {
        //% "Active input method"
        availableSubViewList.data()->setTitle(qtTrId("qtn_txts_active_input_method"));
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

    const MImOnScreenPlugins::SubView &activeSubView = onscreenPlugins.activeSubView();

    foreach (const MImSubview &subView, selectedSubViews) {
        if (subView.pluginId == activeSubView.plugin
                && subView.id == activeSubView.id) {
            activeSubViewItem->setSubtitle(subView.displayName);
            break;
        }
    }
}


void MImSettingsWidget::updateSelectedSubViews()
{
    selectedSubViews.clear();
    foreach (const MImSettingsConf::MImSubView &subview, MImSettingsConf::instance().subViews()) {
        if (onscreenPlugins.isSubViewEnabled(MImOnScreenPlugins::SubView(subview.pluginName, subview.subViewId))) {
            selectedSubViews.push_back(MImSubview(subview.subViewId, subview.subViewTitle, subview.pluginName));
        }
    }
}

void MImSettingsWidget::addPluginSettings(const QString &plugin,
                                          MAbstractInputMethodSettings *settings)
{
    QGraphicsWidget *contentWidget = settings->createContentWidget(this);
    if (!contentWidget)
        return;

    QGraphicsLinearLayout *layout = new QGraphicsLinearLayout(Qt::Vertical);
    MContainer *container = new MContainer(this);
    container->setStyleName(StylePluginContainer);
    container->setHeaderVisible(false);
    container->centralWidget()->setLayout(layout);

    MLabel *header = new MLabel(settings->title(), this);
    header->setStyleName(StylePluginHeader);
    //TODO: icon for the settings.
    layout->addItem(header);
    layout->addItem(contentWidget);

    mainLayout->addItem(container);
    mainLayout->setStretchFactor(container, 0);

    settingsLabelMap.insert(settings, header);
    settingsContainerMap.insert(plugin, container);
}

