/* * This file is part of Maliit framework *
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 *
 * Contact: maliit-discuss@lists.maliit.org
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
#include <maliit/plugins/abstractinputmethod.h>
#include "windowgroup.h"

#include <maliit/namespace.h>

MInputMethodHost::MInputMethodHost(const QSharedPointer<MInputContextConnection> &inputContextConnection,
                                   MIMPluginManager *pluginManager,
                                   const QSharedPointer<Maliit::WindowGroup> &windowGroup,
                                   const QString &plugin,
                                   const QString &description)
    : MAbstractInputMethodHost(),
      connection(inputContextConnection),
      pluginManager(pluginManager),
      inputMethod(0),
      enabled(false),
      pluginId(plugin),
      pluginDescription(description),
      mWindowGroup(windowGroup)
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

QString MInputMethodHost::selection(bool &valid)
{
    return connection->selection(valid);
}

void MInputMethodHost::registerWindow (QWindow *window,
                                       Maliit::Position position)
{
    mWindowGroup->setupWindow(window, position);
}

int MInputMethodHost::preeditClickPos(bool &valid) const
{
    return connection->preeditClickPos(valid);
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

bool MInputMethodHost::hiddenText(bool &valid)
{
    return connection->hiddenText(valid);
}

void MInputMethodHost::sendPreeditString(const QString &string,
                                         const QList<Maliit::PreeditTextFormat> &preeditFormats,
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
                                    Maliit::EventRequestType requestType)
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

void MInputMethodHost::invokeAction(const QString &action,
                                  const QKeySequence &sequence)
{
    if (enabled) {
        connection->invokeAction(action, sequence);
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

void MInputMethodHost::switchPlugin(Maliit::SwitchDirection direction)
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

void MInputMethodHost::setScreenRegion(const QRegion &region, QWindow *window)
{
    mWindowGroup->setScreenRegion(region, window);
}

void MInputMethodHost::setInputMethodArea(const QRegion &region, QWindow *window)
{
    mWindowGroup->setInputMethodArea(region, window);
}

void MInputMethodHost::setSelection(int start, int length)
{
    if (enabled) {
        connection->setSelection(start, length);
    }
}

QList<MImPluginDescription> MInputMethodHost::pluginDescriptions(Maliit::HandlerState state) const
{
    return pluginManager->pluginDescriptions(state);
}

QList<MImSubViewDescription>
MInputMethodHost::surroundingSubViewDescriptions(Maliit::HandlerState state) const
{
    return pluginManager->surroundingSubViewDescriptions(state);
}

void MInputMethodHost::setLanguage(const QString &language)
{
    if (enabled) {
        connection->setLanguage(language);
    }
}

void MInputMethodHost::setOrientationAngleLocked(bool)
{
    // NOT implemented.
}

int MInputMethodHost::anchorPosition(bool &valid)
{
    return connection->anchorPosition(valid);
}

AbstractPluginSetting *MInputMethodHost::registerPluginSetting(const QString &key,
                                                               const QString &description,
                                                               Maliit::SettingEntryType type,
                                                               const QVariantMap &attributes)
{
    return pluginManager->registerPluginSetting(pluginId, pluginDescription, key, description, type, attributes);
}
