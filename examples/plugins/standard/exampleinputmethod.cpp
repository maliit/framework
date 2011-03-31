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

#include "exampleinputmethod.h"

#include <mabstractinputmethodhost.h>

#include <QDebug>
#include <QtGui>

namespace {
    const char * const exampleSubViewId("ExamplePluginSubview1");
}

ExampleInputMethod::ExampleInputMethod(MAbstractInputMethodHost *host,
                                       QWidget *mainWindow)
    : MAbstractInputMethod(host, mainWindow)
    , mainWidget(new QPushButton(mainWindow))
    , showIsInhibited(false)
    , showRequested(false)
{
    // Set up UI
    mainWidget->setText("Hello World");
    connect(mainWidget, SIGNAL(clicked()), this, SLOT(handleButtonClicked()));

    // Used only for unittest/sanity test
    inputMethodHost()->sendCommitString("Maliit");
    inputMethodHost()->sendPreeditString("Mali", QList<MInputMethod::PreeditTextFormat>(), 0, 6);
}

ExampleInputMethod::~ExampleInputMethod()
{}

// Slot for our Hello World button
void ExampleInputMethod::handleButtonClicked()
{
    inputMethodHost()->sendCommitString(mainWidget->text());
}

void ExampleInputMethod::show()
{
    showRequested = true;
    if (showIsInhibited) {
        return;
    }

    // Set size of our container to screen size
    const QSize screenSize = qApp->desktop()->screenGeometry().size();
    mainWidget->parentWidget()->resize(screenSize);

    // Set size of the input method
    const QRect imGeometry(0, screenSize.height() - 200, screenSize.width(), 200);
    mainWidget->setGeometry(imGeometry);

    // Tell input method server about our size
    inputMethodHost()->setScreenRegion(QRegion(mainWidget->geometry()));
    inputMethodHost()->setInputMethodArea(QRegion(mainWidget->geometry()));

    mainWidget->show();
}

void ExampleInputMethod::hide()
{
    if (!showRequested) {
        return;
    }
    showRequested = false;

    mainWidget->hide();

    inputMethodHost()->setScreenRegion(QRegion());
    inputMethodHost()->setInputMethodArea(QRegion());
}

QList<MAbstractInputMethod::MInputMethodSubView>
ExampleInputMethod::subViews(MInputMethod::HandlerState state) const
{
    QList<MAbstractInputMethod::MInputMethodSubView> subViews;

    if (state == MInputMethod::OnScreen) {
        MAbstractInputMethod::MInputMethodSubView subView1;
        subView1.subViewId = exampleSubViewId;
        subView1.subViewTitle = "Example plugin subview 1";
        subViews.append(subView1);
    }
    return subViews;
}

QString ExampleInputMethod::activeSubView(MInputMethod::HandlerState state) const
{
    QString subView = (state == MInputMethod::OnScreen) ? exampleSubViewId : "";
    return subView;
}

void ExampleInputMethod::setState(const QSet<MInputMethod::HandlerState> &state)
{
    if (state.contains(MInputMethod::OnScreen)) {
        if (showRequested && !showIsInhibited) {
            mainWidget->show();
        }
    } else {
        mainWidget->hide();
    }
}

void ExampleInputMethod::handleClientChange()
{
    if (showRequested) {
        hide();
    }
}

void ExampleInputMethod::handleVisualizationPriorityChange(bool inhibitShow)
{
    if (showIsInhibited == inhibitShow) {
        return;
    }
    showIsInhibited = inhibitShow;

    if (showRequested) {
        if (inhibitShow) {
            mainWidget->hide();
        } else {
            mainWidget->show();
        }
    }
}

void ExampleInputMethod::handleAppOrientationAboutToChange(int angle)
{
    // Rotate your input method UI here
    Q_UNUSED(angle);
}

void ExampleInputMethod::handleAppOrientationChanged(int angle)
{
    // Can typically be forwarded to handleAppOrientationAboutToChange
    // as long as that method will not do anything when newAngle == previousAngle
    Q_UNUSED(angle);
}

void ExampleInputMethod::update()
{
    // empty default implementation
}

void ExampleInputMethod::reset()
{
    // empty default implementation
}

void ExampleInputMethod::handleFocusChange(bool focusIn)
{
    // empty default implementation
    Q_UNUSED(focusIn);
}

void ExampleInputMethod::switchContext(MInputMethod::SwitchDirection direction,
                                       bool enableAnimation)
{
    // empty default implementation
    Q_UNUSED(direction);
    Q_UNUSED(enableAnimation);
}

void ExampleInputMethod::setPreedit(const QString &preeditString, int cursorPos)
{
    // empty default implementation
    Q_UNUSED(preeditString);
    Q_UNUSED(cursorPos);
}

void ExampleInputMethod::setActiveSubView(const QString &subViewId, MInputMethod::HandlerState state)
{
    // Ignored as input method only support one subview
    Q_UNUSED(subViewId);
    Q_UNUSED(state);
}
