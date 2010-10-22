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


MInputMethodHost::MInputMethodHost(MInputContextConnection *inputContextConnection,
                                   QObject *parent)
    : MAbstractInputMethodHost(parent),
      connection(inputContextConnection)
{
    // nothing
}


MInputMethodHost::~MInputMethodHost()
{
    // nothing
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
    connection->sendPreeditString(string, preeditFace);
}

void MInputMethodHost::sendCommitString(const QString &string)
{
    connection->sendCommitString(string);
}

void MInputMethodHost::sendKeyEvent(const QKeyEvent &keyEvent,
                                    MInputMethod::EventRequestType requestType)
{
    connection->sendKeyEvent(keyEvent, requestType);
}

void MInputMethodHost::notifyImInitiatedHiding()
{
    connection->notifyImInitiatedHiding();
}

void MInputMethodHost::copy()
{
    connection->copy();
}

void MInputMethodHost::paste()
{
    connection->paste();
}

void MInputMethodHost::setRedirectKeys(bool redirectEnabled)
{
    connection->setRedirectKeys(redirectEnabled);
}

void MInputMethodHost::setDetectableAutoRepeat(bool autoRepeatEnabled)
{
    connection->setDetectableAutoRepeat(autoRepeatEnabled);
}

void MInputMethodHost::setGlobalCorrectionEnabled(bool correctionEnabled)
{
    connection->setGlobalCorrectionEnabled(correctionEnabled);
}

void MInputMethodHost::updateInputMethodArea(const QRegion &region)
{
    connection->updateInputMethodArea(region);
}
