/* * This file is part of Maliit framework *
 *
 * Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 *
 * Contact: maliit-discuss@lists.maliit.org
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
    class SubView {
    public:
        QString plugin;
        QString id;

        explicit SubView();
        explicit SubView(const QString &new_plugin,
                         const QString &new_id = NULL);

        bool operator==(const SubView &other) const;
    };

    explicit MImOnScreenPlugins();

    bool isEnabled(const QString &plugin) const;
    QList<SubView> enabledSubViews(const QString &plugin) const;
    QList<SubView> enabledSubViews() const;

    bool isSubViewEnabled(const SubView &subview) const;
    void setEnabledSubViews(const QList<SubView> &subViews);
    void setAutoEnabledSubViews(const QList<SubView> &subViews);

    void updateAvailableSubViews(const QList<SubView> &availableSubViews);
    bool isSubViewAvailable(const SubView &subview) const;
    bool isSubViewUnavailable(const SubView &subview) const;

    const SubView activeSubView();
    void setActiveSubView(const SubView &subView);
    void setAutoActiveSubView(const SubView &subView);

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
    bool mAllSubviewsEnabled;

};

Q_DECLARE_METATYPE(MImOnScreenPlugins::SubView)
#endif // MIMENABLEDPLUGINS_H
