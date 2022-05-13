/* 
 * Copyright (c) 2017 Jan Arne Petersen
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 *
 */

#include "standaloneinputmethod.h"

#include "abstractplatform.h"
#include "connectionfactory.h"
#include "minputcontextconnection.h"
#include "standaloneinputmethodhost.h"
#include "windowgroup.h"

#include "maliit/plugins/abstractinputmethod.h"
#include "maliit/plugins/inputmethodplugin.h"

#include <maliit/plugins/updateevent.h>
#include <common/maliit/namespaceinternal.h>
#include <QtGui/QGuiApplication>

namespace Maliit
{

StandaloneInputMethod::StandaloneInputMethod(Maliit::Plugins::InputMethodPlugin *plugin)
    : QObject()
    , mConnection(createConnection())
    , mPlatform(createPlatform().release())
    , mWindowGroup(new WindowGroup(mPlatform))
    , mInputMethodHost(new StandaloneInputMethodHost(mConnection.get(), mWindowGroup.get()))
    , mInputMethod(plugin->createInputMethod(mInputMethodHost.get()))
{
    connect(mConnection.get(), &MInputContextConnection::showInputMethodRequest,
            mWindowGroup.get(), &WindowGroup::activate);
            connect(mConnection.get(), &MInputContextConnection::showInputMethodRequest,
            mInputMethod.get(), &MAbstractInputMethod::show);
    connect(mConnection.get(), &MInputContextConnection::hideInputMethodRequest,
            mInputMethod.get(), &MAbstractInputMethod::hide);
    connect(mConnection.get(), &MInputContextConnection::hideInputMethodRequest,
            mWindowGroup.get(), [this]() { mWindowGroup->deactivate(Maliit::WindowGroup::HideDelayed); } );

    connect(mConnection.get(), &MInputContextConnection::focusChanged,
            mWindowGroup.get(), &WindowGroup::setApplicationWindow);

    connect(mConnection.get(), &MInputContextConnection::resetInputMethodRequest,
            mInputMethod.get(), &MAbstractInputMethod::reset);

    connect(mConnection.get(), &MInputContextConnection::activeClientDisconnected,
            mInputMethod.get(), &MAbstractInputMethod::handleClientChange);
    connect(mConnection.get(), &MInputContextConnection::clientActivated,
            mInputMethod.get(), &MAbstractInputMethod::handleClientChange);

    connect(mConnection.get(), &MInputContextConnection::contentOrientationAboutToChangeCompleted,
            mInputMethod.get(), &MAbstractInputMethod::handleAppOrientationAboutToChange);
    connect(mConnection.get(), &MInputContextConnection::contentOrientationChangeCompleted,
            mInputMethod.get(), &MAbstractInputMethod::handleAppOrientationChanged);

    connect(mConnection.get(), &MInputContextConnection::preeditChanged,
            mInputMethod.get(), &MAbstractInputMethod::setPreedit);
//    connect(mConnection.get(), &MInputContextConnection::mouseClickedOnPreedit,
//            mInputMethod.get(), &MAbstractInputMethod::handleMouseClickOnPreedit);
    connect(mConnection.get(), &MInputContextConnection::receivedKeyEvent,
            mInputMethod.get(), &MAbstractInputMethod::processKeyEvent);

    connect(mConnection.get(), &MInputContextConnection::widgetStateChanged,
            this, &StandaloneInputMethod::handleWidgetStateChanged);
}

StandaloneInputMethod::~StandaloneInputMethod()
{
}

void StandaloneInputMethod::handleWidgetStateChanged(unsigned int,
                                                     const QMap<QString, QVariant> &newState,
                                                     const QMap<QString, QVariant> &oldState,
                                                     bool focusChanged)
{
    static const char * const VisualizationAttribute = "visualizationPriority";
    static const char * const FocusStateAttribute = "focusState";

    // check visualization change
    bool oldVisualization = oldState.value(VisualizationAttribute).toBool();
    bool newVisualization = newState.value(VisualizationAttribute).toBool();

    // update state
    QStringList changedProperties;
    const auto newStateKeys = newState.keys();
    for (auto key: newStateKeys) {
        if (oldState.value(key) != newState.value(key)) {
            changedProperties.append(key);
        }
    }

    const bool widgetFocusState = newState.value(FocusStateAttribute).toBool();

    if (focusChanged) {
        mInputMethod->handleFocusChange(widgetFocusState);
    }

    // call notification methods if needed
    if (oldVisualization != newVisualization) {
        mInputMethod->handleVisualizationPriorityChange(newVisualization);
    }

    const Qt::InputMethodHints lastHints = newState.value(Maliit::Internal::inputMethodHints).value<Qt::InputMethodHints>();
    MImUpdateEvent ev(newState, changedProperties, lastHints);

    // general notification last
    if (!changedProperties.isEmpty()) {
        mInputMethod->imExtensionEvent(&ev);
    }
    mInputMethod->update();

    // Make sure windows get hidden when no longer focus
    if (!widgetFocusState) {
        mInputMethod->hide();
        mWindowGroup->deactivate(Maliit::WindowGroup::HideDelayed);
    }
}

std::unique_ptr<MInputContextConnection> createConnection()
{
#ifdef HAVE_WAYLAND
    auto forceDbus = qgetenv("MALIIT_FORCE_DBUS_CONNECTION");
    if (QGuiApplication::platformName().startsWith("wayland") && (forceDbus.isEmpty() || forceDbus == "0")) {
        return std::unique_ptr<MInputContextConnection>(Maliit::createWestonIMProtocolConnection());
    } else
#endif
        return std::unique_ptr<MInputContextConnection>(Maliit::DBus::createInputContextConnectionWithDynamicAddress());

}

}
