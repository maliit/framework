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

#ifndef MIMSETTINGSCONF_H
#define MIMSETTINGSCONF_H

#include <QObject>
#include <QMap>
#include <QList>
#include <QStringList>

#include "minputmethodnamespace.h"

class MInputMethodPlugin;
class MAbstractInputMethodSettings;
class QDBusInterface;

class MImSettingsConf : public QObject
{
    Q_OBJECT
public:
    ~MImSettingsConf();

    /*!
     * \brief Get singleton instance
     * \return singleton instance
     */
    static MImSettingsConf &instance();

    //! \brief Create singleton
    static void createInstance();

    //! \brief Destroy singleton
    static void destroyInstance();

    /*!
     * \brief Return all valid input method plugins.
     */
    QList<MInputMethodPlugin *> plugins() const;

    /*!
     * \brief Return all input method settings.
     */
    QMap<QString, MAbstractInputMethodSettings *> settings() const;

    /*!
     * \brief Set \a pluginName as the active input method plugin, and \a subViewId as the active subView.
     */
    void setActivePlugin(const QString &pluginName, const QString &subViewId = "");

    /*!
     * \brief Set \a subViewId as the active subView for current input method plugin.
     */
    void setActiveSubView(const QString &subViewId);

    struct MImSubView {
        QString pluginName;
        QString subViewId;
        QString subViewTitle;
    };

    /*!
     * \brief Return current active subView.
     */
    MImSubView activeSubView() const;

    /*!
     * \brief Return all available subViews.
     */
    QList<MImSubView> subViews() const;

Q_SIGNALS:
    void activeSubViewChanged();

protected:
    MImSettingsConf();

private:
    void loadPlugins();
    void loadSettings();
    bool loadPlugin(const QString &fileName);
    void connectToIMPluginManagerDBus();

    QMap<MInputMethodPlugin *, QString> imPlugins;
    QStringList paths;
    QStringList blacklist;
    QMap<QString, MAbstractInputMethodSettings *> settingList;
    //! Singleton instance
    static MImSettingsConf *imSettingsConfInstance;

    QDBusInterface *impluginMgrIface;

    friend class Ut_MIMSettingsConf;
    friend class Ft_MIMSettingsConf;
};

inline MImSettingsConf &MImSettingsConf::instance()
{
    Q_ASSERT(imSettingsConfInstance);
    return *imSettingsConfInstance;
}

#endif
