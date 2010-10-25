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

#include "minputmethodhost.h"
#include "minputcontextconnection.h"
#include "mimpluginmanager.h"


MInputMethodHost::MInputMethodHost(MInputContextConnection *inputContextConnection,
                                   MIMPluginManager *pluginManager, QObject *parent)
    : MAbstractInputMethodHost(parent),
      connection(inputContextConnection),
      pluginManager(pluginManager),
      inputMethod(0),
      enabled(false)
{
    // nothing
}


MInputMethodHost::~MInputMethodHost()
{
    // nothing
}


void MInputMethodHost::setEnabled(bool enabled)
{
    this->enabled = enabled;
}

void MInputMethodHost::setInputMethod(MInputMethodBase *inputMethod)
{
    this->inputMethod = inputMethod;
}


int MInputMethodHost::contentType(bool &valid)
{
    return connection->contentType(valid);
}

bool MInputMethodHost::correctionEnabled(bool &valid)
{
    return connection->correctionEnabled(valid);
}

bool MInputMethodHost::predictionEnabled(bool &valid)
{
    return connection->predictionEnabled(valid);
}

bool MInputMethodHost::autoCapitalizationEnabled(bool &valid)
{
    return connection->autoCapitalizationEnabled(valid);
}

bool MInputMethodHost::surroundingText(QString &text, int &cursorPosition)
{
    return connection->surroundingText(text, cursorPosition);
}

bool MInputMethodHost::hasSelection(bool &valid)
{
    return connection->hasSelection(valid);
}

int MInputMethodHost::inputMethodMode(bool &valid)
{
    return connection->inputMethodMode(valid);
}

QRect MInputMethodHost::preeditRectangle(bool &valid)
{
    return connection->preeditRectangle(valid);
}

void MInputMethodHost::sendPreeditString(const QString &string,
                                         MInputMethod::PreeditFace preeditFace)
{
    if (enabled) {
        connection->sendPreeditString(string, preeditFace);
    }
}

void MInputMethodHost::sendCommitString(const QString &string)
{
    if (enabled) {
        connection->sendCommitString(string);
    }
}

void MInputMethodHost::sendKeyEvent(const QKeyEvent &keyEvent,
                                    MInputMethod::EventRequestType requestType)
{
    if (enabled) {
        connection->sendKeyEvent(keyEvent, requestType);
    }
}

void MInputMethodHost::notifyImInitiatedHiding()
{
    if (enabled) {
        connection->notifyImInitiatedHiding();
    }
}

void MInputMethodHost::copy()
{
    if (enabled) {
        connection->copy();
    }
}

void MInputMethodHost::paste()
{
    if (enabled) {        
        connection->paste();
    }
}

void MInputMethodHost::setRedirectKeys(bool redirectEnabled)
{
    if (enabled) {
        connection->setRedirectKeys(redirectEnabled);
    }
}

void MInputMethodHost::setDetectableAutoRepeat(bool autoRepeatEnabled)
{
    if (enabled) {
        connection->setDetectableAutoRepeat(autoRepeatEnabled);
    }
}

void MInputMethodHost::setGlobalCorrectionEnabled(bool correctionEnabled)
{
    if (enabled) {
        connection->setGlobalCorrectionEnabled(correctionEnabled);
    }
}

void MInputMethodHost::switchPlugin(MInputMethod::SwitchDirection direction)
{    
    if (enabled) {
        pluginManager->switchPlugin(direction, inputMethod);
    }
}

void MInputMethodHost::switchPlugin(const QString &pluginName)
{
    if (enabled) {
        pluginManager->switchPlugin(pluginName, inputMethod);
    }
}

void MInputMethodHost::showSettings()
{
    if (enabled) {
        pluginManager->showInputMethodSettings();
    }
}
