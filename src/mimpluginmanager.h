/* * This file is part of Maliit framework *
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 *
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

#include <maliit/namespace.h>

#include <maliit/plugins/plugindescription.h>
#include <maliit/plugins/subviewdescription.h>

#include "minputcontextconnection.h"

QT_BEGIN_NAMESPACE
class QRegion;
QT_END_NAMESPACE
class MIMPluginManagerPrivate;
class MAbstractInputMethod;

namespace Maliit {

class AbstractPlatform;

}

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
    MIMPluginManager(const QSharedPointer<MInputContextConnection> &icConnection,
                     const QSharedPointer<Maliit::AbstractPlatform> &platform);

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

Q_SIGNALS:
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

private Q_SLOTS:
    //! Update and activate input source.
    void updateInputSource();

    void handleAppOrientationChanged(int angle);
    void handleAppOrientationAboutToChange(int angle);
    void handleAppFocusChanged(WId id);

    void handleClientChange();

    void handleWidgetStateChanged(unsigned int clientId, const QMap<QString, QVariant> &newState,
                                  const QMap<QString, QVariant> &oldState, bool focusChanged);
    void handleMouseClickOnPreedit(const QPoint &pos, const QRect &preeditRect);
    void handlePreeditChanged(const QString &text, int cursorPos);

    void processKeyEvent(QEvent::Type keyType, Qt::Key keyCode,
                         Qt::KeyboardModifiers modifiers, const QString &text, bool autoRepeat,
                         int count, quint32 nativeScanCode, quint32 nativeModifiers, unsigned long time);

private:
    QSet<MAbstractInputMethod *> targets();

protected:
    MIMPluginManagerPrivate *const d_ptr;

private:
    Q_DISABLE_COPY(MIMPluginManager)
    Q_DECLARE_PRIVATE(MIMPluginManager)

    Q_PRIVATE_SLOT(d_func(), void _q_syncHandlerMap(int))
    Q_PRIVATE_SLOT(d_func(), void _q_setActiveSubView(const QString &, Maliit::HandlerState))
    Q_PRIVATE_SLOT(d_func(), void _q_onScreenSubViewChanged())

    friend class Ut_MIMPluginManager;
    friend class Ut_MIMPluginManagerConfig;
    friend class Ft_MIMPluginManager;
    friend class Ut_MIMSettingsDialog;
};

#endif

