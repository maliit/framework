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

    static MImInputContextDirectConnection *instance();

    void connectTo(MImDirectServerConnection *mServerConnection);

    //! reimpl
    virtual void sendPreeditString(const QString &string,
                                   const QList<Maliit::PreeditTextFormat> &preeditFormats,
                                   int replacementStart = 0,
                                   int replacementLength = 0,
                                   int cursorPos = -1);
    virtual void sendCommitString(const QString &string, int replaceStart = 0,
                                  int replaceLength = 0, int cursorPos = -1);
    virtual void sendKeyEvent(const QKeyEvent &keyEvent,
                              Maliit::EventRequestType requestType
                              = Maliit::EventRequestBoth);
    virtual void notifyImInitiatedHiding();
    virtual void invokeAction(const QString &action, const QKeySequence &sequence);
    virtual void setRedirectKeys(bool enabled);
    virtual void setDetectableAutoRepeat(bool enabled);
    virtual void setGlobalCorrectionEnabled(bool enabled);
    virtual void setSelection(int start, int length);
    virtual void updateInputMethodArea(const QRegion &region);
    virtual void notifyExtendedAttributeChanged(int id,
                                                const QString &target,
                                                const QString &targetItem,
                                                const QString &attribute,
                                                const QVariant &value);
    virtual void notifyExtendedAttributeChanged(const QList<int> &clientIds,
                                                int id,
                                                const QString &target,
                                                const QString &targetItem,
                                                const QString &attribute,
                                                const QVariant &value);
    virtual void pluginSettingsLoaded(int clientId, const QList<MImPluginSettingsInfo> &info);
    //! reimpl end

private:
    MImServerConnection *mServerConnection;
};

#endif // MIMINPUTCONTEXTDIRECTCONNECTION_H
