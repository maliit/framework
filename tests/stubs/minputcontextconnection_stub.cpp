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

#include "minputcontextconnection_stub.h"

#include "minputmethodbase.h"


MInputContextConnectionStub::MInputContextConnectionStub()
{
    // nothing
}


MInputContextConnectionStub::~MInputContextConnectionStub()
{
}

void MInputContextConnectionStub::sendPreeditString(const QString &string,
                                                    MInputMethod::PreeditFace preeditFace)
{
    Q_UNUSED(string);
    Q_UNUSED(preeditFace);
}

void MInputContextConnectionStub::sendCommitString(const QString &string)
{
    Q_UNUSED(string);
}

void MInputContextConnectionStub::sendKeyEvent(const QKeyEvent &keyEvent,
                                               MInputMethod::EventRequestType requestType)
{
    Q_UNUSED(keyEvent);
    Q_UNUSED(requestType);
}

void MInputContextConnectionStub::notifyImInitiatedHiding()
{
}

int MInputContextConnectionStub::contentType(bool &valid)
{
    valid = false;
    return 0;
}

bool MInputContextConnectionStub::correctionEnabled(bool &valid)
{
    valid = false;
    return false;
}

bool MInputContextConnectionStub::predictionEnabled(bool &valid)
{
    valid = false;
    return false;
}

bool MInputContextConnectionStub::autoCapitalizationEnabled(bool &valid)
{
    valid = false;
    return false;
}

void MInputContextConnectionStub::setGlobalCorrectionEnabled(bool)
{
}

bool MInputContextConnectionStub::surroundingText(QString &text, int &cursorPosition)
{
    Q_UNUSED(text);
    Q_UNUSED(cursorPosition);

    return false;
}

bool MInputContextConnectionStub::hasSelection(bool &valid)
{
    valid = false;
    return false;
}

int MInputContextConnectionStub::winId(bool &valid)
{
    valid = false;
    return 0;
}

int MInputContextConnectionStub::inputMethodMode(bool &valid)
{
    valid = false;
    return false;
}

QRect MInputContextConnectionStub::preeditRectangle(bool &valid)
{
    valid = false;
    return QRect();
}

void MInputContextConnectionStub::copy()
{
}

void MInputContextConnectionStub::paste()
{
}

void MInputContextConnectionStub::setRedirectKeys(bool /* enabled */)
{
}

void MInputContextConnectionStub::setDetectableAutoRepeat(bool /* enabled */)
{
}
