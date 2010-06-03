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
#include "mimhandlerstate.h"

class MInputMethodPlugin;
class MInputMethodSettingsBase;
class MGConfItem;

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
    QList<MInputMethodSettingsBase *> settings() const;

protected:
    MImSettingsConf();

private:
    void loadPlugins();
    void loadSettings();
    bool loadPlugin(const QString &fileName);

    QMap<MInputMethodPlugin *, QString> imPlugins;
    QStringList paths;
    QStringList blacklist;
    QList<MInputMethodSettingsBase *> settingList;
    //! Singleton instance
    static MImSettingsConf *imSettingsConfInstance;
};

inline MImSettingsConf &MImSettingsConf::instance()
{
    Q_ASSERT(imSettingsConfInstance);
    return *imSettingsConfInstance;
}

#endif
