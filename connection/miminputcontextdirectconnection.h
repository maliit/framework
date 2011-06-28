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

#ifndef MIMINPUTCONTEXTDIRECTCONNECTION_H
#define MIMINPUTCONTEXTDIRECTCONNECTION_H

#include "minputcontextconnection.h"

#include <QtCore>

class MImServerConnection;
class MImDirectServerConnection;

class MImInputContextDirectConnection : public MInputContextConnection
{
    Q_OBJECT
public:
    explicit MImInputContextDirectConnection(QObject *parent = 0);

    void connectTo(MImDirectServerConnection *mServerConnection);

    //! reimpl
    virtual void sendPreeditString(const QString &string,
                                   const QList<MInputMethod::PreeditTextFormat> &preeditFormats,
                                   int replacementStart = 0,
                                   int replacementLength = 0,
                                   int cursorPos = -1);
    virtual void sendCommitString(const QString &string, int replaceStart = 0,
                                  int replaceLength = 0, int cursorPos = -1);
    virtual void sendKeyEvent(const QKeyEvent &keyEvent,
                              MInputMethod::EventRequestType requestType
                              = MInputMethod::EventRequestBoth);
    virtual void notifyImInitiatedHiding();
    virtual void copy();
    virtual void paste();
    virtual void setRedirectKeys(bool enabled);
    virtual void setDetectableAutoRepeat(bool enabled);
    virtual void setGlobalCorrectionEnabled(bool enabled);
    virtual void setSelection(int start, int length);
    //! reimpl end

private:
    MImServerConnection *mServerConnection;
};

#endif // MIMINPUTCONTEXTDIRECTCONNECTION_H
