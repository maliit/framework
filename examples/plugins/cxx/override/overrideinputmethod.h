/* * This file is part of Maliit framework *
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 *
 * Contact: maliit-discuss@lists.maliit.org
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 */

#ifndef OVERRIDE_INPUT_METHOD_H
#define OVERRIDE_INPUT_METHOD_H

#include <maliit/plugins/abstractinputmethod.h>
#include <maliit/plugins/keyoverride.h>

#include <QSet>
#include <QWidget>
#include <QPushButton>
#include <QLayout>

class OverrideInputMethod
    : public MAbstractInputMethod
{
    Q_OBJECT

public:
    OverrideInputMethod(MAbstractInputMethodHost *host);
    ~OverrideInputMethod();

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
    virtual void switchContext(Maliit::SwitchDirection direction,
                               bool enableAnimation);
    virtual void handleFocusChange(bool focusIn);
    virtual void setPreedit(const QString &preeditString,
                            int cursorPos);
    virtual void setKeyOverrides(const QMap<QString, QSharedPointer<MKeyOverride> > &overrides);

    /* Optional, rarely used */
    /* Using default implementations
    virtual bool imExtensionEvent(MImExtensionEvent *event);
    virtual void showLanguageNotification();
    virtual void processKeyEvent(QEvent::Type keyType, Qt::Key keyCode,
                                 Qt::KeyboardModifiers modifiers, const QString &text,
                                 bool autoRepeat, int count, quint32 nativeScanCode,
                                 quint32 nativeModifiers, unsigned long time);
    virtual void handleMouseClickOnPreedit(const QPoint &pos, const QRect &preeditRect);
    */
    //! \reimp_end

private Q_SLOTS:
    void handleButtonClicked();
    void onKeyAttributesChanged(const QString &keyId,
                                const MKeyOverride::KeyOverrideAttributes changedAttributes);

private:
    void updateActionKey(const MKeyOverride::KeyOverrideAttributes changedAttributes);

    QScopedPointer<QWidget> surface;
    QPushButton *mainWidget;
    bool showIsInhibited;
    bool showRequested;
    QSharedPointer<MKeyOverride> activeActionKeyOverride;
};

#endif // OVERRIDE_INPUT_METHOD_H

