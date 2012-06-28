/* * This file is part of maliit-framework *
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

#include <maliit/namespace.h>

#include <maliit/plugins/plugindescription.h>
#include <maliit/plugins/subviewdescription.h>

#include "mattributeextensionid.h"
#include "minputcontextconnection.h"
#include "abstractsurfacegroupfactory.h"

class QRegion;
class MIMPluginManagerPrivate;
class MAttributeExtensionId;
class MAbstractInputMethod;
class MAttributeExtensionManager;

namespace Maliit {
namespace Plugins {
    class AbstractPluginSetting;
}
}
using Maliit::Plugins::AbstractPluginSetting;

//! \internal
/*! \ingroup maliitserver
 * \brief Manager of MInputMethodPlugin instances.
 * \note this class is not considered stable API
 */
class MIMPluginManager: public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.meego.inputmethodpluginmanager1")

public:
    /*!
     * \Brief Constructs object MIMPluginManager
     */
    MIMPluginManager(std::tr1::shared_ptr<MInputContextConnection> icConnection, QSharedPointer<Maliit::Server::AbstractSurfaceGroupFactory>);

    virtual ~MIMPluginManager();

    //! Returns names of loaded plugins
    QStringList loadedPluginsNames() const;

    //! Returns names of loaded plugins which support \a state
    QStringList loadedPluginsNames(Maliit::HandlerState state) const;

    //! \brief Return information about loaded input method plugins which could work in specified \a state.
    QList<MImPluginDescription> pluginDescriptions(Maliit::HandlerState state) const;

    //! \brief Return information about previous, current and next subviews.
    //!
    //! \sa MAbstractInputMethodHost::surroundingSubViewDescriptions()
    QList<MImSubViewDescription> surroundingSubViewDescriptions(Maliit::HandlerState state) const;

    //! Returns names of activated plugins
    QStringList activePluginsNames() const;

    //! Returns names of activated plugin for \a state
    QString activePluginsName(Maliit::HandlerState state) const;

    //! Returns all subviews (IDs and titles) of loaded plugins which support \a state.
    QMap<QString, QString> availableSubViews(const QString &plugin,
                                             Maliit::HandlerState state
                                              = Maliit::OnScreen) const;

    //! Returns the ID of active subview of the activated plugin for \a state.
    QString activeSubView(Maliit::HandlerState state) const;

    //! Sets \a pluginName as the activated plugin for \a state.
    void setActivePlugin(const QString &pluginName, Maliit::HandlerState state);

    //! Sets \a subViewId as the active subview of the activated plugin for \a state.
    void setActiveSubView(const QString &subViewId, Maliit::HandlerState state);

    //! Switches plugin in according to given \a direction
    void switchPlugin(Maliit::SwitchDirection direction, MAbstractInputMethod *initiator);

    //! Switches active plugin to inactive plugin with given \a name
    void switchPlugin(const QString &name, MAbstractInputMethod *initiator);

    //! Enables all installed subviews
    void setAllSubViewsEnabled(bool enable);

    //! Register a setting entry for the specified plugin
    AbstractPluginSetting *registerPluginSetting(const QString &pluginId,
                                                 const QString &pluginDescription,
                                                 const QString &key,
                                                 const QString &description,
                                                 Maliit::SettingEntryType type,
                                                 const QVariantMap &attributes);

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

    void resetInputMethods();

    //! Updates the whole painting area for input method objects.
    void updateRegion(const QRegion &region);

private Q_SLOTS:
    //! Update and activate input source.
    void updateInputSource();

    //! Set toolbar to active plugin with given \a id
    void setToolbar(const MAttributeExtensionId &id);

    //! Update the key overrides for active plugin.
    void updateKeyOverrides();

    void handleAppOrientationChanged(int angle);
    void handleAppOrientationAboutToChange(int angle);

    void handleClientChange();

    void handleWidgetStateChanged(unsigned int clientId, const QMap<QString, QVariant> &newState,
                                  const QMap<QString, QVariant> &oldState, bool focusChanged);
    void handleMouseClickOnPreedit(const QPoint &pos, const QRect &preeditRect);
    void handlePreeditChanged(const QString &text, int cursorPos);

    void processKeyEvent(QEvent::Type keyType, Qt::Key keyCode,
                         Qt::KeyboardModifiers modifiers, const QString &text, bool autoRepeat,
                         int count, quint32 nativeScanCode, quint32 nativeModifiers, unsigned long time);

    void pluginSettingsRequested(int clientId, const QString &descriptionLanguage);

    /*!
     * \brief Handle global attribute change
     * \param id id of the attribute extension that triggered this change
     * \param targetItem Item name
     * \param attribute Attribute name
     * \param value New attribute value
     */
    void onGlobalAttributeChanged(const MAttributeExtensionId &id,
                                  const QString &targetItem,
                                  const QString &attribute,
                                  const QVariant &value);
private:
    QSet<MAbstractInputMethod *> targets();

protected:
    MIMPluginManagerPrivate *const d_ptr;

private:
    Q_DISABLE_COPY(MIMPluginManager)
    Q_DECLARE_PRIVATE(MIMPluginManager)
    Q_PRIVATE_SLOT(d_func(), void _q_syncHandlerMap(int));
    Q_PRIVATE_SLOT(d_func(), void _q_setActiveSubView(const QString &, Maliit::HandlerState));
    Q_PRIVATE_SLOT(d_func(), void _q_ensureEmptyRegionWhenHidden());
    Q_PRIVATE_SLOT(d_func(), void _q_onScreenSubViewChanged());

    friend class Ut_MIMPluginManager;
    friend class Ft_MIMPluginManager;
    friend class Ut_MIMSettingsDialog;
};

#endif

