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

#include "minputmethodbase.h"
#include "minputcontextconnection.h"

#include <QGraphicsView>
#include <QDebug>
#include <QKeyEvent>


class MInputMethodBasePrivate
{
public:
    MInputMethodBasePrivate(MInputContextConnection *icConnection);
    ~MInputMethodBasePrivate();

    MInputContextConnection *icConnection;
};



MInputMethodBasePrivate::MInputMethodBasePrivate(MInputContextConnection *icConnection)
    : icConnection(icConnection)
{
    // nothing
}


MInputMethodBasePrivate::~MInputMethodBasePrivate()
{
    // nothing
}



///////////////

MInputMethodBase::MInputMethodBase(MInputContextConnection *icConnection, QObject *parent)
    : QObject(parent),
      d(new MInputMethodBasePrivate(icConnection))
{
    // nothing
}


MInputMethodBase::~MInputMethodBase()
{
    delete d;
}


MInputContextConnection *
MInputMethodBase::inputContextConnection() const
{
    return d->icConnection;
}

void MInputMethodBase::show()
{
    // empty default implementation
}

void MInputMethodBase::hide()
{
    // empty default implementation
}

void MInputMethodBase::setPreedit(const QString &)
{
    // empty default implementation
}

void MInputMethodBase::update()
{
    // empty default implementation
}

void MInputMethodBase::reset()
{
    // empty default implementation
}

void MInputMethodBase::mouseClickedOnPreedit(const QPoint &pos, const QRect &preeditRect)
{
    // empty default implementation
    Q_UNUSED(pos);
    Q_UNUSED(preeditRect);
}

void MInputMethodBase::visualizationPriorityChanged(bool priority)
{
    // empty default implementation
    Q_UNUSED(priority);
}

void MInputMethodBase::appOrientationChanged(int angle)
{
    // empty default implementation
    Q_UNUSED(angle);
}

void MInputMethodBase::setCopyPasteState(bool copyAvailable, bool pasteAvailable)
{
    // empty default implementation
    Q_UNUSED(copyAvailable);
    Q_UNUSED(pasteAvailable);
}

void MInputMethodBase::setToolbar(const QString &toolbar)
{
    // empty default implementation
    Q_UNUSED(toolbar);
}

void MInputMethodBase::processKeyEvent(QEvent::Type keyType, Qt::Key keyCode,
                                       Qt::KeyboardModifiers modifiers,
                                       const QString &text, bool autoRepeat, int count,
                                       int /* nativeScanCode */)
{
    // default implementation, just sendKeyEvent back
    inputContextConnection()->sendKeyEvent(QKeyEvent(keyType, keyCode, modifiers, text, autoRepeat,
                                                     count));
}

void MInputMethodBase::setState(const QSet<MIMHandlerState> &state)
{
    // empty default implementation
    Q_UNUSED(state);
}

void MInputMethodBase::clientChanged()
{
    // empty default implementation
}

void MInputMethodBase::switchContext(M::InputMethodSwitchDirection direction, bool enableAnimation)
{
    // empty default implementation
    Q_UNUSED(direction);
    Q_UNUSED(enableAnimation);
}

