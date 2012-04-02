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

#include "overrideinputmethod.h"

#include <mabstractinputmethodhost.h>

#include <QDebug>
#include <QApplication>
#include <QDesktopWidget>
#include <QKeyEvent>

namespace {
    const char * const overrideSubViewId("OverridePluginSubview1");
    const char * const actionKeyName = "actionKey";
    const char * const actionKeyLabel = "Enter";
}

OverrideInputMethod::OverrideInputMethod(MAbstractInputMethodHost *host,
                                         QWidget *mainWindow)
    : MAbstractInputMethod(host, mainWindow)
    , mainWidget(new QPushButton(mainWindow))
    , showIsInhibited(false)
    , showRequested(false)
    , activeActionKeyOverride()
{
    // Set up UI
    mainWidget->setText(actionKeyLabel);
    connect(mainWidget, SIGNAL(clicked()), this, SLOT(handleButtonClicked()));

    // Used only for unittest/sanity test
    inputMethodHost()->sendCommitString("Maliit");
    inputMethodHost()->sendPreeditString("Mali", QList<Maliit::PreeditTextFormat>(), 0, 6);
}

OverrideInputMethod::~OverrideInputMethod()
{}

// Slot for our action key
void OverrideInputMethod::handleButtonClicked()
{
    const QKeyEvent event(QEvent::KeyPress, Qt::Key_Return, Qt::NoModifier);

    inputMethodHost()->sendKeyEvent(event);
}

void OverrideInputMethod::show()
{
    showRequested = true;
    if (showIsInhibited) {
        return;
    }

    // Set size of our container to screen size
    const QSize screenSize = QApplication::desktop()->screenGeometry().size();
    mainWidget->parentWidget()->resize(screenSize);

    // Set size of the input method
    const QRect imGeometry(0, screenSize.height() - 200, screenSize.width(), 200);
    mainWidget->setGeometry(imGeometry);

    // Tell input method server about our size
    inputMethodHost()->setScreenRegion(QRegion(mainWidget->geometry()));
    inputMethodHost()->setInputMethodArea(QRegion(mainWidget->geometry()));

    mainWidget->show();
}

void OverrideInputMethod::hide()
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
OverrideInputMethod::subViews(Maliit::HandlerState state) const
{
    QList<MAbstractInputMethod::MInputMethodSubView> subViews;

    if (state == Maliit::OnScreen) {
        MAbstractInputMethod::MInputMethodSubView subView1;
        subView1.subViewId = overrideSubViewId;
        subView1.subViewTitle = "Override plugin subview 1";
        subViews.append(subView1);
    }
    return subViews;
}

QString OverrideInputMethod::activeSubView(Maliit::HandlerState state) const
{
    QString subView = (state == Maliit::OnScreen) ? overrideSubViewId : "";
    return subView;
}

void OverrideInputMethod::setState(const QSet<Maliit::HandlerState> &state)
{
    if (state.contains(Maliit::OnScreen)) {
        if (showRequested && !showIsInhibited) {
            mainWidget->show();
        }
    } else {
        mainWidget->hide();
    }
}

void OverrideInputMethod::handleClientChange()
{
    if (showRequested) {
        hide();
    }
}

void OverrideInputMethod::handleVisualizationPriorityChange(bool inhibitShow)
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

void OverrideInputMethod::handleAppOrientationAboutToChange(int angle)
{
    // Rotate your input method UI here
    Q_UNUSED(angle);
}

void OverrideInputMethod::handleAppOrientationChanged(int angle)
{
    // Can typically be forwarded to handleAppOrientationAboutToChange
    // as long as that method will not do anything when newAngle == previousAngle
    Q_UNUSED(angle);
}

void OverrideInputMethod::update()
{
    // empty default implementation
}

void OverrideInputMethod::reset()
{
    // empty default implementation
}

void OverrideInputMethod::handleFocusChange(bool focusIn)
{
    // empty default implementation
    Q_UNUSED(focusIn);
}

void OverrideInputMethod::switchContext(Maliit::SwitchDirection direction,
                                        bool enableAnimation)
{
    // empty default implementation
    Q_UNUSED(direction);
    Q_UNUSED(enableAnimation);
}

void OverrideInputMethod::setPreedit(const QString &preeditString,
                                     int cursorPos)
{
    // empty default implementation
    Q_UNUSED(preeditString);
    Q_UNUSED(cursorPos);
}

void OverrideInputMethod::setActiveSubView(const QString &subViewId,
                                           Maliit::HandlerState state)
{
    // Ignored as input method only support one subview
    Q_UNUSED(subViewId);
    Q_UNUSED(state);
}

void OverrideInputMethod::setKeyOverrides(const QMap<QString, QSharedPointer<MKeyOverride> > &overrides)
{
    if (activeActionKeyOverride) {
        disconnect(activeActionKeyOverride.data(), SIGNAL(keyAttributesChanged(const QString &, const MKeyOverride::KeyOverrideAttributes)),
                   this, SLOT(onKeyAttributesChanged(const QString &, const MKeyOverride::KeyOverrideAttributes)));
        activeActionKeyOverride.clear();
    }

    QMap<QString, QSharedPointer<MKeyOverride> >::iterator actionKeyOverrideIter(overrides.find(actionKeyName));

    if (actionKeyOverrideIter != overrides.end()) {
        QSharedPointer<MKeyOverride> overrideShared = *actionKeyOverrideIter;

        if (overrideShared) {
            connect(overrideShared.data(), SIGNAL(keyAttributesChanged(const QString &, const MKeyOverride::KeyOverrideAttributes)),
                    this, SLOT(onKeyAttributesChanged(const QString &, const MKeyOverride::KeyOverrideAttributes)));
            activeActionKeyOverride = overrideShared;
        }
    }

    updateActionKey(MKeyOverride::All);
}

void OverrideInputMethod::onKeyAttributesChanged(const QString &keyId,
                                                 const MKeyOverride::KeyOverrideAttributes changedAttributes)
{
    if (keyId == actionKeyName) {
        updateActionKey(changedAttributes);
    }
}

void OverrideInputMethod::updateActionKey (const MKeyOverride::KeyOverrideAttributes changedAttributes)
{
    const bool useKeyOverride(activeActionKeyOverride);

    if (changedAttributes & MKeyOverride::Label) {
        bool useDefault(false);

        if (useKeyOverride) {
            const QString label(activeActionKeyOverride->label());

            if (label.isEmpty()) {
                useDefault = true;
            } else {
                mainWidget->setText(label);
            }
        } else {
            useDefault = true;
        }

        if (useDefault) {
            mainWidget->setText(actionKeyLabel);
        }
    }
    if (changedAttributes & MKeyOverride::Icon) {
        // maybe later
    }
    if (changedAttributes & MKeyOverride::Highlighted) {
        // maybe later
    }
    if (changedAttributes & MKeyOverride::Enabled) {
        // maybe later
    }
}
