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

#include "standaloneinputmethodhost.h"

#include "minputcontextconnection.h"
#include "maliit/plugins/plugindescription.h"
#include "windowgroup.h"

namespace Maliit
{

StandaloneInputMethodHost::StandaloneInputMethodHost(MInputContextConnection *connection, WindowGroup *windowGroup)
    : MAbstractInputMethodHost()
    , mConnection(connection)
    , mWindowGroup(windowGroup)
{
}

bool StandaloneInputMethodHost::hiddenText(bool &valid)
{
    return mConnection->hiddenText(valid);
}

void StandaloneInputMethodHost::setLanguage(const QString &language)
{
    if (enabled)
        mConnection->setLanguage(language);
}

int StandaloneInputMethodHost::contentType(bool &valid)
{
    return mConnection->contentType(valid);
}

bool StandaloneInputMethodHost::correctionEnabled(bool &valid)
{
    return mConnection->correctionEnabled(valid);
}

bool StandaloneInputMethodHost::predictionEnabled(bool &valid)
{
    return mConnection->predictionEnabled(valid);
}

bool StandaloneInputMethodHost::autoCapitalizationEnabled(bool &valid)
{
    return mConnection->autoCapitalizationEnabled(valid);
}

bool StandaloneInputMethodHost::surroundingText(QString &text, int &cursorPosition)
{
    return mConnection->surroundingText(text, cursorPosition);
}

bool StandaloneInputMethodHost::hasSelection(bool &valid)
{
    return mConnection->hasSelection(valid);
}

int StandaloneInputMethodHost::inputMethodMode(bool &valid)
{
    return mConnection->inputMethodMode(valid);
}

QRect StandaloneInputMethodHost::preeditRectangle(bool &valid)
{
    return mConnection->preeditRectangle(valid);
}

QRect StandaloneInputMethodHost::cursorRectangle(bool &valid)
{
    return mConnection->cursorRectangle(valid);
}

int StandaloneInputMethodHost::anchorPosition(bool &valid)
{
    return mConnection->anchorPosition(valid);
}

QString StandaloneInputMethodHost::selection(bool &valid)
{
    return mConnection->selection(valid);
}

void StandaloneInputMethodHost::registerWindow(QWindow *window, Maliit::Position position)
{
    mWindowGroup->setupWindow(window, position);
}

void StandaloneInputMethodHost::sendPreeditString(const QString &string,
                                                  const QList<Maliit::PreeditTextFormat> &preeditFormats,
                                                  int replacementStart,
                                                  int replacementLength,
                                                  int cursorPos)
{
    if (enabled)
        mConnection->sendPreeditString(string, preeditFormats, replacementStart, replacementLength, cursorPos);
}

void
StandaloneInputMethodHost::sendCommitString(const QString &string, int replaceStart, int replaceLength, int cursorPos)
{
    if (enabled)
        mConnection->sendCommitString(string, replaceStart, replaceLength, cursorPos);
}

void StandaloneInputMethodHost::sendKeyEvent(const QKeyEvent &keyEvent, Maliit::EventRequestType requestType)
{
    if (enabled)
        mConnection->sendKeyEvent(keyEvent, requestType);
}

void StandaloneInputMethodHost::notifyImInitiatedHiding()
{
    if (enabled)
        mConnection->notifyImInitiatedHiding();
}

void StandaloneInputMethodHost::invokeAction(const QString &action, const QKeySequence &sequence)
{
    if (enabled)
        mConnection->invokeAction(action, sequence);
}

void StandaloneInputMethodHost::setRedirectKeys(bool enabled)
{
    if (enabled)
        mConnection->setRedirectKeys(enabled);
}

void StandaloneInputMethodHost::setDetectableAutoRepeat(bool enabled)
{
    if (enabled)
        mConnection->setDetectableAutoRepeat(enabled);
}

void StandaloneInputMethodHost::setGlobalCorrectionEnabled(bool enabled)
{
    if (enabled)
        mConnection->setGlobalCorrectionEnabled(enabled);
}

void StandaloneInputMethodHost::switchPlugin(Maliit::SwitchDirection direction)
{
}

void StandaloneInputMethodHost::switchPlugin(const QString &pluginName)
{
}

void StandaloneInputMethodHost::setScreenRegion(const QRegion &region, QWindow *window)
{
    mWindowGroup->setScreenRegion(region, window);
}

void StandaloneInputMethodHost::setInputMethodArea(const QRegion &region, QWindow *window)
{
    mWindowGroup->setScreenRegion(region, window);
}

void StandaloneInputMethodHost::setSelection(int start, int length)
{
    if (enabled)
        mConnection->setSelection(start, length);
}

void StandaloneInputMethodHost::setOrientationAngleLocked(bool lock)
{
}

QList<MImPluginDescription> StandaloneInputMethodHost::pluginDescriptions(Maliit::HandlerState state) const
{
    return QList<MImPluginDescription>();
}

Maliit::Plugins::AbstractPluginSetting *StandaloneInputMethodHost::registerPluginSetting(const QString &key,
                                                                                         const QString &description,
                                                                                         Maliit::SettingEntryType type,
                                                                                         const QVariantMap &attributes)
{
    return nullptr;
}

}