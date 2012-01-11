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

#ifndef MIMONSCREENPLUGINS_H
#define MIMONSCREENPLUGINS_H

#include <QVariant>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QSet>

#include "mimsettings.h"

/*! \ingroup maliitserver
 * \brief Check the current status of plugins for a subview.
 */
class MImOnScreenPlugins: public QObject
{
    Q_OBJECT

public:
    MImOnScreenPlugins();

    class SubView {
    public:
        SubView() : plugin(), id()
        {}
        SubView(const QString &newPlugin, const QString &newId) :
            plugin(newPlugin),
            id(newId)
        {}

        QString plugin;
        QString id;

        bool operator==(const SubView &other) const
        {
            return plugin == other.plugin && id == other.id;
        }
    };

    bool isEnabled(const QString &plugin) const;
    QList<SubView> enabledSubViews(const QString &plugin) const;

    bool isSubViewEnabled(const SubView &subview) const;
    void setEnabledSubViews(const QList<SubView> &subViews);

    void updateAvailableSubViews(const QList<SubView> &availableSubViews);
    bool isSubViewAvailable(const SubView &subview) const;
    bool isSubViewUnavailable(const SubView &subview) const;

    const SubView activeSubView();
    void setActiveSubView(const SubView &subView);

    void setAllSubViewsEnabled(bool enable);

Q_SIGNALS:
    void activeSubViewChanged();

    void enabledPluginsChanged();

private Q_SLOTS:
    void updateEnabledSubviews();
    void updateActiveSubview();

private:
    QList<SubView> mAvailableSubViews;
    QList<SubView> mEnabledSubViews;
    QList<SubView> mLastEnabledSubViews;
    SubView mActiveSubView;

    MImSettings mEnabledSubViewsSettings;
    MImSettings mActiveSubViewSettings;

    QSet<QString> enabledPlugins; //should be updated when mEnabledSubViews is changed

};

Q_DECLARE_METATYPE(MImOnScreenPlugins::SubView);

#endif // MIMENABLEDPLUGINS_H
