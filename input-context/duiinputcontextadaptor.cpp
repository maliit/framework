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

#include "duiinputcontext.h"
#include "duiinputcontextadaptor.h"

#include <QDBusArgument>


DuiInputContextAdaptor::DuiInputContextAdaptor(DuiInputContext *parent)
    : QDBusAbstractAdaptor(parent),
      owner(parent)
{
    if (!parent) {
        qFatal("Creating DuiInputContextAdaptor without a parent");
    }
}


DuiInputContextAdaptor::~DuiInputContextAdaptor()
{
    // nothing
}


void DuiInputContextAdaptor::activationLostEvent()
{
    owner->activationLostEvent();
}


void DuiInputContextAdaptor::imInitiatedHide()
{
    owner->imInitiatedHide();
}


void DuiInputContextAdaptor::commitString(const QString &string)
{
    owner->commitString(string);
}


void DuiInputContextAdaptor::updatePreedit(const QString &string, int preeditFace)
{
    Dui::PreeditFace face = Dui::PreeditDefault;

    if (preeditFace >= Dui::PreeditDefault
            && preeditFace <= Dui::PreeditNoCandidates)
        face = Dui::PreeditFace(preeditFace);

    owner->updatePreedit(string, face);
}


void DuiInputContextAdaptor::keyEvent(int type, int key, int modifiers, const QString &text,
                                      bool autoRepeat, int count)
{
    owner->keyEvent(type, key, modifiers, text, autoRepeat, count);
}


void DuiInputContextAdaptor::updateInputMethodArea(const QList<QVariant>& data)
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


int DuiInputContextAdaptor::contentType(bool &valid)
{
    DuiInputContext::WidgetInfo info = owner->getFocusWidgetInfo();

    valid = info.valid;

    if (info.valid) {
        return info.contentType;
    }

    return -1;
}


bool DuiInputContextAdaptor::correctionEnabled(bool &valid)
{
    DuiInputContext::WidgetInfo info = owner->getFocusWidgetInfo();

    valid = info.valid;

    if (info.valid) {
        return info.correctionEnabled;
    }

    return false;
}


bool DuiInputContextAdaptor::predictionEnabled(bool &valid)
{
    DuiInputContext::WidgetInfo info = owner->getFocusWidgetInfo();

    valid = info.valid;

    if (info.valid) {
        return info.predictionEnabled;
    }

    return false;
}


bool DuiInputContextAdaptor::autoCapitalizationEnabled(bool &valid)
{
    DuiInputContext::WidgetInfo info = owner->getFocusWidgetInfo();

    valid = info.valid;

    if (info.valid) {
        return info.autoCapitalizationEnabled;
    }

    return false;
}


void DuiInputContextAdaptor::setGlobalCorrectionEnabled(bool enabled)
{
    owner->setGlobalCorrectionEnabled(enabled);
}


void DuiInputContextAdaptor::copy()
{
    owner->copy();
}


void DuiInputContextAdaptor::paste()
{
    owner->paste();
}


bool DuiInputContextAdaptor::surroundingText(QString &text, int &cursorPosition)
{
    return owner->surroundingText(text, cursorPosition);
}

bool DuiInputContextAdaptor::hasSelection(bool &valid)
{
    return owner->hasSelection(valid);
}

int DuiInputContextAdaptor::inputMethodMode(bool &valid)
{
    DuiInputContext::WidgetInfo info = owner->getFocusWidgetInfo();

    valid = info.valid;

    if (info.valid) {
        return info.inputMethodMode;
    }

    return -1;
}

QRect DuiInputContextAdaptor::preeditRectangle(bool &valid)
{
    return owner->preeditRectangle(valid);
}

void DuiInputContextAdaptor::setRedirectKeys(bool enabled)
{
    owner->setRedirectKeys(enabled);
}
