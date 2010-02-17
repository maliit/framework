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

#ifndef DUIINPUTCONTEXTCONNECTIONSTUB_H
#define DUIINPUTCONTEXTCONNECTIONSTUB_H

#include "duipreeditface.h"
#include "duiinputcontextconnection.h"
#include <QRect>
#include <QObject>

class QString;
class QRegion;
class QKeyEvent;

class DuiInputContextConnectionStub: public DuiInputContextConnection
{
    Q_OBJECT

public:
    DuiInputContextConnectionStub();
    virtual ~DuiInputContextConnectionStub();

    virtual void sendPreeditString(const QString &string,
                                   PreeditFace preeditFace = PreeditDefault);

    virtual void sendCommitString(const QString &string);

    virtual void sendKeyEvent(const QKeyEvent &keyEvent);

    virtual void notifyImInitiatedHiding();

    virtual int contentType(bool &valid);

    virtual bool correctionEnabled(bool &valid);

    virtual bool predictionEnabled(bool &valid);

    virtual bool autoCapitalizationEnabled(bool &valid);

    virtual void setGlobalCorrectionEnabled(bool);

    virtual bool surroundingText(QString &text, int &cursorPosition);

    virtual bool hasSelection(bool &valid);

    virtual int inputMethodMode(bool &valid);

    virtual QRect preeditRectangle(bool &valid);

    virtual void copy();

    virtual void paste();

    virtual void setComposingTextInput(bool);

    virtual void addRedirectedKey(int keyCode, bool eatInBetweenKey, bool eatItself);

    virtual void removeRedirectedKey(int keyCode);

    virtual void setNextKeyRedirected(bool);
};


#endif
