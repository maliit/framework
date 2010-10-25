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

#ifndef MINPUTMETHODHOST_H
#define MINPUTMETHODHOST_H

#include "mabstractinputmethodhost.h"

class MInputContextConnection;

/*!
 * \brief Interface implementation for connecting input method instances to the environment.
 */
class MInputMethodHost: public MAbstractInputMethodHost
{
    Q_OBJECT

public:
    explicit MInputMethodHost(MInputContextConnection *inputContextConnection, QObject *parent = 0);
    virtual ~MInputMethodHost();

    //! if enabled, the plugin(s) associated with this host are allowed to communicate
    void setEnabled(bool enabled);

    // \reimp
    virtual int contentType(bool &valid);
    virtual bool correctionEnabled(bool &valid);
    virtual bool predictionEnabled(bool &valid);
    virtual bool autoCapitalizationEnabled(bool &valid);
    virtual bool surroundingText(QString &text, int &cursorPosition);
    virtual bool hasSelection(bool &valid);
    virtual int inputMethodMode(bool &valid);
    virtual QRect preeditRectangle(bool &valid);
    virtual void sendPreeditString(const QString &string,
                                   MInputMethod::PreeditFace preeditFace
                                    = MInputMethod::PreeditDefault);
    virtual void sendCommitString(const QString &string);
    virtual void sendKeyEvent(const QKeyEvent &keyEvent,
                              MInputMethod::EventRequestType requestType
                               = MInputMethod::EventRequestBoth);
    virtual void notifyImInitiatedHiding();
    virtual void copy();
    virtual void paste();
    virtual void setRedirectKeys(bool enabled);
    virtual void setDetectableAutoRepeat(bool enabled);
    virtual void setGlobalCorrectionEnabled(bool enabled);
    // \reimp_end


private:
    Q_DISABLE_COPY(MInputMethodHost)

    MInputContextConnection *connection;
    bool enabled;
};


#endif
