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
#include <QList>
#include <tr1/memory>

#include "minputmethodnamespace.h"
#include "mimplugindescription.h"
#include "mattributeextensionid.h"
#include "mimsubviewdescription.h"
#include "minputcontextconnection.h"

class QRegion;
class MIMPluginManagerPrivate;
class MAttributeExtensionId;
class MAbstractInputMethod;

//! Note: this class is not considered stable API
class MIMPluginManager: public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.meego.inputmethodpluginmanager1")

public:
    /*!
     * \Brief Constructs object MIMPluginManager
     */
    MIMPluginManager(std::tr1::shared_ptr<MInputContextConnection> icConnection, QWidget *proxyWidget);

    virtual ~MIMPluginManager();

    //! Returns names of loaded plugins
    QStringList loadedPluginsNames() const;

    //! Returns names of loaded plugins which support \a state
    QStringList loadedPluginsNames(MInputMethod::HandlerState state) const;

    //! \brief Return information about loaded input method plugins which could work in specified \a state.
    QList<MImPluginDescription> pluginDescriptions(MInputMethod::HandlerState state) const;

    //! \brief Return information about previous, current and next subviews.
    //!
    //! \sa MAbstractInputMethodHost::surroundingSubViewDescriptions()
    QList<MImSubViewDescription> surroundingSubViewDescriptions(MInputMethod::HandlerState state) const;

    //! Returns names of activated plugins
    QStringList activePluginsNames() const;

    //! Returns names of activated plugin for \a state
    QString activePluginsName(MInputMethod::HandlerState state) const;

    //! Returns all subviews (IDs and titles) of loaded plugins which support \a state.
    QMap<QString, QString> availableSubViews(const QString &plugin,
                                             MInputMethod::HandlerState state
                                              = MInputMethod::OnScreen) const;

    //! Returns the ID of active subview of the activated plugin for \a state.
    QString activeSubView(MInputMethod::HandlerState state) const;

    //! Sets \a pluginName as the activated plugin for \a state.
    void setActivePlugin(const QString &pluginName, MInputMethod::HandlerState state);

    //! Sets \a subViewId as the active subview of the activated plugin for \a state.
    void setActiveSubView(const QString &subViewId, MInputMethod::HandlerState state);

    //! Returns true if the connection to D-Bus is valid and the object is properly usable
    bool isDBusConnectionValid() const;

    //! Switches plugin in according to given \a direction
    void switchPlugin(MInputMethod::SwitchDirection direction, MAbstractInputMethod *initiator);

    //! Switches active plugin to inactive plugin with given \a name
    void switchPlugin(const QString &name, MAbstractInputMethod *initiator);

    //! Enables all installed subviews
    void setAllSubViewsEnabled(bool enable);

Q_SIGNALS:
    //!
    // This signal is emitted when the whole painting area of the
    // widget is changed.
    void regionUpdated(const QRegion &region);

    //! This signal is emitted when input method plugins are loaded, unloaded,
    //! enabled or disabled
    void pluginsChanged();

    void pluginLoaded();

public Q_SLOTS:
    //! Show active plugins.
    void showActivePlugins();

    //! Hide active plugins.
    void hideActivePlugins();

    //! Updates the whole painting area for input method objects.
    void updateRegion(const QRegion &region);

private Q_SLOTS:
    //! Update and activate input source.
    void updateInputSource();

    //! Set toolbar to active plugin with given \a id
    void setToolbar(const MAttributeExtensionId &id);

    //! Update the key overrides for active plugin.
    void updateKeyOverrides();

    /*!
     * \brief Handle global attribute change
     * \param targetItem Item name
     * \param attribute Attribute name
     * \param value New attribute value
     */
    void onGlobalAttributeChange(const QString &targetItem,
                                 const QString &attribute,
                                 const QVariant &value);

protected:
    MIMPluginManagerPrivate *const d_ptr;

private:
    Q_DISABLE_COPY(MIMPluginManager)
    Q_DECLARE_PRIVATE(MIMPluginManager)
    Q_PRIVATE_SLOT(d_func(), void _q_syncHandlerMap(int));
    Q_PRIVATE_SLOT(d_func(), void _q_setActiveSubView(const QString &, MInputMethod::HandlerState));
    Q_PRIVATE_SLOT(d_func(), void _q_ensureEmptyRegionWhenHidden());
    Q_PRIVATE_SLOT(d_func(), void _q_onScreenSubViewChanged());

    friend class Ut_MIMPluginManager;
    friend class Ft_MIMPluginManager;
    friend class Ut_MIMSettingsDialog;
};

#endif

