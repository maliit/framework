/* * This file is part of dui-im-framework *
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

#include "duiinputcontextconnection_stub.h"

#include "duiinputmethodbase.h"


DuiInputContextConnectionStub::DuiInputContextConnectionStub()
{
    // nothing
}


DuiInputContextConnectionStub::~DuiInputContextConnectionStub()
{
}

void DuiInputContextConnectionStub::sendPreeditString(const QString &string,
        PreeditFace preeditFace)
{
    Q_UNUSED(string);
    Q_UNUSED(preeditFace);
}

void DuiInputContextConnectionStub::sendCommitString(const QString &string)
{
    Q_UNUSED(string);
}

void DuiInputContextConnectionStub::sendKeyEvent(const QKeyEvent &keyEvent)
{
    Q_UNUSED(keyEvent);
}

void DuiInputContextConnectionStub::notifyImInitiatedHiding()
{
}

int DuiInputContextConnectionStub::contentType(bool &valid)
{
    valid = false;
    return 0;
}

bool DuiInputContextConnectionStub::correctionEnabled(bool &valid)
{
    valid = false;
    return false;
}

bool DuiInputContextConnectionStub::predictionEnabled(bool &valid)
{
    valid = false;
    return false;
}

bool DuiInputContextConnectionStub::autoCapitalizationEnabled(bool &valid)
{
    valid = false;
    return false;
}

void DuiInputContextConnectionStub::setGlobalCorrectionEnabled(bool)
{
}

bool DuiInputContextConnectionStub::surroundingText(QString &text, int &cursorPosition)
{
    Q_UNUSED(text);
    Q_UNUSED(cursorPosition);

    return false;
}

bool DuiInputContextConnectionStub::hasSelection(bool &valid)
{
    valid = false;
    return false;
}

int DuiInputContextConnectionStub::inputMethodMode(bool &valid)
{
    valid = false;
    return false;
}

QRect DuiInputContextConnectionStub::preeditRectangle(bool &valid)
{
    valid = false;
    return QRect();
}

void DuiInputContextConnectionStub::copy()
{
}

void DuiInputContextConnectionStub::paste()
{
}

void DuiInputContextConnectionStub::setComposingTextInput(bool)
{
}

void DuiInputContextConnectionStub::addRedirectedKey(int keyCode, bool eatInBetweenKey,
        bool eatItself)
{
    Q_UNUSED(keyCode);
    Q_UNUSED(eatInBetweenKey);
    Q_UNUSED(eatItself);
}

void DuiInputContextConnectionStub::removeRedirectedKey(int keyCode)
{
    Q_UNUSED(keyCode);
}

void DuiInputContextConnectionStub::setNextKeyRedirected(bool)
{
}
