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

#include "minputcontext.h"
#include "minputcontextadaptor.h"

#include <QDBusArgument>


MInputContextAdaptor::MInputContextAdaptor(MInputContext *parent)
    : QDBusAbstractAdaptor(parent),
      owner(parent)
{
    if (!parent) {
        qFatal("Creating MInputContextAdaptor without a parent");
    }
}


MInputContextAdaptor::~MInputContextAdaptor()
{
    // nothing
}


void MInputContextAdaptor::activationLostEvent()
{
    owner->activationLostEvent();
}


void MInputContextAdaptor::imInitiatedHide()
{
    owner->imInitiatedHide();
}


void MInputContextAdaptor::commitString(const QString &string)
{
    owner->commitString(string);
}


void MInputContextAdaptor::updatePreedit(const QString &string, int preeditFace)
{
    MInputMethod::PreeditFace face = MInputMethod::PreeditDefault;

    if (preeditFace >= MInputMethod::PreeditDefault
        && preeditFace <= MInputMethod::PreeditNoCandidates)
        face = MInputMethod::PreeditFace(preeditFace);

    owner->updatePreedit(string, face);
}


void MInputContextAdaptor::keyEvent(int type, int key, int modifiers, const QString &text,
                                    bool autoRepeat, int count, uchar requestType)
{
    owner->keyEvent(type, key, modifiers, text, autoRepeat, count,
                    static_cast<MInputMethod::EventRequestType>(requestType));
}


void MInputContextAdaptor::updateInputMethodArea(const QList<QVariant>& data)
{
    QList<QVariant> newData;
    const int dataSize = data.size();
    for (int i = 0; i < dataSize; ++i) {
        QDBusArgument e = qvariant_cast<QDBusArgument>(data.at(i));
        QRect r = qdbus_cast<QRect>(e);
        newData.append(r);
    }
    owner->updateInputMethodArea(newData);
}


void MInputContextAdaptor::setGlobalCorrectionEnabled(bool enabled)
{
    owner->setGlobalCorrectionEnabled(enabled);
}


void MInputContextAdaptor::copy()
{
    owner->copy();
}


void MInputContextAdaptor::paste()
{
    owner->paste();
}

QRect MInputContextAdaptor::preeditRectangle(bool &valid)
{
    return owner->preeditRectangle(valid);
}

void MInputContextAdaptor::setRedirectKeys(bool enabled)
{
    owner->setRedirectKeys(enabled);
}

void MInputContextAdaptor::setDetectableAutoRepeat(bool enabled)
{
    owner->setDetectableAutoRepeat(enabled);
}
