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

#ifndef MINPUTCONTEXT_H
#define MINPUTCONTEXT_H

#include <maliit/namespace.h>
#include "dbusserverconnection.h"

#include <QObject>
#include <QTimer>
#include <QPointer>
#include <QRect>

#include <qpa/qplatforminputcontext.h>

class MImServerConnection;

class MInputContext : public QPlatformInputContext
{
    Q_OBJECT
    // Exposing preedit state as an extension. Use only if you know what you're doing.
    Q_PROPERTY(QString preedit READ preeditString NOTIFY preeditChanged)

public:
    enum OrientationAngle {
        Angle0   =   0,
        Angle90  =  90,
        Angle180 = 180,
        Angle270 = 270
    };

    MInputContext();
    virtual ~MInputContext();

    // reimplemented methods
    virtual bool isValid() const;
    virtual void reset();
    virtual void commit();
    virtual void update(Qt::InputMethodQueries);
    virtual void invokeAction(QInputMethod::Action, int cursorPosition);
    virtual bool filterEvent(const QEvent *event);
    virtual QRectF keyboardRect() const;
    virtual bool isAnimating() const;
    virtual void showInputPanel();
    virtual void hideInputPanel();
    virtual bool isInputPanelVisible() const;
    virtual QLocale locale() const;
    virtual Qt::LayoutDirection inputDirection() const;
    virtual void setFocusObject(QObject *object);

    QString preeditString();

public Q_SLOTS:
    // Hooked up to the input method server
    void activationLostEvent();
    void imInitiatedHide();

    void commitString(const QString &string, int replacementStart = 0,
                      int replacementLength = 0, int cursorPos = -1);

    void updatePreedit(const QString &string, const QList<Maliit::PreeditTextFormat> &preeditFormats,
                       int replacementStart = 0, int replacementLength = 0, int cursorPos = -1);

    void keyEvent(int type, int key, int modifiers, const QString &text, bool autoRepeat,
                  int count, Maliit::EventRequestType requestType = Maliit::EventRequestBoth);

    void updateInputMethodArea(const QRect &rect);
    void setGlobalCorrectionEnabled(bool);
    void getPreeditRectangle(QRect &rectangle, bool &valid) const;
    void onInvokeAction(const QString &action, const QKeySequence &sequence);
    void setRedirectKeys(bool enabled);
    void setDetectableAutoRepeat(bool enabled);
    void setSelection(int start, int length);
    void getSelection(QString &selection, bool &valid) const;
    void setLanguage(const QString &language);
    // End input method server connection slots.

private Q_SLOTS:
    void sendHideInputMethod();
    void updateServerOrientation(Qt::ScreenOrientation orientation);

    void onDBusDisconnection();
    void onDBusConnection();

    // Notify input method plugin about the application's active window prepare to change to a new orientation angle.
    void notifyOrientationAboutToChange(MInputContext::OrientationAngle orientation);

    // Notify input method plugin about new \a orientation angle of application's active window.
    // note: this method is called when the orientation change is finished
    void notifyOrientationChanged(MInputContext::OrientationAngle orientation);

Q_SIGNALS:
    void preeditChanged();

private:
    Q_DISABLE_COPY(MInputContext)

    enum InputPanelState {
        InputPanelShowPending,   // input panel showing requested, but activation pending
        InputPanelShown,
        InputPanelHidden
    };

    void updatePreeditInternally(const QString &string, const QList<Maliit::PreeditTextFormat> &preeditFormats,
                                 int replacementStart = 0, int replacementLength = 0, int cursorPos = -1);

    void connectInputMethodServer();

    void updateInputMethodExtensions();

    // returns content type corresponding to specified hints
    Maliit::TextContentType contentType(Qt::InputMethodHints hints) const;

    // returns state for currently focused widget, key is attribute name.
    QMap<QString, QVariant> getStateInformation() const;

    // Gets cursor start position, relative to widget surrounding text.
    // Parameter valid set to false on failure.
    int cursorStartPosition(bool *valid);

    DBusServerConnection *imServer;
    bool active; // is connection active
    QPointer<QWindow> window;
    QRect keyboardRectangle;
    InputPanelState inputPanelState; // state for the input method server's software input panel

    /* Timer for hiding the current Software Input Panel.
     *  This is mainly for switching directly between widgets that have input method enabled. */
    QTimer sipHideTimer;
    QString preedit;
    int preeditCursorPos;
    bool redirectKeys; // redirect all hw key events to the input method or not
    QLocale inputLocale;
    bool currentFocusAcceptsInput;
    QPlatformInputContext *composeInputContext;
};

#endif
