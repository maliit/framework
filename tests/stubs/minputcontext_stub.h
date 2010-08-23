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
#ifndef MINPUTCONTEXT_STUB_H
#define MINPUTCONTEXT_STUB_H

#include "minputcontext.h"
#include "stubbase.h"

#include <QDebug>


/**
 * MInputContext stub class.
 * To fake MInputContext operations, derive from this class
 * and implement the methods you want to fake. Instantiate your
 * derived stub class and assign it to gMInputContextStub
 * global variable.
 */
class MInputContextStub : public StubBase
{
public:
    virtual void mInputContextConstructor(QObject *parent);
    virtual void mInputContextDestructor();
    virtual bool event(QEvent *event);
    virtual QString identifierName();
    virtual bool isComposing();
    virtual QString language();
    virtual void reset();
    virtual void update();
    virtual void mouseHandler(int x, QMouseEvent *event);
    virtual void imInitiatedHide();
    virtual void commitString(const QString &string);
    virtual void updatePreedit(const QString &string, PreeditFace preeditFace);
    virtual void keyEvent(int type, int key, int modifiers, const QString &text,
                          bool autoRepeat, int count);
    virtual void updateInputMethodArea(const QList<QVariant> &rectList);
    virtual void setGlobalCorrectionEnabled(bool enable);
};

void MInputContextStub::mInputContextConstructor(QObject *)
{
}

void MInputContextStub::mInputContextDestructor()
{
}

bool MInputContextStub::event(QEvent *event)
{
    QList<ParameterBase *> params;
    params.append(new Parameter<QEvent *>(event));
    stubMethodEntered("event", params);
    return stubReturnValue<bool>("event");
}

QString MInputContextStub::identifierName()
{
    stubMethodEntered("identifierName");
    return stubReturnValue<QString>("identifierName");
}

bool MInputContextStub::isComposing()
{
    stubMethodEntered("isComposing");
    return stubReturnValue<bool>("isComposing");
}

QString MInputContextStub::language()
{
    stubMethodEntered("language");
    return stubReturnValue<QString>("language");
}

void MInputContextStub::reset()
{
    stubMethodEntered("reset");
}

void MInputContextStub::update()
{
    stubMethodEntered("update");
}

void MInputContextStub::mouseHandler(int x, QMouseEvent *event)
{
    QList<ParameterBase *> params;
    params.append(new Parameter<int>(x));
    params.append(new Parameter<QMouseEvent *>(event));
    stubMethodEntered("mouseHandler", params);
}

void MInputContextStub::imInitiatedHide()
{
    stubMethodEntered("imInitiatedHide");
}

void MInputContextStub::commitString(const QString &string)
{
    QList<ParameterBase *> params;
    params.append(new Parameter<QString>(string));
    stubMethodEntered("commitString", params);
}

void MInputContextStub::updatePreedit(const QString &string, PreeditFace preeditFace)
{
    QList<ParameterBase *> params;
    params.append(new Parameter<QString>(string));
    params.append(new Parameter<PreeditFace>(preeditFace));
    stubMethodEntered("updatePreedit", params);
}

void MInputContextStub::keyEvent(int type, int key, int modifiers, const QString &text,
                                   bool autoRepeat, int count)
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

void MInputContextStub::updateInputMethodArea(const QList<QVariant> &rectList)
{
    QList<ParameterBase *> params;
    params.append(new Parameter<const QList<QVariant> >(rectList));
    stubMethodEntered("updateInputMethodArea", params);
}

void MInputContextStub::setGlobalCorrectionEnabled(bool enable)
{
    QList<ParameterBase *> params;
    params.append(new Parameter<bool>(enable));
    stubMethodEntered("setGlobalCorrectionEnabled", params);
}


MInputContextStub gDefaultMInputContextStub;

/**
 * This is the stub class instance used by the system. If you want to alter behaviour,
 * derive your stub class from MInputContextStub, implement the methods you want to
 * fake, create an instance of your stub class and assign the instance into this global variable.
 */
MInputContextStub *gMInputContextStub = &gDefaultMInputContextStub;

/**
 * These are the proxy method implementations of MInputContext. They will
 * call the stub object methods of the gMInputContextStub.
 */

MInputContext::MInputContext(QObject *parent)
    : QInputContext(parent),
      active(false),
      inputPanelState(InputPanelHidden),
      sipHideTimer(0),
      iface(0),
      ownsMComponentData(false),
      correctionEnabled(false),
      styleContainer(0),
      connectedObject(0),
      pasteAvailable(false),
      copyAllowed(false),
      redirectKeys(false),
      objectPath("")
{
    gMInputContextStub->mInputContextConstructor(parent);
}

MInputContext::~MInputContext()
{
    gMInputContextStub->mInputContextDestructor();
}

bool MInputContext::event(QEvent *event)
{
    return gMInputContextStub->event(event);
}

QString MInputContext::identifierName()
{
    return gMInputContextStub->identifierName();
}

bool MInputContext::isComposing() const
{
    return gMInputContextStub->isComposing();
}

QString MInputContext::language()
{
    return gMInputContextStub->language();
}

void MInputContext::reset()
{
    gMInputContextStub->reset();
}

void MInputContext::update()
{
    gMInputContextStub->update();
}

void MInputContext::mouseHandler(int x, QMouseEvent *event)
{
    gMInputContextStub->mouseHandler(x, event);
}

void MInputContext::imInitiatedHide()
{
    gMInputContextStub->imInitiatedHide();
}

void MInputContext::commitString(const QString &string)
{
    gMInputContextStub->commitString(string);
}

void MInputContext::updatePreedit(const QString &string, PreeditFace preeditFace)
{
    gMInputContextStub->updatePreedit(string, preeditFace);
}

void MInputContext::keyEvent(int type, int key, int modifiers, const QString &text,
                               bool autoRepeat, int count)
{
    gMInputContextStub->keyEvent(type, key, modifiers, text, autoRepeat, count);
}

void MInputContext::updateInputMethodArea(const QList<QVariant> &rectList)
{
    gMInputContextStub->updateInputMethodArea(rectList);
}

void MInputContext::setGlobalCorrectionEnabled(bool enable)
{
    gMInputContextStub->setGlobalCorrectionEnabled(enable);
}


#endif
