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

#ifndef MIMPLUGINMANAGER_H
#define MIMPLUGINMANAGER_H

#include <QObject>

class QRegion;
class MIMPluginManagerPrivate;


class MIMPluginManager: public QObject
{
    Q_OBJECT

public:
    /*!
     * \Brief Constructs object MIMPluginManager
     * \param deleteIMtimeout int timeout for deletion of inactive input method objects.
     * Use default value to set default timeout.
     */
    MIMPluginManager();

    virtual ~MIMPluginManager();

    //!Returns names of loaded plugins
    QStringList loadedPluginsNames() const;

    //!Returns names of activated plugins
    QStringList activePluginsNames() const;

    //!Returns names of existing input methods
    QStringList activeInputMethodsNames() const;

    /*!
     * \brief Set timeout for deletion of inactive input method objects.
     * \param timeout int New timeout
     */
    void setDeleteIMTimeout(int timeout);

signals:
    //!
    // This signal is emitted when the whole painting area of the
    // widget is changed.
    void regionUpdated(const QRegion &region);

private slots:
    //! This method reloads map from keyboard handler to plugin
    void reloadHandlerMap();

    //! This method deletes all inactive input method objects
    void deleteInactiveIM();

    //! Update and activate input source.
    void updateInputSource();

private:
    Q_DISABLE_COPY(MIMPluginManager)

    MIMPluginManagerPrivate *d;

    friend class Ut_MIMPluginManager;
};

#endif

