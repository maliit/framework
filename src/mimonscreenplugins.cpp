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

#include "mimonscreenplugins.h"

#include <QLocale>
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

MImOnScreenPlugins::SubView::SubView()
    : plugin()
    , id()
{}

MImOnScreenPlugins::SubView::SubView(const QString &new_plugin,
                                     const QString &new_id)
    : plugin(new_plugin)
    , id(new_id)
{}

bool MImOnScreenPlugins::SubView::operator==(const MImOnScreenPlugins::SubView &other) const
{
    return (plugin == other.plugin
            && id == other.id);
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
    updateEnabledSubviews();
    updateActiveSubview();
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

QList<MImOnScreenPlugins::SubView> MImOnScreenPlugins::enabledSubViews() const
{
    return mEnabledSubViews;
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

void MImOnScreenPlugins::setAutoEnabledSubViews(const QList<MImOnScreenPlugins::SubView> &subViews)
{
    // Update the enabled subviews list without saving the configuration to disk
    mEnabledSubViews = subViews;
}

void MImOnScreenPlugins::updateAvailableSubViews(const QList<SubView> &availableSubViews)
{
    mAvailableSubViews = availableSubViews;

    autoDetectActiveSubView();
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

    // Changed subviews cause emission of enabledPluginsChanged() signal
    // because some subview from the setting might not really exists and therefore
    // changed subview might have implications to enabled plugins.
    if (mEnabledSubViews != oldEnabledSubviews) {
        Q_EMIT enabledPluginsChanged();
    }
}

void MImOnScreenPlugins::updateActiveSubview()
{
    const QString &active = mActiveSubViewSettings.value().toString();
    if (active.isEmpty()) {
        mActiveSubView = MImOnScreenPlugins::SubView(MALIIT_DEFAULT_PLUGIN);
        return;
    }

    const QList<SubView> &activeList = fromSettings(QStringList() << active);
    const MImOnScreenPlugins::SubView &subView = activeList.first();

    if (mActiveSubView == subView) {
        return;
    }

    setAutoActiveSubView(subView);
}

const MImOnScreenPlugins::SubView MImOnScreenPlugins::activeSubView()
{
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

void MImOnScreenPlugins::setAutoActiveSubView(const MImOnScreenPlugins::SubView &subView)
{
    // Update the active subview without writing the configuration to disk
    if (mActiveSubView == subView)
        return;

    mActiveSubView = subView;
    Q_EMIT activeSubViewChanged();
}


void MImOnScreenPlugins::setAllSubViewsEnabled(bool enable)
{
    if (mAllSubviewsEnabled != enable) {
        mAllSubviewsEnabled = enable;

        if (mAllSubviewsEnabled) {
            mLastEnabledSubViews = mEnabledSubViews;
        } else {
            if (!mLastEnabledSubViews.contains(mActiveSubView)) {
                mLastEnabledSubViews.append(mActiveSubView);
            }
        }

        setEnabledSubViews(mAllSubviewsEnabled ? mAvailableSubViews
                                               : mLastEnabledSubViews);
    }
}

void MImOnScreenPlugins::autoDetectActiveSubView()
{
    // If no subviews are enabled by the configuration, try to auto-detect
    // them.
    if (enabledSubViews().empty()) {
        autoDetectEnabledSubViews();
    }

    // If we still don't have an enabled subview, enable the first available
    // one.
    if (enabledSubViews().empty()) {
        MImOnScreenPlugins::SubView subView = mAvailableSubViews.first();
        setAutoEnabledSubViews(QList<MImOnScreenPlugins::SubView>() << subView);
    }

    // If we have an active subview in the configuration, check that it is
    // enabled
    // If we don't have an active subview, auto-activate the first enabled
    // one.
    if (mActiveSubView.id.isEmpty() || !isSubViewEnabled(mActiveSubView)) {
        MImOnScreenPlugins::SubView subView = enabledSubViews().first();
        setAutoActiveSubView(subView);
    }

}

void MImOnScreenPlugins::autoDetectEnabledSubViews()
{
    const QString &plugin = mActiveSubView.plugin;
    QList<MImOnScreenPlugins::SubView> to_enable;

    // Try to auto-detect subviews for the selected plugin by looking for
    // subviews that coincide with the languages selected for use on the
    // system.
    // FIXME: This works for the keyboard plugin, but won't work everywhere.
    // The methodology for auto-configuring subviews should be somehow
    // plugin-dictated.
    QStringList langs = QLocale::system().uiLanguages();
    Q_FOREACH (QString lang, langs) {
        // Convert to lower case, remove any .utf8 suffix, and use _ as
        // the separator between language and country.
        lang = lang.split('.')[0].toLower().replace("-", "_");

        MImOnScreenPlugins::SubView subView(plugin, lang);

        // First try the language code as-is
        if (isSubViewAvailable(subView) && !to_enable.contains(subView)) {
            to_enable << subView;
            continue;
        }

        // See if we get a match if we expand "de" to "de_de"
        if (!lang.contains('_')) {
            subView.id = lang + "_" + lang;
            if (isSubViewAvailable(subView) && !to_enable.contains(subView)) {
                to_enable << subView;
            }
            continue;
        }

        // See if we get a match if we trim "de_at" to "de"
        subView.id = lang.split("_").first();
        if (isSubViewAvailable(subView) && !to_enable.contains(subView)) {
            to_enable << subView;
        }
    }

    if (!to_enable.isEmpty()) {
        setAutoEnabledSubViews(to_enable);
    }
}
