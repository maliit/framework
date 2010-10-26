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
#include "mabstractinputmethodhost.h"

#include <QKeyEvent>

class MInputMethodBasePrivate
{
public:
    MInputMethodBasePrivate(MAbstractInputMethodHost *imHost, MInputMethodBase *parent);
    ~MInputMethodBasePrivate();

    MAbstractInputMethodHost *imHost;
};



MInputMethodBasePrivate::MInputMethodBasePrivate(MAbstractInputMethodHost *imHost,
                                                 MInputMethodBase *parent)
    : imHost(imHost)
{
}


MInputMethodBasePrivate::~MInputMethodBasePrivate()
{
}

///////////////

MInputMethodBase::MInputMethodBase(MAbstractInputMethodHost *imHost, QObject *parent)
    : QObject(parent),
      d_ptr(new MInputMethodBasePrivate(imHost, this))
{
    // nothing
}


MInputMethodBase::~MInputMethodBase()
{
    delete d_ptr;
}


MAbstractInputMethodHost *
MInputMethodBase::inputMethodHost() const
{
    Q_D(const MInputMethodBase);

    return d->imHost;
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

void MInputMethodBase::focusChanged(bool /* focusIn */)
{
    // empty default implementation
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

void MInputMethodBase::setToolbar(QSharedPointer<const MToolbarData> toolbar)
{
    // empty default implementation
    Q_UNUSED(toolbar);
}

void MInputMethodBase::processKeyEvent(QEvent::Type keyType, Qt::Key keyCode,
                                       Qt::KeyboardModifiers modifiers,
                                       const QString &text, bool autoRepeat, int count,
                                       quint32 /* nativeScanCode */, quint32 /* nativeModifiers */)
{
    // default implementation, just sendKeyEvent back
    inputMethodHost()->sendKeyEvent(QKeyEvent(keyType, keyCode, modifiers, text, autoRepeat,
                                              count));
}

void MInputMethodBase::setState(const QSet<MInputMethod::HandlerState> &state)
{
    // empty default implementation
    Q_UNUSED(state);
}

void MInputMethodBase::clientChanged()
{
    // empty default implementation
}

void MInputMethodBase::switchContext(MInputMethod::SwitchDirection direction, bool enableAnimation)
{
    // empty default implementation
    Q_UNUSED(direction);
    Q_UNUSED(enableAnimation);
}

QList<MInputMethodBase::MInputMethodSubView>
MInputMethodBase::subViews(MInputMethod::HandlerState state) const
{
    Q_UNUSED(state);
    QList<MInputMethodSubView> sVs;
    return sVs;
}

void MInputMethodBase::setActiveSubView(const QString &subViewId, MInputMethod::HandlerState state)
{
    // empty default implementation
    Q_UNUSED(subViewId);
    Q_UNUSED(state);
}

QString MInputMethodBase::activeSubView(MInputMethod::HandlerState state) const
{
    Q_UNUSED(state);
    return QString();
}

void MInputMethodBase::showLanguageNotification()
{
    // empty default implementation
}

