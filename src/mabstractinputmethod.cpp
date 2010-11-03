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

#include "mabstractinputmethod.h"
#include "mabstractinputmethodhost.h"

#include <QKeyEvent>

class MAbstractInputMethodPrivate
{
public:
    MAbstractInputMethodPrivate(MAbstractInputMethodHost *imHost, MAbstractInputMethod *parent);
    ~MAbstractInputMethodPrivate();

    MAbstractInputMethodHost *imHost;
};



MAbstractInputMethodPrivate::MAbstractInputMethodPrivate(MAbstractInputMethodHost *imHost,
                                                         MAbstractInputMethod *parent)
    : imHost(imHost)
{
}


MAbstractInputMethodPrivate::~MAbstractInputMethodPrivate()
{
}

///////////////

MAbstractInputMethod::MAbstractInputMethod(MAbstractInputMethodHost *imHost, QObject *parent)
    : QObject(parent),
      d_ptr(new MAbstractInputMethodPrivate(imHost, this))
{
    // nothing
}


MAbstractInputMethod::~MAbstractInputMethod()
{
    delete d_ptr;
}


MAbstractInputMethodHost *
MAbstractInputMethod::inputMethodHost() const
{
    Q_D(const MAbstractInputMethod);

    return d->imHost;
}

void MAbstractInputMethod::show()
{
    // empty default implementation
}

void MAbstractInputMethod::hide()
{
    // empty default implementation
}

void MAbstractInputMethod::setPreedit(const QString &, int)
{
    // empty default implementation
}

void MAbstractInputMethod::update()
{
    // empty default implementation
}

void MAbstractInputMethod::reset()
{
    // empty default implementation
}

void MAbstractInputMethod::handleMouseClickOnPreedit(const QPoint &pos, const QRect &preeditRect)
{
    // empty default implementation
    Q_UNUSED(pos);
    Q_UNUSED(preeditRect);
}

void MAbstractInputMethod::handleFocusChange(bool /* focusIn */)
{
    // empty default implementation
}

void MAbstractInputMethod::handleVisualizationPriorityChange(bool priority)
{
    // empty default implementation
    Q_UNUSED(priority);
}

void MAbstractInputMethod::handleAppOrientationChange(int angle)
{
    // empty default implementation
    Q_UNUSED(angle);
}

void MAbstractInputMethod::setToolbar(QSharedPointer<const MToolbarData> toolbar)
{
    // empty default implementation
    Q_UNUSED(toolbar);
}

void MAbstractInputMethod::processKeyEvent(QEvent::Type keyType, Qt::Key keyCode,
                                       Qt::KeyboardModifiers modifiers,
                                       const QString &text, bool autoRepeat, int count,
                                       quint32 /* nativeScanCode */, quint32 /* nativeModifiers */)
{
    // default implementation, just sendKeyEvent back
    inputMethodHost()->sendKeyEvent(QKeyEvent(keyType, keyCode, modifiers, text, autoRepeat,
                                              count));
}

void MAbstractInputMethod::setState(const QSet<MInputMethod::HandlerState> &state)
{
    // empty default implementation
    Q_UNUSED(state);
}

void MAbstractInputMethod::handleClientChange()
{
    // empty default implementation
}

void MAbstractInputMethod::switchContext(MInputMethod::SwitchDirection direction,
                                         bool enableAnimation)
{
    // empty default implementation
    Q_UNUSED(direction);
    Q_UNUSED(enableAnimation);
}

QList<MAbstractInputMethod::MInputMethodSubView>
MAbstractInputMethod::subViews(MInputMethod::HandlerState state) const
{
    Q_UNUSED(state);
    QList<MInputMethodSubView> sVs;
    return sVs;
}

void MAbstractInputMethod::setActiveSubView(const QString &subViewId,
                                            MInputMethod::HandlerState state)
{
    // empty default implementation
    Q_UNUSED(subViewId);
    Q_UNUSED(state);
}

QString MAbstractInputMethod::activeSubView(MInputMethod::HandlerState state) const
{
    Q_UNUSED(state);
    return QString();
}

void MAbstractInputMethod::showLanguageNotification()
{
    // empty default implementation
}

