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
class MIMPluginManager;
class MIndicatorServiceClient;
class MAbstractInputMethod;

class QRegion;


//! \internal
/*!
 * \brief Interface implementation for connecting input method instances to the environment.
 */
class MInputMethodHost: public MAbstractInputMethodHost
{
    Q_OBJECT

public:
    MInputMethodHost(MInputContextConnection *inputContextConnection,
                     MIMPluginManager *pluginManager, MIndicatorServiceClient &indicatorService,
                     QObject *parent = 0);
    virtual ~MInputMethodHost();

    //! if enabled, the plugin associated with this host are allowed to communicate
    void setEnabled(bool enabled);

    //! associate input method with this host instance.
    //! Multiple calls is (currently) undefined behavior.
    void setInputMethod(MAbstractInputMethod *inputMethod);

    // \reimp
    virtual int contentType(bool &valid);
    virtual bool correctionEnabled(bool &valid);
    virtual bool predictionEnabled(bool &valid);
    virtual bool autoCapitalizationEnabled(bool &valid);
    virtual bool surroundingText(QString &text, int &cursorPosition);
    virtual bool hasSelection(bool &valid);
    virtual int inputMethodMode(bool &valid);
    virtual QRect preeditRectangle(bool &valid);
    virtual QRect cursorRectangle(bool &valid);
    virtual void sendPreeditString(const QString &string,
                                   const QList<MInputMethod::PreeditTextFormat> &preeditFormats,
                                   int cursorPos = -1);
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

    virtual void setInputModeIndicator(MInputMethod::InputModeIndicator mode);

    virtual void switchPlugin(MInputMethod::SwitchDirection direction);
    virtual void switchPlugin(const QString &pluginName);
    virtual void setScreenRegion(const QRegion &region);
    virtual void setInputMethodArea(const QRegion &region);
    virtual void showSettings();
    // \reimp_end


private:
    Q_DISABLE_COPY(MInputMethodHost)

    MInputContextConnection *connection;
    MIMPluginManager *pluginManager;
    MAbstractInputMethod *inputMethod;
    bool enabled;
    MIndicatorServiceClient &indicatorService;
};

//! \internal_end

#endif
