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

#include "minputmethodnamespace.h"

class QRegion;
class MIMPluginManagerPrivate;
class MToolbarId;
class MInputMethodBase;


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
    QStringList loadedPluginsNames(MInputMethod::HandlerState state) const;

    //! Returns names of activated plugins
    QStringList activePluginsNames() const;

    //! Returns names of activated plugin for \a state
    QString activePluginsName(MInputMethod::HandlerState state) const;

    //! Returns all subviews (IDs and titles) of loaded plugins which support \a state.
    QMap<QString, QString> availableSubViews(const QString &plugin,
                                             MInputMethod::HandlerState state = MInputMethod::OnScreen) const;

    //! Returns the ID of active subview of the activated plugin for \a state.
    QString activeSubView(MInputMethod::HandlerState state) const;

    //! Sets \a pluginName as the activated plugin for \a state.
    void setActivePlugin(const QString &pluginName, MInputMethod::HandlerState state);

    //! Sets \a subViewId as the active subview of the activated plugin for \a state.
    void setActiveSubView(const QString &subViewId, MInputMethod::HandlerState state);

    //! Returns true if the connection to D-Bus is valid and the object is properly usable
    bool isDBusConnectionValid() const;

    //! Switches plugin in according to given \a direction
    void switchPlugin(MInputMethod::SwitchDirection direction, MInputMethodBase *initiator);

    //! Switches active plugin to inactive plugin with given \a name
    void switchPlugin(const QString &name, MInputMethodBase *initiator);

    //! Shows settings for all loaded inputmethod plugins.
    void showInputMethodSettings();

signals:
    //!
    // This signal is emitted when the whole painting area of the
    // widget is changed.
    void regionUpdated(const QRegion &region);

public slots:
    //! Show active plugins.
    void showActivePlugins();

    //! Hide active plugins.
    void hideActivePlugins();

private slots:
    //! Update and activate input source.
    void updateInputSource();

    //! Updates the whole painting area for input method objects.
    void updateRegion(const QRegion &region);

    //! Set toolbar to active plugin with given \a id
    void setToolbar(const MToolbarId &id);

protected:
    MIMPluginManagerPrivate *const d_ptr;

private:
    Q_DISABLE_COPY(MIMPluginManager)
    Q_DECLARE_PRIVATE(MIMPluginManager)
    Q_PRIVATE_SLOT(d_func(), void _q_syncHandlerMap(int));
    Q_PRIVATE_SLOT(d_func(), void _q_setActiveSubView(const QString &, MInputMethod::HandlerState));

    friend class Ut_MIMPluginManager;
    friend class Ft_MIMPluginManager;
    friend class Ut_MIMSettingsDialog;
};

#endif

