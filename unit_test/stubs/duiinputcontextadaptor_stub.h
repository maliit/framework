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

#ifndef DUIINPUTCONTEXTADAPTOR_STUB_H
#define DUIINPUTCONTEXTADAPTOR_STUB_H

#include "duiinputcontext.h"
#include "duiinputcontextadaptor.h"
#include "stubbase.h"

#include <QDebug>


/**
 * DuiInputContextAdaptor stub class.
 * To fake DuiInputContextAdaptor operations, derive from this class
 * and implement the methods you want to fake. Instantiate your
 * derived stub class and assign it to gDuiInputContextAdaptorStub
 * global variable.
 */
class DuiInputContextAdaptorStub : public StubBase
{
public:
    DuiInputContextAdaptorStub();
    virtual ~DuiInputContextAdaptorStub();

    virtual void constructor(DuiInputContext *inputContext);
    virtual void destructor();

    virtual void imInitiatedHide();
    virtual void commitString(const QString &string);
    virtual void updatePreedit(const QString &string, int preeditFace);
    virtual void keyEvent(int type, int key, int modifiers, const QString &text,
                          bool autoRepeat, int count);
    virtual void updateInputMethodArea(const QList<QVariant> &data);
    virtual void setGlobalCorrectionEnabled(bool);
    virtual void setRedirectKeys(bool enabled);

    // valid is out parameter for value validity
    virtual int contentType(bool &valid);
    virtual bool correctionEnabled(bool &valid);
    virtual bool predictionEnabled(bool &valid);
    virtual bool autoCapitalizationEnabled(bool &valid);
    virtual bool surroundingText(QString &text, int &cursorPosition);
    virtual bool hasSelection(bool &valid);
    virtual int inputMethodMode(bool &valid);
    virtual QRect preeditRectangle(bool &valid);
};

DuiInputContextAdaptorStub::DuiInputContextAdaptorStub()
{
}

DuiInputContextAdaptorStub::~DuiInputContextAdaptorStub()
{
}

void DuiInputContextAdaptorStub::constructor(DuiInputContext *)
{
}

void DuiInputContextAdaptorStub::destructor()
{
}

void DuiInputContextAdaptorStub::imInitiatedHide()
{
    stubMethodEntered("imInitiatedHide");
}

void DuiInputContextAdaptorStub::commitString(const QString &string)
{
    QList<ParameterBase *> params;
    params.append(new Parameter<QString>(string));
    stubMethodEntered("commitString", params);
}

void DuiInputContextAdaptorStub::updatePreedit(const QString &string, int preeditFace)
{
    QList<ParameterBase *> params;
    params.append(new Parameter<QString>(string));
    params.append(new Parameter<int>(preeditFace));
    stubMethodEntered("updatePreedit", params);
}

void DuiInputContextAdaptorStub::keyEvent(int type, int key, int modifiers,
        const QString &text, bool autoRepeat, int count)
{
    QList<ParameterBase *> params;
    params.append(new Parameter<int>(type));
    params.append(new Parameter<int>(key));
    params.append(new Parameter<int>(modifiers));
    params.append(new Parameter<QString>(text));
    params.append(new Parameter<bool>(autoRepeat));
    params.append(new Parameter<int>(count));
    stubMethodEntered("keyEvent", params);
}

void DuiInputContextAdaptorStub::updateInputMethodArea(const QList<QVariant> &data)
{
    QList<ParameterBase *> params;
    params.append(new Parameter< QList<QVariant> >(data));
    stubMethodEntered("updateInputMethodArea", params);
}

void DuiInputContextAdaptorStub::setGlobalCorrectionEnabled(bool enable)
{
    QList<ParameterBase *> params;
    params.append(new Parameter<bool>(enable));
    stubMethodEntered("setGlobalCorrectionEnabled", params);
}


void DuiInputContextAdaptorStub::setRedirectKeys(bool enabled)
{
    QList<ParameterBase *> params;
    params.append(new Parameter<bool>(enabled));
    stubMethodEntered("setRedirectKeys", params);
}

int DuiInputContextAdaptorStub::contentType(bool &valid)
{
    QList<ParameterBase *> params;
    params.append(new Parameter<bool>(valid));
    stubMethodEntered("contentType", params);
    return stubReturnValue<int>("contentType");
}

bool DuiInputContextAdaptorStub::correctionEnabled(bool &valid)
{
    QList<ParameterBase *> params;
    params.append(new Parameter<bool>(valid));
    stubMethodEntered("correctionEnabled", params);
    return stubReturnValue<bool>("correctionEnabled");
}

bool DuiInputContextAdaptorStub::predictionEnabled(bool &valid)
{
    QList<ParameterBase *> params;
    params.append(new Parameter<bool>(valid));
    stubMethodEntered("predictionEnabled", params);
    return stubReturnValue<bool>("predictionEnabled");
}

bool DuiInputContextAdaptorStub::autoCapitalizationEnabled(bool &valid)
{
    QList<ParameterBase *> params;
    params.append(new Parameter<bool>(valid));
    stubMethodEntered("autoCapitalizationEnabled", params);
    return stubReturnValue<bool>("autoCapitalizationEnabled");
}

bool DuiInputContextAdaptorStub::surroundingText(QString &text, int &cursorPosition)
{
    QList<ParameterBase *> params;
    params.append(new Parameter<QString>(text));
    params.append(new Parameter<int>(cursorPosition));
    stubMethodEntered("surroundingText", params);
    return stubReturnValue<bool>("surroundingText");
}

bool DuiInputContextAdaptorStub::hasSelection(bool &valid)
{
    QList<ParameterBase *> params;
    params.append(new Parameter<bool>(valid));
    stubMethodEntered("hasSelection", params);
    return stubReturnValue<bool>("haselection");
}

int DuiInputContextAdaptorStub::inputMethodMode(bool &valid)
{
    QList<ParameterBase *> params;
    params.append(new Parameter<bool>(valid));
    stubMethodEntered("inputMethodMode", params);
    return stubReturnValue<int>("inputMethodMode");
}

QRect DuiInputContextAdaptorStub::preeditRectangle(bool &valid)
{
    QList<ParameterBase *> params;
    params.append(new Parameter<bool>(valid));
    stubMethodEntered("preeditRectangle", params);
    return stubReturnValue<QRect>("preeditRectangle");
}


DuiInputContextAdaptorStub gDefaultDuiInputContextAdaptorStub;

/**
 * This is the stub class instance used by the system. If you want to alter behaviour,
 * derive your stub class from DuiInputContextAdaptorStub, implement the methods you want to
 * fake, create an instance of your stub class and assign the instance into this global variable.
 */
DuiInputContextAdaptorStub *gDuiInputContextAdaptorStub = &gDefaultDuiInputContextAdaptorStub;

/**
 * These are the proxy method implementations of DuiInputContextAdaptor. They will
 * call the stub object methods of the gDuiInputContextAdaptorStub.
 */

DuiInputContextAdaptor::DuiInputContextAdaptor(DuiInputContext *inputContext)
    : QDBusAbstractAdaptor(inputContext)
{
    gDuiInputContextAdaptorStub->constructor(inputContext);
}

DuiInputContextAdaptor::~DuiInputContextAdaptor()
{
    gDuiInputContextAdaptorStub->destructor();
}

void DuiInputContextAdaptor::imInitiatedHide()
{
    gDuiInputContextAdaptorStub->imInitiatedHide();
}


void DuiInputContextAdaptor::commitString(const QString &string)
{
    gDuiInputContextAdaptorStub->commitString(string);
}

void DuiInputContextAdaptor::updatePreedit(const QString &string, int preeditFace)
{
    gDuiInputContextAdaptorStub->updatePreedit(string, preeditFace);
}

void DuiInputContextAdaptor::keyEvent(int type, int key, int modifiers, const QString &text,
                                      bool autoRepeat, int count)
{
    gDuiInputContextAdaptorStub->keyEvent(type, key, modifiers, text, autoRepeat, count);
}

void DuiInputContextAdaptor::updateInputMethodArea(const QList<QVariant> &data)
{
    gDuiInputContextAdaptorStub->updateInputMethodArea(data);
}

void DuiInputContextAdaptor::setGlobalCorrectionEnabled(bool enable)
{
    gDuiInputContextAdaptorStub->setGlobalCorrectionEnabled(enable);
}

void DuiInputContextAdaptor::setRedirectKeys(bool enabled)
{
    return gDuiInputContextAdaptorStub->setRedirectKeys(enabled);
}

int DuiInputContextAdaptor::contentType(bool &valid)
{
    return gDuiInputContextAdaptorStub->contentType(valid);
}

bool DuiInputContextAdaptor::correctionEnabled(bool &valid)
{
    return gDuiInputContextAdaptorStub->correctionEnabled(valid);
}
bool DuiInputContextAdaptor::predictionEnabled(bool &valid)
{
    return gDuiInputContextAdaptorStub->predictionEnabled(valid);
}

bool DuiInputContextAdaptor::autoCapitalizationEnabled(bool &valid)
{
    return gDuiInputContextAdaptorStub->autoCapitalizationEnabled(valid);
}

bool DuiInputContextAdaptor::surroundingText(QString &text, int &cursorPosition)
{
    return gDuiInputContextAdaptorStub->surroundingText(text, cursorPosition);
}

bool DuiInputContextAdaptor::hasSelection(bool &valid)
{
    return gDuiInputContextAdaptorStub->hasSelection(valid);
}

int DuiInputContextAdaptor::inputMethodMode(bool &valid)
{
    return gDuiInputContextAdaptorStub->inputMethodMode(valid);
}

QRect DuiInputContextAdaptor::preeditRectangle(bool &valid)
{
    return gDuiInputContextAdaptorStub->preeditRectangle(valid);
}

#endif
