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

#ifndef HELLO_WORLD_INPUT_METHOD_H
#define HELLO_WORLD_INPUT_METHOD_H

#include <maliit/plugins/abstractinputmethod.h>

#include <QSet>
#include <QWidget>
#include <QPushButton>
#include <QLayout>

class HelloWorldInputMethod
    : public MAbstractInputMethod
{
    Q_OBJECT

public:
    HelloWorldInputMethod(MAbstractInputMethodHost *host,
                          QWidget *mainWindow);
    ~HelloWorldInputMethod();

    //! \reimp
    /* Mandatory */
    virtual void show();
    virtual void hide();
    virtual void handleVisualizationPriorityChange(bool priority);
    virtual void handleClientChange();
    virtual void handleAppOrientationAboutToChange(int angle);
    virtual void handleAppOrientationChanged(int angle);
    virtual QList<MInputMethodSubView> subViews(Maliit::HandlerState state
                                                = Maliit::OnScreen) const;
    virtual void setActiveSubView(const QString &subViewId,
                                  Maliit::HandlerState state = Maliit::OnScreen);
    virtual QString activeSubView(Maliit::HandlerState state = Maliit::OnScreen) const;
    virtual void setState(const QSet<Maliit::HandlerState> &state);

    /* Optional, often used */
    virtual void update();
    virtual void reset();
    virtual void switchContext(Maliit::SwitchDirection direction, bool enableAnimation);
    virtual void handleFocusChange(bool focusIn);
    virtual void setPreedit(const QString &preeditString, int cursorPos);

    /* Optional, rarely used */
    /* Using default implementations
    virtual bool imExtensionEvent(MImExtensionEvent *event);
    virtual void showLanguageNotification();
    virtual void setKeyOverrides(const QMap<QString, QSharedPointer<MKeyOverride> > &overrides);
    virtual void processKeyEvent(QEvent::Type keyType, Qt::Key keyCode,
                                 Qt::KeyboardModifiers modifiers, const QString &text,
                                 bool autoRepeat, int count, quint32 nativeScanCode,
                                 quint32 nativeModifiers, unsigned long time);
    virtual void setKeyOverrides(const QMap<QString, QSharedPointer<MKeyOverride> > &overrides);
    virtual void handleMouseClickOnPreedit(const QPoint &pos, const QRect &preeditRect);
    */
    //! \reimp_end

private Q_SLOTS:
    void handleButtonClicked();

private:
    QPushButton *mainWidget;
    bool showIsInhibited;
    bool showRequested;
};

#endif // HELLO_WORLD_INPUT_METHOD_H

