/* * This file is part of Maliit framework *
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 *
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 */

#include <maliit/plugins/abstractinputmethod.h>
#include <maliit/plugins/abstractinputmethodhost.h>

#include <QKeyEvent>

class MAbstractInputMethodPrivate
{
public:
    MAbstractInputMethodPrivate(MAbstractInputMethodHost *imHost,
                                MAbstractInputMethod *parent);
    ~MAbstractInputMethodPrivate();

    MAbstractInputMethodHost *imHost;
};



MAbstractInputMethodPrivate::MAbstractInputMethodPrivate(MAbstractInputMethodHost *imHost,
                                                         MAbstractInputMethod *parent)
    : imHost(imHost)
{
    Q_UNUSED(parent)
}


MAbstractInputMethodPrivate::~MAbstractInputMethodPrivate()
{
}

///////////////

MAbstractInputMethod::MAbstractInputMethod(MAbstractInputMethodHost *host)
    : QObject(0), // MAbstractInputMethod is not deleted by mainWindow
      d_ptr(new MAbstractInputMethodPrivate(host, this))
{
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

void MAbstractInputMethod::handleAppOrientationAboutToChange(int angle)
{
    // empty default implementation
    Q_UNUSED(angle);
}

void MAbstractInputMethod::handleAppOrientationChanged(int angle)
{
    // empty default implementation
    Q_UNUSED(angle);
}

void MAbstractInputMethod::processKeyEvent(QEvent::Type keyType, Qt::Key keyCode,
                                           Qt::KeyboardModifiers modifiers,
                                           const QString &text, bool autoRepeat, int count,
                                           quint32 /* nativeScanCode */, quint32 /* nativeModifiers */,
                                           unsigned long /*time*/)
{
    // default implementation, just sendKeyEvent back
    inputMethodHost()->sendKeyEvent(QKeyEvent(keyType, keyCode, modifiers, text, autoRepeat,
                                              count));
}

void MAbstractInputMethod::setState(const QSet<Maliit::HandlerState> &state)
{
    // empty default implementation
    Q_UNUSED(state);
}

void MAbstractInputMethod::handleClientChange()
{
    // empty default implementation
}

void MAbstractInputMethod::switchContext(Maliit::SwitchDirection direction,
                                         bool enableAnimation)
{
    // empty default implementation
    Q_UNUSED(direction);
    Q_UNUSED(enableAnimation);
}

QList<MAbstractInputMethod::MInputMethodSubView>
MAbstractInputMethod::subViews(Maliit::HandlerState state) const
{
    Q_UNUSED(state);
    QList<MInputMethodSubView> sVs;
    return sVs;
}

void MAbstractInputMethod::setActiveSubView(const QString &subViewId,
                                            Maliit::HandlerState state)
{
    // empty default implementation
    Q_UNUSED(subViewId);
    Q_UNUSED(state);
}

QString MAbstractInputMethod::activeSubView(Maliit::HandlerState state) const
{
    Q_UNUSED(state);
    return QString();
}

void MAbstractInputMethod::showLanguageNotification()
{
    // empty default implementation
}

void MAbstractInputMethod::setKeyOverrides(const QMap<QString, QSharedPointer<MKeyOverride> > &overrides)
{
    // empty default implementation
    Q_UNUSED(overrides);
}
