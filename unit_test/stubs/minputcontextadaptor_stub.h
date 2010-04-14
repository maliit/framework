/* * This file is part of m-im-framework *
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

#ifndef MINPUTCONTEXTADAPTOR_STUB_H
#define MINPUTCONTEXTADAPTOR_STUB_H

#include "minputcontext.h"
#include "minputcontextadaptor.h"
#include "stubbase.h"

#include <QDebug>


/**
 * MInputContextAdaptor stub class.
 * To fake MInputContextAdaptor operations, derive from this class
 * and implement the methods you want to fake. Instantiate your
 * derived stub class and assign it to gMInputContextAdaptorStub
 * global variable.
 */
class MInputContextAdaptorStub : public StubBase
{
public:
    MInputContextAdaptorStub();
    virtual ~MInputContextAdaptorStub();

    virtual void constructor(MInputContext *inputContext);
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
    virtual QRect preeditRectangle(bool &valid);
};

MInputContextAdaptorStub::MInputContextAdaptorStub()
{
}

MInputContextAdaptorStub::~MInputContextAdaptorStub()
{
}

void MInputContextAdaptorStub::constructor(MInputContext *)
{
}

void MInputContextAdaptorStub::destructor()
{
}

void MInputContextAdaptorStub::imInitiatedHide()
{
    stubMethodEntered("imInitiatedHide");
}

void MInputContextAdaptorStub::commitString(const QString &string)
{
    QList<ParameterBase *> params;
    params.append(new Parameter<QString>(string));
    stubMethodEntered("commitString", params);
}

void MInputContextAdaptorStub::updatePreedit(const QString &string, int preeditFace)
{
    QList<ParameterBase *> params;
    params.append(new Parameter<QString>(string));
    params.append(new Parameter<int>(preeditFace));
    stubMethodEntered("updatePreedit", params);
}

void MInputContextAdaptorStub::keyEvent(int type, int key, int modifiers,
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

void MInputContextAdaptorStub::updateInputMethodArea(const QList<QVariant> &data)
{
    QList<ParameterBase *> params;
    params.append(new Parameter< QList<QVariant> >(data));
    stubMethodEntered("updateInputMethodArea", params);
}

void MInputContextAdaptorStub::setGlobalCorrectionEnabled(bool enable)
{
    QList<ParameterBase *> params;
    params.append(new Parameter<bool>(enable));
    stubMethodEntered("setGlobalCorrectionEnabled", params);
}


void MInputContextAdaptorStub::setRedirectKeys(bool enabled)
{
    QList<ParameterBase *> params;
    params.append(new Parameter<bool>(enabled));
    stubMethodEntered("setRedirectKeys", params);
}

QRect MInputContextAdaptorStub::preeditRectangle(bool &valid)
{
    QList<ParameterBase *> params;
    params.append(new Parameter<bool>(valid));
    stubMethodEntered("preeditRectangle", params);
    return stubReturnValue<QRect>("preeditRectangle");
}


MInputContextAdaptorStub gDefaultMInputContextAdaptorStub;

/**
 * This is the stub class instance used by the system. If you want to alter behaviour,
 * derive your stub class from MInputContextAdaptorStub, implement the methods you want to
 * fake, create an instance of your stub class and assign the instance into this global variable.
 */
MInputContextAdaptorStub *gMInputContextAdaptorStub = &gDefaultMInputContextAdaptorStub;

/**
 * These are the proxy method implementations of MInputContextAdaptor. They will
 * call the stub object methods of the gMInputContextAdaptorStub.
 */

MInputContextAdaptor::MInputContextAdaptor(MInputContext *inputContext)
    : QDBusAbstractAdaptor(inputContext)
{
    gMInputContextAdaptorStub->constructor(inputContext);
}

MInputContextAdaptor::~MInputContextAdaptor()
{
    gMInputContextAdaptorStub->destructor();
}

void MInputContextAdaptor::imInitiatedHide()
{
    gMInputContextAdaptorStub->imInitiatedHide();
}


void MInputContextAdaptor::commitString(const QString &string)
{
    gMInputContextAdaptorStub->commitString(string);
}

void MInputContextAdaptor::updatePreedit(const QString &string, int preeditFace)
{
    gMInputContextAdaptorStub->updatePreedit(string, preeditFace);
}

void MInputContextAdaptor::keyEvent(int type, int key, int modifiers, const QString &text,
                                      bool autoRepeat, int count)
{
    gMInputContextAdaptorStub->keyEvent(type, key, modifiers, text, autoRepeat, count);
}

void MInputContextAdaptor::updateInputMethodArea(const QList<QVariant> &data)
{
    gMInputContextAdaptorStub->updateInputMethodArea(data);
}

void MInputContextAdaptor::setGlobalCorrectionEnabled(bool enable)
{
    gMInputContextAdaptorStub->setGlobalCorrectionEnabled(enable);
}

void MInputContextAdaptor::setRedirectKeys(bool enabled)
{
    return gMInputContextAdaptorStub->setRedirectKeys(enabled);
}

QRect MInputContextAdaptor::preeditRectangle(bool &valid)
{
    return gMInputContextAdaptorStub->preeditRectangle(valid);
}

#endif
