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

#include "duiinputmethodbase.h"
#include "duiinputcontextconnection.h"

#include <QGraphicsView>
#include <QDebug>
#include <QKeyEvent>


class DuiInputMethodBasePrivate
{
public:
    DuiInputMethodBasePrivate(DuiInputContextConnection *icConnection);
    ~DuiInputMethodBasePrivate();

    DuiInputContextConnection *icConnection;
};



DuiInputMethodBasePrivate::DuiInputMethodBasePrivate(DuiInputContextConnection *icConnection)
    : icConnection(icConnection)
{
    // nothing
}


DuiInputMethodBasePrivate::~DuiInputMethodBasePrivate()
{
    // nothing
}



///////////////

DuiInputMethodBase::DuiInputMethodBase(DuiInputContextConnection *icConnection,
                                       QObject *parent)
    : QObject(parent),
      d(new DuiInputMethodBasePrivate(icConnection))
{
    // nothing
}


DuiInputMethodBase::~DuiInputMethodBase()
{
    delete d;
}


DuiInputContextConnection *
DuiInputMethodBase::inputContextConnection() const
{
    return d->icConnection;
}

void DuiInputMethodBase::show()
{
    // empty default implementation
}

void DuiInputMethodBase::hide()
{
    // empty default implementation
}

void DuiInputMethodBase::setPreedit(const QString &)
{
    // empty default implementation
}

void DuiInputMethodBase::update()
{
    // empty default implementation
}

void DuiInputMethodBase::reset()
{
    // empty default implementation
}

void DuiInputMethodBase::mouseClickedOnPreedit(const QPoint &pos, const QRect &preeditRect)
{
    // empty default implementation
    Q_UNUSED(pos);
    Q_UNUSED(preeditRect);
}

void DuiInputMethodBase::visualizationPriorityChanged(bool priority)
{
    // empty default implementation
    Q_UNUSED(priority);
}

void DuiInputMethodBase::appOrientationChanged(int angle)
{
    // empty default implementation
    Q_UNUSED(angle);
}

void DuiInputMethodBase::setCopyPasteState(bool copyAvailable, bool pasteAvailable)
{
    // empty default implementation
    Q_UNUSED(copyAvailable);
    Q_UNUSED(pasteAvailable);
}

void DuiInputMethodBase::setToolbar(const QString &toolbar)
{
    // empty default implementation
    Q_UNUSED(toolbar);
}

void DuiInputMethodBase::processKeyEvent(QEvent::Type keyType, Qt::Key keyCode,
                                         Qt::KeyboardModifiers modifiers,
                                         const QString &text, bool autoRepeat, int count,
                                         int /* nativeScanCode */)
{
    // default implementation, just sendKeyEvent back
    inputContextConnection()->sendKeyEvent(QKeyEvent(keyType, keyCode, modifiers, text, autoRepeat,
                                                     count));
}

void DuiInputMethodBase::setState(const QList<DuiIMHandlerState> &state)
{
    // empty default implementation
    Q_UNUSED(state);
}

void DuiInputMethodBase::clientChanged()
{
    // empty default implementation
}

