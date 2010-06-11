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
#include <QMap>
#include "mimdirection.h"
#include "mimhandlerstate.h"

class MIMPluginManagerPrivate;
class QRegion;

class MIMPluginManager: public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.maemo.inputmethodpluginmanager1")

public:
    /*!
     * \Brief Constructs object MIMPluginManager
     * \param deleteIMtimeout int timeout for deletion of inactive input method objects.
     * Use default value to set default timeout.
     */
    MIMPluginManager();

    virtual ~MIMPluginManager();

    //! Returns names of loaded plugins
    QStringList loadedPluginsNames() const;

    //! Returns names of loaded plugins which support \a state
    QStringList loadedPluginsNames(MIMHandlerState state) const;

    //! Returns names of activated plugins
    QStringList activePluginsNames() const;

    //! Returns names of activated plugin for \a state
    QString activePluginsName(MIMHandlerState state) const;

    //! Returns all subviews (IDs and titles) of loaded plugins which support \a state.
    QMap<QString, QString> availableSubViews(const QString &plugin, MIMHandlerState state = OnScreen) const;

    //! Returns the ID of active subview of the activated plugin for \a state.
    QString activeSubView(MIMHandlerState state) const;

    //! Sets \a pluginName as the activated plugin for \a state.
    void setActivePlugin(const QString &pluginName, MIMHandlerState state);

    //! Sets \a subViewId as the active subview of the activated plugin for \a state.
    void setActiveSubView(const QString &subViewId, MIMHandlerState state);

signals:
    //!
    // This signal is emitted when the whole painting area of the
    // widget is changed.
    void regionUpdated(const QRegion &region);

public slots:
    //! Hide all active plugins.
    void hideActivePlugins();

private slots:
    //! Update and activate input source.
    void updateInputSource();

    //! Switches plugin in according to given \a direction
    void switchPlugin(M::InputMethodSwitchDirection direction);

    //! Switches active plugin to inactive plugin with given \a name
    void switchPlugin(const QString &name);

    //! Shows settings for all loaded inputmethod plugins.
    void showInputMethodSettings();

    //! Updates the whole painting area for input method objects.
    void updateRegion(const QRegion &region);

protected:
    MIMPluginManagerPrivate *const d_ptr;

private:
    Q_DISABLE_COPY(MIMPluginManager)
    Q_DECLARE_PRIVATE(MIMPluginManager)
    Q_PRIVATE_SLOT(d_func(), void _q_syncHandlerMap(int));
    Q_PRIVATE_SLOT(d_func(), void _q_setActiveSubView(const QString &, MIMHandlerState));

    friend class Ut_MIMPluginManager;
    friend class Ft_MIMPluginManager;
};

#endif

