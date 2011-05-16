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

#ifndef MIMSETTINGSWIDGET_H
#define MIMSETTINGSWIDGET_H

#include <DcpWidget>
#include <QModelIndex>
#include <QWeakPointer>

#include "mimonscreenplugins.h"
#include "mimsubviewmodel.h"

class MPopupList;
class MAbstractInputMethodSettings;
class MDialog;
class MLabel;
class MBasicListItem;
class MList;
class MContainer;
class QGraphicsLinearLayout;

class MImSettingsWidget : public DcpWidget
{
    Q_OBJECT

public:
    MImSettingsWidget();
    virtual ~MImSettingsWidget();

protected:
    void initWidget();

    //! reimp
    virtual void retranslateUi();
    virtual bool back();
    //! reimp_end

private slots:
    void syncActiveSubView();
    void showAvailableSubViewList();
    void setActiveSubView(const QModelIndex &);
    void updateAvailableSubViewModel();
    void updateActiveSubViewIndex();

    void showSelectedKeyboardsDialog();
    void updateSelectedKeyboards(const QModelIndex &index = QModelIndex());
    void selectKeyboards();

private:
    void updateActiveSubViewTitle();
    void updateSelectedKeyboardsModel();
    void updateSelectedKeyboardsSelection();
    void updateSelectedSubViews();

    void addPluginSettings(const QString &plugin, MAbstractInputMethodSettings *settings);
    void updatePluginContainerVisibility(const QString &plugin, MContainer *container);

    QGraphicsLinearLayout* mainLayout;
    MLabel *headerLabel;
    MBasicListItem *activeSubViewItem;
    MBasicListItem *availableSubViewItem;

    QWeakPointer<MPopupList> availableSubViewList;
    QWeakPointer<MDialog> keyboardDialog;
    QWeakPointer<MList> keyboardList;

    MImOnScreenPlugins onscreenPlugins;
    QList<MImSubview> selectedSubViews;

    QMap<MAbstractInputMethodSettings *, MLabel *> settingsLabelMap;
    QMap<QString, MContainer *> settingsContainerMap;
};
#endif
