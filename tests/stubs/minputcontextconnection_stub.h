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

#ifndef MINPUTCONTEXTCONNECTIONSTUB_H
#define MINPUTCONTEXTCONNECTIONSTUB_H

#include "mpreeditface.h"
#include "minputcontextconnection.h"
#include <QRect>
#include <QObject>

class QString;
class QRegion;
class QKeyEvent;

class MInputContextConnectionStub: public MInputContextConnection
{
    Q_OBJECT

public:
    MInputContextConnectionStub();
    virtual ~MInputContextConnectionStub();

    virtual void sendPreeditString(const QString &string,
                                   PreeditFace preeditFace = PreeditDefault);

    virtual void sendCommitString(const QString &string);

    virtual void sendKeyEvent(const QKeyEvent &keyEvent, bool signalOnly = false);

    virtual void notifyImInitiatedHiding();

    virtual int contentType(bool &valid);

    virtual bool correctionEnabled(bool &valid);

    virtual bool predictionEnabled(bool &valid);

    virtual bool autoCapitalizationEnabled(bool &valid);

    virtual void setGlobalCorrectionEnabled(bool);

    virtual bool surroundingText(QString &text, int &cursorPosition);

    virtual bool hasSelection(bool &valid);

    virtual int winId(bool &valid);

    virtual int inputMethodMode(bool &valid);

    virtual QRect preeditRectangle(bool &valid);

    virtual void copy();

    virtual void paste();

    virtual void setRedirectKeys(bool enabled);

    virtual void setDetectableAutoRepeat(bool enabled);
};


#endif
