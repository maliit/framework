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

#ifndef MIMSUBVIEWMODEL_H
#define MIMSUBVIEWMODEL_H

#include <QAbstractListModel>

#include "mimonscreenplugins.h"

struct MImSubview
{
    MImSubview(const QString &newId, const QString &newDisplayName, const QString &newPluginId)
        : id(newId), displayName(newDisplayName), pluginId(newPluginId)
    {}

    QString id;
    QString displayName;
    QString pluginId;

    MImOnScreenPlugins::SubView subView() const
    {
        return MImOnScreenPlugins::SubView(pluginId, id);
    }
};

class MImSubviewModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit MImSubviewModel(QList<MImSubview> subViews, QObject *parent = 0);

    /*! \reimp */
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    /*! \reimp_end */

    static const int IdRole = Qt::UserRole;
    static const int PluginIdRole = Qt::UserRole + 1;
    static const int SubViewRole = Qt::UserRole + 2;

private:
    QList<MImSubview> mSubViews;
};

#endif // MIMSUBVIEWMODEL_H
