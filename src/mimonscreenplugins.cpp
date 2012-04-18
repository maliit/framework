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

#include "mimonscreenplugins.h"

#include <QString>
#include <QSet>
#include <QDebug>

#include <algorithm>
#include <tr1/functional>

using namespace std::tr1::placeholders;

namespace
{
    const char * const EnabledSubViews = MALIIT_CONFIG_ROOT"onscreen/enabled";
    const char * const ActiveSubView   = MALIIT_CONFIG_ROOT"onscreen/active";

    bool equalPlugin(const MImOnScreenPlugins::SubView &subView, const QString &plugin)
    {
        return subView.plugin == plugin;
    }

    bool notEqualPlugin(const MImOnScreenPlugins::SubView &subView, const QString &plugin)
    {
        return subView.plugin != plugin;
    }

    QStringList toSettings(const QList<MImOnScreenPlugins::SubView> &subViews)
    {
        QStringList result;

        Q_FOREACH(const MImOnScreenPlugins::SubView &subView, subViews) {
            result.push_back(subView.plugin + ":" + subView.id);
        }

        return result;
    }

    QList<MImOnScreenPlugins::SubView> fromSettings(const QStringList& list)
    {
        QList<MImOnScreenPlugins::SubView> result;

        Q_FOREACH (const QString &value, list) {
            QString plugin = value.section(':', 0, 0);
            QString subview = value.section(':', 1, -1);

            result.push_back(MImOnScreenPlugins::SubView(plugin, subview));
        }

        return result;
    }

    QSet<QString> findEnabledPlugins(const QList<MImOnScreenPlugins::SubView> &enabledSubViews)
    {
        QSet<QString> result;

        Q_FOREACH (const MImOnScreenPlugins::SubView &subView, enabledSubViews) {
            result.insert(subView.plugin);
        }

        return result;
    }
}

MImOnScreenPlugins::MImOnScreenPlugins():
    QObject(),
    mAvailableSubViews(),
    mEnabledSubViews(),
    mLastEnabledSubViews(),
    mActiveSubView(),
    mEnabledSubViewsSettings(EnabledSubViews),
    mActiveSubViewSettings(ActiveSubView),
    mAllSubviewsEnabled(false)
{
    connect(&mEnabledSubViewsSettings, SIGNAL(valueChanged()),
            this, SLOT(updateEnabledSubviews()));
    connect(&mActiveSubViewSettings, SIGNAL(valueChanged()),
            this, SLOT(updateActiveSubview()));
    updateActiveSubview();
    updateEnabledSubviews();
}

bool MImOnScreenPlugins::isEnabled(const QString &plugin) const
{
    QList<MImOnScreenPlugins::SubView> mEnabledAndAvailableSubViews;

    std::remove_copy_if(mEnabledSubViews.begin(), mEnabledSubViews.end(),
                        std::back_inserter(mEnabledAndAvailableSubViews),
                        std::tr1::bind(&MImOnScreenPlugins::isSubViewUnavailable, this, _1));

    return std::find_if(mEnabledAndAvailableSubViews.begin(), mEnabledAndAvailableSubViews.end(),
                        std::tr1::bind(equalPlugin, _1, plugin)) != mEnabledAndAvailableSubViews.end();
}

bool MImOnScreenPlugins::isSubViewEnabled(const SubView &subView) const
{
    return mEnabledSubViews.contains(subView);
}

QList<MImOnScreenPlugins::SubView> MImOnScreenPlugins::enabledSubViews(const QString &plugin) const
{
    QList<MImOnScreenPlugins::SubView> result;
    std::remove_copy_if(mEnabledSubViews.begin(), mEnabledSubViews.end(),
                        std::back_inserter(result), std::tr1::bind(notEqualPlugin, _1, plugin));
    return result;
}

void MImOnScreenPlugins::setEnabledSubViews(const QList<MImOnScreenPlugins::SubView> &subViews)
{
    mEnabledSubViewsSettings.set(QVariant(toSettings(subViews)));
}

void MImOnScreenPlugins::updateAvailableSubViews(const QList<SubView> &availableSubViews)
{
    mAvailableSubViews = availableSubViews;
}

bool MImOnScreenPlugins::isSubViewAvailable(const SubView &subview) const
{
    return mAvailableSubViews.contains(subview);
}

bool MImOnScreenPlugins::isSubViewUnavailable(const SubView &subview) const
{
    return !mAvailableSubViews.contains(subview);
}

void MImOnScreenPlugins::updateEnabledSubviews()
{
    const QStringList &list = mEnabledSubViewsSettings.value().toStringList();
    const QList<SubView> oldEnabledSubviews = mEnabledSubViews;
    mEnabledSubViews = fromSettings(list);
    enabledPlugins = findEnabledPlugins(mEnabledSubViews);

    if (!isSubViewEnabled(mActiveSubView) && !mEnabledSubViews.empty()) {
        setActiveSubView(mEnabledSubViews.first());
    }

    // Changed subviews cause emission of enabledPluginsChanged() signal
    // because some subview from GConf might not really exists and therefore
    // changed subview might have implications to enabled plugins.
    if (mEnabledSubViews != oldEnabledSubviews) {
        Q_EMIT enabledPluginsChanged();
    }
}

void MImOnScreenPlugins::updateActiveSubview()
{
    const QString &active = mActiveSubViewSettings.value().toString();
    if (active.isEmpty())
        return;

    const QList<SubView> &activeList = fromSettings(QStringList() << active);
    if (activeList.empty())
        return;

    const MImOnScreenPlugins::SubView &subView = activeList.first();
    if (mActiveSubView == subView)
        return;
    mActiveSubView = subView;
    Q_EMIT activeSubViewChanged();
}

const MImOnScreenPlugins::SubView MImOnScreenPlugins::activeSubView()
{
    if (mActiveSubView.id.isEmpty()) {
        // Subview id might be empty because we jumped to a new plugin,
        // without calling MIMPluginManager::setActivePlugin. In this case, we
        // try to guess the active subview from the first enabled subview of
        // that plugin:
        Q_FOREACH (const SubView &sub, mEnabledSubViews) {
            if (sub.plugin == mActiveSubView.plugin) {
                setActiveSubView(sub);
                break;
            }
        }
    }

    return mActiveSubView;
}

void MImOnScreenPlugins::setActiveSubView(const MImOnScreenPlugins::SubView &subView)
{
    if (mActiveSubView == subView)
        return;
    mActiveSubView = subView;
    QList<MImOnScreenPlugins::SubView> subViews;
    subViews << subView;
    mActiveSubViewSettings.set(toSettings(subViews));
    Q_EMIT activeSubViewChanged();
}

void MImOnScreenPlugins::setAllSubViewsEnabled(bool enable)
{
    if (mAllSubviewsEnabled != enable) {
        mAllSubviewsEnabled = enable;

        if (mAllSubviewsEnabled) {
            mLastEnabledSubViews = mEnabledSubViews;
        } else {
            if (not mLastEnabledSubViews.contains(mActiveSubView)) {
                mLastEnabledSubViews.append(mActiveSubView);
            }
        }

        setEnabledSubViews(mAllSubviewsEnabled ? mAvailableSubViews
                                               : mLastEnabledSubViews);
    }
}
