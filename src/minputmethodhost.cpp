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
#include "mindicatorserviceclient.h"
#include "mabstractinputmethod.h"


MInputMethodHost::MInputMethodHost(MInputContextConnection *inputContextConnection,
                                   MIMPluginManager *pluginManager,
                                   MIndicatorServiceClient &indicatorService, QObject *parent)
    : MAbstractInputMethodHost(parent),
      connection(inputContextConnection),
      pluginManager(pluginManager),
      inputMethod(0),
      enabled(false),
      indicatorService(indicatorService)
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

void MInputMethodHost::setInputMethod(MAbstractInputMethod *inputMethod)
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

QRect MInputMethodHost::cursorRectangle(bool &valid)
{
    return connection->cursorRectangle(valid);
}

void MInputMethodHost::sendPreeditString(const QString &string,
                                         const QList<MInputMethod::PreeditTextFormat> &preeditFormats,
                                         int replacementStart, int replacementLength,
                                         int cursorPos)
{
    if (enabled) {
        connection->sendPreeditString(string, preeditFormats, replacementStart, replacementLength, cursorPos);
    }
}

void MInputMethodHost::sendCommitString(const QString &string, int replaceStart,
                                        int replaceLength, int cursorPos)
{
    if (enabled) {
        connection->sendCommitString(string, replaceStart, replaceLength, cursorPos);
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

void MInputMethodHost::setInputModeIndicator(MInputMethod::InputModeIndicator mode)
{
    if (enabled) {
        indicatorService.setInputModeIndicator(mode);
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

void MInputMethodHost::setScreenRegion(const QRegion &region)
{
    if (enabled) {
        pluginManager->updateRegion(region);
    }
}

void MInputMethodHost::setInputMethodArea(const QRegion &region)
{
    if (enabled) {
        connection->updateInputMethodArea(region);
    }
}

void MInputMethodHost::showSettings()
{
    if (enabled) {
        pluginManager->showInputMethodSettings();
    }
}

void MInputMethodHost::setSelection(int start, int length)
{
    if (enabled) {
        connection->setSelection(start, length);
    }
}

void MInputMethodHost::setOrientationAngleLocked(bool lock)
{
    if (enabled) {
        connection->setOrientationAngleLocked(lock);
    }
}

int MInputMethodHost::anchorPosition(bool &valid)
{
    return connection->anchorPosition(valid);
}

