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

//    connect(mConnection.get(), &MInputContextConnection::widgetStateChanged,
//            this, &StandaloneInputMethod::handleWidgetStateChanged);

//    connect(d->mICConnection.data(), SIGNAL(pluginSettingsRequested(int,QString)),
//            this, SLOT(pluginSettingsRequested(int,QString)));

}

StandaloneInputMethod::~StandaloneInputMethod()
{
}

std::unique_ptr<MInputContextConnection> createConnection()
{
    return std::unique_ptr<MInputContextConnection>(Maliit::DBus::createInputContextConnectionWithDynamicAddress());
}


}