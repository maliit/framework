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
#ifndef DUIINPUTCONTEXT_STUB_H
#define DUIINPUTCONTEXT_STUB_H

#include "duiinputcontext.h"
#include "stubbase.h"

#include <QDebug>


/**
 * DuiInputContext stub class.
 * To fake DuiInputContext operations, derive from this class
 * and implement the methods you want to fake. Instantiate your
 * derived stub class and assign it to gDuiInputContextStub
 * global variable.
 */
class DuiInputContextStub : public StubBase
{
public:
    virtual void duiInputContextConstructor(QObject *parent);
    virtual void duiInputContextDestructor();
    virtual bool event(QEvent *event);
    virtual QString identifierName();
    virtual bool isComposing();
    virtual QString language();
    virtual void reset();
    virtual void update();
    virtual void mouseHandler(int x, QMouseEvent *event);
    virtual void imInitiatedHide();
    virtual void commitString(const QString &string);
    virtual void updatePreedit(const QString &string, Dui::PreeditFace preeditFace);
    virtual void keyEvent(int type, int key, int modifiers, const QString &text,
                          bool autoRepeat, int count);
    virtual void updateInputMethodArea(const QList<QVariant> &rectList);
    virtual void setGlobalCorrectionEnabled(bool enable);
    virtual DuiInputContext::WidgetInfo getFocusWidgetInfo();
    virtual bool surroundingText(QString &text, int &cursorPosition);
};

void DuiInputContextStub::duiInputContextConstructor(QObject *)
{
}

void DuiInputContextStub::duiInputContextDestructor()
{
}

bool DuiInputContextStub::event(QEvent *event)
{
    QList<ParameterBase *> params;
    params.append(new Parameter<QEvent *>(event));
    stubMethodEntered("event", params);
    return stubReturnValue<bool>("event");
}

QString DuiInputContextStub::identifierName()
{
    stubMethodEntered("identifierName");
    return stubReturnValue<QString>("identifierName");
}

bool DuiInputContextStub::isComposing()
{
    stubMethodEntered("isComposing");
    return stubReturnValue<bool>("isComposing");
}

QString DuiInputContextStub::language()
{
    stubMethodEntered("language");
    return stubReturnValue<QString>("language");
}

void DuiInputContextStub::reset()
{
    stubMethodEntered("reset");
}

void DuiInputContextStub::update()
{
    stubMethodEntered("update");
}

void DuiInputContextStub::mouseHandler(int x, QMouseEvent *event)
{
    QList<ParameterBase *> params;
    params.append(new Parameter<int>(x));
    params.append(new Parameter<QMouseEvent *>(event));
    stubMethodEntered("mouseHandler", params);
}

void DuiInputContextStub::imInitiatedHide()
{
    stubMethodEntered("imInitiatedHide");
}

void DuiInputContextStub::commitString(const QString &string)
{
    QList<ParameterBase *> params;
    params.append(new Parameter<QString>(string));
    stubMethodEntered("commitString", params);
}

void DuiInputContextStub::updatePreedit(const QString &string, Dui::PreeditFace preeditFace)
{
    QList<ParameterBase *> params;
    params.append(new Parameter<QString>(string));
    params.append(new Parameter<Dui::PreeditFace>(preeditFace));
    stubMethodEntered("updatePreedit", params);
}

void DuiInputContextStub::keyEvent(int type, int key, int modifiers, const QString &text,
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

void DuiInputContextStub::updateInputMethodArea(const QList<QVariant> &rectList)
{
    QList<ParameterBase *> params;
    params.append(new Parameter<const QList<QVariant> >(rectList));
    stubMethodEntered("updateInputMethodArea", params);
}

void DuiInputContextStub::setGlobalCorrectionEnabled(bool enable)
{
    QList<ParameterBase *> params;
    params.append(new Parameter<bool>(enable));
    stubMethodEntered("setGlobalCorrectionEnabled", params);
}

DuiInputContext::WidgetInfo DuiInputContextStub::getFocusWidgetInfo()
{
    stubMethodEntered("getFocusWidgetInfo");
    return stubReturnValue<DuiInputContext::WidgetInfo>("getFocusWidgetInfo");
}

bool DuiInputContextStub::surroundingText(QString &text, int &cursorPosition)
{
    QList<ParameterBase *> params;
    params.append(new Parameter<QString &>(text));
    params.append(new Parameter<int &>(cursorPosition));
    stubMethodEntered("surroundingText", params);
    return stubReturnValue<bool>("surroundingText");
}

DuiInputContextStub gDefaultDuiInputContextStub;

/**
 * This is the stub class instance used by the system. If you want to alter behaviour,
 * derive your stub class from DuiInputContextStub, implement the methods you want to
 * fake, create an instance of your stub class and assign the instance into this global variable.
 */
DuiInputContextStub *gDuiInputContextStub = &gDefaultDuiInputContextStub;

/**
 * These are the proxy method implementations of DuiInputContext. They will
 * call the stub object methods of the gDuiInputContextStub.
 */

DuiInputContext::DuiInputContext(QObject *parent)
    : QInputContext(parent)
{
    gDuiInputContextStub->duiInputContextConstructor(parent);
}

DuiInputContext::~DuiInputContext()
{
    gDuiInputContextStub->duiInputContextDestructor();
}

bool DuiInputContext::event(QEvent *event)
{
    return gDuiInputContextStub->event(event);
}

QString DuiInputContext::identifierName()
{
    return gDuiInputContextStub->identifierName();
}

bool DuiInputContext::isComposing() const
{
    return gDuiInputContextStub->isComposing();
}

QString DuiInputContext::language()
{
    return gDuiInputContextStub->language();
}

void DuiInputContext::reset()
{
    gDuiInputContextStub->reset();
}

void DuiInputContext::update()
{
    gDuiInputContextStub->update();
}

void DuiInputContext::mouseHandler(int x, QMouseEvent *event)
{
    gDuiInputContextStub->mouseHandler(x, event);
}

void DuiInputContext::imInitiatedHide()
{
    gDuiInputContextStub->imInitiatedHide();
}

void DuiInputContext::commitString(const QString &string)
{
    gDuiInputContextStub->commitString(string);
}

void DuiInputContext::updatePreedit(const QString &string, Dui::PreeditFace preeditFace)
{
    gDuiInputContextStub->updatePreedit(string, preeditFace);
}

void DuiInputContext::keyEvent(int type, int key, int modifiers, const QString &text,
                               bool autoRepeat, int count)
{
    gDuiInputContextStub->keyEvent(type, key, modifiers, text, autoRepeat, count);
}

void DuiInputContext::updateInputMethodArea(const QList<QVariant> &rectList)
{
    gDuiInputContextStub->updateInputMethodArea(rectList);
}

void DuiInputContext::setGlobalCorrectionEnabled(bool enable)
{
    gDuiInputContextStub->setGlobalCorrectionEnabled(enable);
}

DuiInputContext::WidgetInfo DuiInputContext::getFocusWidgetInfo()
{
    return gDuiInputContextStub->getFocusWidgetInfo();
}

bool DuiInputContext::surroundingText(QString &text, int &cursorPosition)
{
    return gDuiInputContextStub->surroundingText(text, cursorPosition);
}

#endif
