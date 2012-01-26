/* * This file is part of meego-im-framework *
 *
 * Copyright (C) 2010, 2011 Nokia Corporation and/or its subsidiary(-ies).
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

// Based on minputmethodstate.cpp from libmeegotouch

#include "inputmethod.h"
#include "inputmethod_p.h"

#if QT_VERSION >= 0x050000
#include <QGuiApplication>
#include <QInputPanel>
#else
#include <QApplication>
#include <QInputContext>
#endif

namespace Maliit {

InputMethodPrivate::InputMethodPrivate() :
    area(),
    widget(0),
    orientationAngle(),
    rotationInProgress(false)
{
}

InputMethodPrivate::~InputMethodPrivate()
{
}

InputMethod::InputMethod() :
    QObject(),
    d_ptr(new InputMethodPrivate)
{
}

InputMethod::~InputMethod()
{
}

InputMethod *InputMethod::instance()
{
    static InputMethod singleton;

    return &singleton;
}

void InputMethod::setWidget(QWidget *widget)
{
    Q_D(InputMethod);
    d->widget = widget;
}

QWidget *InputMethod::widget() const
{
    Q_D(const InputMethod);
    return d->widget;
}

QRect InputMethod::area() const
{
    Q_D(const InputMethod);

    return d->area;
}

void InputMethod::setArea(const QRect &newArea)
{
    Q_D(InputMethod);

    if (d->area != newArea) {
        d->area = newArea;
        Q_EMIT areaChanged(d->area);
    }
}

void InputMethod::startOrientationAngleChange(OrientationAngle newOrientationAngle)
{
    Q_D(InputMethod);

    if (d->orientationAngle != newOrientationAngle) {
        d->orientationAngle = newOrientationAngle;
        d->rotationInProgress = true;
        Q_EMIT orientationAngleAboutToChange(d->orientationAngle);
    }
}

void InputMethod::setOrientationAngle(OrientationAngle newOrientationAngle)
{
    Q_D(InputMethod);

    if (d->orientationAngle != newOrientationAngle) {
        d->orientationAngle = newOrientationAngle;
        d->rotationInProgress = true;
    }

    if (d->rotationInProgress) {
        d->rotationInProgress = false;
        Q_EMIT orientationAngleChanged(d->orientationAngle);
    }
}

OrientationAngle InputMethod::orientationAngle() const
{
    Q_D(const InputMethod);

    return d->orientationAngle;
}

void InputMethod::emitKeyPress(const QKeyEvent &event)
{
    Q_EMIT keyPress(event);
}

void InputMethod::emitKeyRelease(const QKeyEvent &event)
{
    Q_EMIT keyRelease(event);
}

void InputMethod::setLanguage(const QString &language)
{
    Q_D(InputMethod);
    if (d->language != language) {
        d->language = language;
        Q_EMIT languageChanged(language);
    }
}

const QString &InputMethod::language() const
{
    Q_D(const InputMethod);
    return d->language;
}

void requestInputMethodPanel()
{
#if QT_VERSION >= 0x050000
    qApp->inputPanel()->show();
#else
    QInputContext *inputContext = qApp->inputContext();

    if (!inputContext) {
        return;
    }

    QEvent request(QEvent::RequestSoftwareInputPanel);
    inputContext->filterEvent(&request);
#endif
}

void closeInputMethodPanel()
{
#if QT_VERSION >= 0x050000
    qApp->inputPanel()->hide();
#else
    QInputContext *inputContext = qApp->inputContext();

    if (!inputContext) {
        return;
    }

    QEvent close(QEvent::CloseSoftwareInputPanel);
    inputContext->filterEvent(&close);
    inputContext->reset();
#endif
}

} // namespace Maliit
