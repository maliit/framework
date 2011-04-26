/* * This file is part of meego-im-framework *
 *
 * Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#include "mimsubviewmodel.h"

MImSubviewModel::MImSubviewModel(QList<MImSubview> subViews, QObject *parent) :
    QAbstractListModel(parent),
    mSubViews(subViews)
{
}

int MImSubviewModel::rowCount(const QModelIndex &) const
{
    return mSubViews.size();
}

QVariant MImSubviewModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    QList<MImSubview>::size_type row = index.row();

    if (row >= mSubViews.size())
        return QVariant();

    switch (role) {
    case Qt::DisplayRole:
        return QVariant(mSubViews.at(row).displayName);
    case IdRole:
        return QVariant(mSubViews.at(row).id);
    case PluginIdRole:
        return QVariant(mSubViews.at(row).pluginId);
    case SubViewRole:
        return QVariant::fromValue(mSubViews.at(row).subView());
    default:
        return QVariant();
    }
}
