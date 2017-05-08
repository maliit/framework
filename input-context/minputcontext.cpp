/* * This file is part of Maliit framework *
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
 * Copyright (C) 2013 Jolla Ltd.
 *
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


#include "minputcontext.h"

#include <QGuiApplication>
#include <QScreen>
#include <QKeyEvent>
#include <QTextFormat>
#include <QDebug>
#include <QByteArray>
#include <QRectF>
#include <QLocale>
#include <QWindow>
#include <QSharedDataPointer>
#include <QQuickItem>

namespace
{
    const int SoftwareInputPanelHideTimer = 100;
    const char * const InputContextName = "MInputContext";

    int orientationAngle(Qt::ScreenOrientation orientation)
    {
        // Maliit uses orientations relative to screen, Qt relative to world
        // Note: doesn't work with inverted portrait or landscape as native screen orientation.
        static bool portraitRotated = qGuiApp->primaryScreen()->primaryOrientation() == Qt::PortraitOrientation;

        switch (orientation) {
        case Qt::PrimaryOrientation: // Urgh.
        case Qt::PortraitOrientation:
            return portraitRotated ? MInputContext::Angle0 : MInputContext::Angle270;
        case Qt::LandscapeOrientation:
            return portraitRotated ? MInputContext::Angle90 : MInputContext::Angle0;
        case Qt::InvertedPortraitOrientation:
            return portraitRotated ? MInputContext::Angle180 : MInputContext::Angle90;
        case Qt::InvertedLandscapeOrientation:
            return portraitRotated ? MInputContext::Angle270 : MInputContext::Angle180;
        }
        return MInputContext::Angle0;
    }
}

bool MInputContext::debug = false;


MInputContext::MInputContext()
    : imServer(0),
      active(false),
      inputPanelState(InputPanelHidden),
      preeditCursorPos(-1),
      redirectKeys(false),
      currentFocusAcceptsInput(false)
{
    QByteArray debugEnvVar = qgetenv("MALIIT_DEBUG");
    if (!debugEnvVar.isEmpty() && debugEnvVar != "0") {
        qDebug() << "Creating Maliit input context";
        debug = true;
    }

    QSharedPointer<Maliit::InputContext::DBus::Address> address;

    QByteArray maliitServerAddress = qgetenv("MALIIT_SERVER_ADDRESS");
    if (!maliitServerAddress.isEmpty()) {
        address.reset(new Maliit::InputContext::DBus::FixedAddress(maliitServerAddress.constData()));
    } else {
        address.reset(new Maliit::InputContext::DBus::DynamicAddress);
    }

    imServer = new DBusServerConnection(address);
    
    sipHideTimer.setSingleShot(true);
    sipHideTimer.setInterval(SoftwareInputPanelHideTimer);
    connect(&sipHideTimer, SIGNAL(timeout()), SLOT(sendHideInputMethod()));

    connectInputMethodServer();
}

MInputContext::~MInputContext()
{
    delete imServer;
}

void MInputContext::connectInputMethodServer()
{
    connect(imServer, SIGNAL(connected()), this, SLOT(onDBusConnection()));
    connect(imServer, SIGNAL(disconnected()), this, SLOT(onDBusDisconnection()));

    // Hook up incoming communication from input method server
    connect(imServer, SIGNAL(activationLostEvent()), this, SLOT(activationLostEvent()));

    connect(imServer, SIGNAL(imInitiatedHide()), this, SLOT(imInitiatedHide()));

    connect(imServer, SIGNAL(commitString(QString,int,int,int)),
            this, SLOT(commitString(QString,int,int,int)));

    connect(imServer, SIGNAL(updatePreedit(QString,QList<Maliit::PreeditTextFormat>,int,int,int)),
            this, SLOT(updatePreedit(QString,QList<Maliit::PreeditTextFormat>,int,int,int)));

    connect(imServer, SIGNAL(keyEvent(int,int,int,QString,bool,int,Maliit::EventRequestType)),
            this, SLOT(keyEvent(int,int,int,QString,bool,int,Maliit::EventRequestType)));

    connect(imServer, SIGNAL(updateInputMethodArea(QRect)),
            this, SLOT(updateInputMethodArea(QRect)));

    connect(imServer, SIGNAL(setGlobalCorrectionEnabled(bool)),
            this, SLOT(setGlobalCorrectionEnabled(bool)));

    connect(imServer, SIGNAL(getPreeditRectangle(QRect&,bool&)),
            this, SLOT(getPreeditRectangle(QRect&,bool&)));

    connect(imServer, SIGNAL(invokeAction(QString,QKeySequence)), this, SLOT(onInvokeAction(QString,QKeySequence)));

    connect(imServer, SIGNAL(setRedirectKeys(bool)), this, SLOT(setRedirectKeys(bool)));

    connect(imServer, SIGNAL(setDetectableAutoRepeat(bool)),
            this, SLOT(setDetectableAutoRepeat(bool)));

    connect(imServer, SIGNAL(setSelection(int,int)),
            this, SLOT(setSelection(int,int)));

    connect(imServer, SIGNAL(getSelection(QString&,bool&)),
            this, SLOT(getSelection(QString&, bool&)));

    connect(imServer, SIGNAL(setLanguage(QString)),
            this, SLOT(setLanguage(QString)));
}


bool MInputContext::isValid() const
{
    return true;
}

void MInputContext::setLanguage(const QString &language)
{
    QLocale newLocale(language);
    Qt::LayoutDirection oldDirection = inputDirection();

    if (newLocale != inputLocale) {
        inputLocale = newLocale;
        emitLocaleChanged();
    }

    Qt::LayoutDirection newDirection = inputDirection();
    if (newDirection != oldDirection) {
        emitInputDirectionChanged(newDirection);
    }
}

void MInputContext::reset()
{
    if (debug) qDebug() << InputContextName << "in" << __PRETTY_FUNCTION__;

    const bool hadPreedit = !preedit.isEmpty();
    preedit.clear();
    preeditCursorPos = -1;

    // reset input method server, preedit requires synchronization.
    // rationale: input method might be autocommitting existing preedit without
    // user interaction.
    imServer->reset(hadPreedit);
}

void MInputContext::commit()
{
    if (debug) qDebug() << InputContextName << "in" << __PRETTY_FUNCTION__;

    const bool hadPreedit = !preedit.isEmpty();

    if (hadPreedit) {
        QList<QInputMethodEvent::Attribute> attributes;
        if (preeditCursorPos >= 0) {
            bool valid = false;
            int start = cursorStartPosition(&valid);
            if (valid) {
                attributes << QInputMethodEvent::Attribute(QInputMethodEvent::Selection,
                                                           start + preeditCursorPos, 0, QVariant());
            }
        }

        QInputMethodEvent event("", attributes);
        event.setCommitString(preedit);
        if (qGuiApp->focusObject()) {
            QGuiApplication::sendEvent(qGuiApp->focusObject(), &event);
        }

        preedit.clear();
        preeditCursorPos = -1;
        Q_EMIT preeditChanged();
    }

    imServer->reset(hadPreedit);
}

void MInputContext::invokeAction(QInputMethod::Action action, int x)
{
    if (debug) qDebug() << InputContextName << "in" << __PRETTY_FUNCTION__;

    if (!inputMethodAccepted())
        return;

    if (action == QInputMethod::Click) {
        if (x < 0 || x >= preedit.length()) {
            reset();
            return;
        }

        // To preserve the wire protocol, the "x" argument gets
        // transferred in widget state instead of being an extra
        // argument to mouseClickedOnPreedit().
        QMap<QString, QVariant> stateInformation = getStateInformation();
        stateInformation["preeditClickPos"] = x;
        imServer->updateWidgetInformation(stateInformation, false);

        // FIXME: proper positions and preeditClickPos
        QRect preeditRect;
        QPoint globalPos;
        imServer->mouseClickedOnPreedit(globalPos, preeditRect);

    } else {
        QPlatformInputContext::invokeAction(action, x);
    }
}

void MInputContext::update(Qt::InputMethodQueries queries)
{
    if (debug) qDebug() << InputContextName << "in" << __PRETTY_FUNCTION__;

    Q_UNUSED(queries) // fetching everything

    if (queries & Qt::ImPlatformData) {
        updateInputMethodExtensions();
    }

    bool effectiveFocusChange = false;
    if (queries & Qt::ImEnabled) {
        bool newAcceptance = inputMethodAccepted();
        if (newAcceptance && !active) {
            setFocusObject(QGuiApplication::focusObject());
            return;
        }

        if (newAcceptance != currentFocusAcceptsInput) {
            currentFocusAcceptsInput = newAcceptance;
            effectiveFocusChange = true;
        }
    }
    // get the state information of currently focused widget, and pass it to input method server
    QMap<QString, QVariant> stateInformation = getStateInformation();
    imServer->updateWidgetInformation(stateInformation, effectiveFocusChange);
}

void MInputContext::updateServerOrientation(Qt::ScreenOrientation orientation)
{
    if (active) {
        imServer->appOrientationChanged(orientationAngle(orientation));
    }
}


void MInputContext::setFocusObject(QObject *focused)
{
    if (debug) qDebug() << InputContextName << "in" << __PRETTY_FUNCTION__ << focused;

    updateInputMethodExtensions();

    QWindow *newFocusWindow = qGuiApp->focusWindow();
    if (newFocusWindow != window.data()) {
       if (window) {
           disconnect(window.data(), SIGNAL(contentOrientationChanged(Qt::ScreenOrientation)),
                      this, SLOT(updateServerOrientation(Qt::ScreenOrientation)));
       }

       window = newFocusWindow;
       if (window) {
           connect(window.data(), SIGNAL(contentOrientationChanged(Qt::ScreenOrientation)),
                   this, SLOT(updateServerOrientation(Qt::ScreenOrientation)));
           updateServerOrientation(window->contentOrientation());
       }
    }

    bool oldAcceptInput = currentFocusAcceptsInput;
    currentFocusAcceptsInput = inputMethodAccepted();

    if (!active && currentFocusAcceptsInput) {
        imServer->activateContext();
        active = true;
        updateServerOrientation(newFocusWindow->contentOrientation());
    }

    if (active && (currentFocusAcceptsInput || oldAcceptInput)) {
        const QMap<QString, QVariant> stateInformation = getStateInformation();
        imServer->updateWidgetInformation(stateInformation, true);
    }

    if (inputPanelState == InputPanelShowPending && currentFocusAcceptsInput) {
        sipHideTimer.stop();
        imServer->showInputMethod();
        inputPanelState = InputPanelShown;
    }
}

QString MInputContext::preeditString()
{
    return preedit;
}

bool MInputContext::filterEvent(const QEvent *event)
{
    bool eaten = false;

    switch (event->type()) {

    case QEvent::KeyPress:
    case QEvent::KeyRelease:
        if (!inputMethodAccepted()) {
            break;
        }

        if (redirectKeys) {
            const QKeyEvent *key = static_cast<const QKeyEvent *>(event);
            imServer->processKeyEvent(key->type(), static_cast<Qt::Key>(key->key()),
                                      key->modifiers(), key->text(), key->isAutoRepeat(),
                                      key->count(), key->nativeScanCode(),
                                      key->nativeModifiers(), 0 /* currentKeyEventTime */);
            eaten = true;
        }
        break;

    default:
        break;
    }

    return eaten;
}

QRectF MInputContext::keyboardRect() const
{
    return keyboardRectangle;
}

bool MInputContext::isAnimating() const
{
    return false; // don't know here when input method server is actually doing transitions
}

void MInputContext::showInputPanel()
{
    if (debug) qDebug() << __PRETTY_FUNCTION__;

    if (inputMethodAccepted()) {
        sipHideTimer.stop();
    }

    if (!active || !inputMethodAccepted()) {
        // in case SIP request comes without a properly focused widget, we
        // don't ask input method server to be shown. It's done when the next widget
        // is focused, so the widget state information can be set.
        inputPanelState = InputPanelShowPending;

    } else {
        // note: could do this also if panel was hidden

        imServer->showInputMethod();
        inputPanelState = InputPanelShown;
    }
}

void MInputContext::hideInputPanel()
{
    if (debug) qDebug() << __PRETTY_FUNCTION__;
    sipHideTimer.start();
}

bool MInputContext::isInputPanelVisible() const
{
    return !keyboardRectangle.isEmpty();
}

QLocale MInputContext::locale() const
{
    return inputLocale;
}

Qt::LayoutDirection MInputContext::inputDirection() const
{
    return inputLocale.textDirection();
}

void MInputContext::sendHideInputMethod()
{
    imServer->hideInputMethod();

    inputPanelState = InputPanelHidden;
}

void MInputContext::activationLostEvent()
{
    // This method is called when activation was gracefully lost.
    // There is similar cleaning up done in onDBusDisconnection.
    active = false;
    inputPanelState = InputPanelHidden;
}


void MInputContext::imInitiatedHide()
{
    if (debug) qDebug() << InputContextName << "in" << __PRETTY_FUNCTION__;

    inputPanelState = InputPanelHidden;

    // remove focus on QtQuick2
    QQuickItem *inputItem = qobject_cast<QQuickItem*>(QGuiApplication::focusObject());
    if (inputItem) {
        inputItem->setFocus(false);
    }

}

void MInputContext::commitString(const QString &string, int replacementStart,
                                 int replacementLength, int cursorPos)
{
    if (debug) qDebug() << InputContextName << "in" << __PRETTY_FUNCTION__;

    if (imServer->pendingResets()) {
        return;
    }

    bool hadPreedit = !preedit.isEmpty();
    preedit.clear();
    preeditCursorPos = -1;

    int start = -1;
    if (cursorPos >= 0) {
        bool valid = false;
        int currentStart = cursorStartPosition(&valid);
        if (valid) {
            start = cursorPos + currentStart + replacementStart;
        }
    }

    if (start >= 0) {
        QList<QInputMethodEvent::Attribute> attributes;
        attributes << QInputMethodEvent::Attribute(QInputMethodEvent::Selection, start, 0, QVariant());
        QInputMethodEvent event("", attributes);
        event.setCommitString(string, replacementStart, replacementLength);
        if (qGuiApp->focusObject()) {
            QGuiApplication::sendEvent(qGuiApp->focusObject(), &event);
        }

    } else {
        QInputMethodEvent event;
        event.setCommitString(string, replacementStart, replacementLength);
        if (qGuiApp->focusObject()) {
            QGuiApplication::sendEvent(qGuiApp->focusObject(), &event);
        }
    }

    if (hadPreedit) {
        Q_EMIT preeditChanged();
    }
}


void MInputContext::updatePreedit(const QString &string,
                                  const QList<Maliit::PreeditTextFormat> &preeditFormats,
                                  int replacementStart, int replacementLength, int cursorPos)
{
    if (debug) {
        qDebug() << InputContextName << "in" << __PRETTY_FUNCTION__ << "preedit:" << string
                 << ", replacementStart:" << replacementStart
                 << ", replacementLength:" << replacementLength
                 << ", cursorPos:" << cursorPos;
    }

    if (imServer->pendingResets()) {
        return;
    }

    updatePreeditInternally(string, preeditFormats, replacementStart, replacementLength, cursorPos);
}

void MInputContext::updatePreeditInternally(const QString &string,
                                            const QList<Maliit::PreeditTextFormat> &preeditFormats,
                                            int replacementStart, int replacementLength, int cursorPos)
{
    preedit = string;
    preeditCursorPos = cursorPos;

    QList<QInputMethodEvent::Attribute> attributes;
    Q_FOREACH (const Maliit::PreeditTextFormat &preeditFormat, preeditFormats) {

        QTextCharFormat format;

        // update style mode
        switch (preeditFormat.preeditFace) {
        case Maliit::PreeditNoCandidates:
            format.setUnderlineStyle(QTextCharFormat::SpellCheckUnderline);
            format.setUnderlineColor(Qt::red);
            break;
        case Maliit::PreeditUnconvertible:
            format.setForeground(QBrush(QColor(128, 128, 128)));
            break;
        case Maliit::PreeditActive:
            format.setForeground(QBrush(QColor(153, 50, 204)));
            format.setFontWeight(QFont::Bold);
            break;
        case Maliit::PreeditKeyPress:
        case Maliit::PreeditDefault:
            format.setUnderlineStyle(QTextCharFormat::SingleUnderline);
            format.setUnderlineColor(QColor(0, 0, 0));
            break;
        }

        attributes << QInputMethodEvent::Attribute(QInputMethodEvent::TextFormat,
                                                   preeditFormat.start,
                                                   preeditFormat.length, format);
    }

    if (cursorPos >= 0) {
        attributes << QInputMethodEvent::Attribute(QInputMethodEvent::Cursor, cursorPos, 1, QVariant());
    }

    QInputMethodEvent event(string, attributes);
    if (replacementStart || replacementLength) {
        event.setCommitString("", replacementStart, replacementLength);
    }

    if (qGuiApp->focusObject()) {
        QGuiApplication::sendEvent(qGuiApp->focusObject(), &event);
    } else {
       if (debug) qDebug() << __PRETTY_FUNCTION__;
       qWarning() << "No focused object, cannot update preedit."
                  << "Wrong reset/preedit behaviour in active input method plugin?";
    }

    Q_EMIT preeditChanged();
}

void MInputContext::keyEvent(int type, int key, int modifiers, const QString &text,
                             bool autoRepeat, int count,
                             Maliit::EventRequestType requestType)
{
    if (debug) qDebug() << InputContextName << "in" << __PRETTY_FUNCTION__;

    if (qGuiApp->focusWindow() != 0 && requestType != Maliit::EventRequestSignalOnly) {
        QEvent::Type eventType = static_cast<QEvent::Type>(type);
        QKeyEvent event(eventType, key, static_cast<Qt::KeyboardModifiers>(modifiers), text, autoRepeat, count);
        // need window instead of focus item so e.g. QQuickItem::keyPressEvent() gets properly called
        QGuiApplication::sendEvent(qGuiApp->focusWindow(), &event);
    }
}


void MInputContext::updateInputMethodArea(const QRect &rect)
{
    bool wasVisible = isInputPanelVisible();

    if (rect != keyboardRectangle) {
        keyboardRectangle = rect;
        emitKeyboardRectChanged();

        if (wasVisible != isInputPanelVisible()) {
            emitInputPanelVisibleChanged();
        }
    }
}


void MInputContext::setGlobalCorrectionEnabled(bool enabled)
{
    Q_UNUSED(enabled)
    // not handling preedit injections, ignored
}


void MInputContext::getPreeditRectangle(QRect &rectangle, bool &valid) const
{
    // not supported
    rectangle = QRect();
    valid = false;

    return;
}

void MInputContext::onInvokeAction(const QString &action, const QKeySequence &sequence)
{
    if (debug) qDebug() << InputContextName << __PRETTY_FUNCTION__ << "action" << action;

    // NOTE: currently not trying to trigger action directly
    static const Qt::KeyboardModifiers AllModifiers = Qt::ShiftModifier | Qt::ControlModifier | Qt::AltModifier
            | Qt::MetaModifier | Qt::KeypadModifier;

    for (int i = 0; i < sequence.count(); i++) {
        const int key = sequence[i] & ~AllModifiers;
        const int modifiers = sequence[i] & AllModifiers;
        QString text("");
        if (modifiers == Qt::NoModifier || modifiers == Qt::ShiftModifier) {
            text = QString(key);
        }
        keyEvent(QEvent::KeyPress, key, modifiers, text, false, 1);
        keyEvent(QEvent::KeyRelease, key, modifiers, text, false, 1);
    }
}

void MInputContext::onDBusDisconnection()
{
    if (debug) qDebug() << __PRETTY_FUNCTION__;

    active = false;
    redirectKeys = false;

    updateInputMethodArea(QRect());
}

void MInputContext::onDBusConnection()
{
    if (debug) qDebug() << __PRETTY_FUNCTION__;

    // using one attribute extension for everything
    imServer->registerAttributeExtension(0, QString());

    // Force activation, since setFocusObject may have been called after
    // onDBusDisconnection set active to false or before the dbus connection.
    active = false;

    if (inputMethodAccepted()) {
        setFocusObject(QGuiApplication::focusObject());
        if (inputPanelState != InputPanelHidden) {
            imServer->showInputMethod();
            inputPanelState = InputPanelShown;
        }
    }
}

void MInputContext::notifyOrientationAboutToChange(MInputContext::OrientationAngle angle)
{
    // can get called from signal so cannot be sure we are really currently active
    if (active) {
        imServer->appOrientationAboutToChange(static_cast<int>(angle));
    }
}

void MInputContext::notifyOrientationChanged(MInputContext::OrientationAngle angle)
{
    // can get called from signal so cannot be sure we are really currently active
    if (active) {
        imServer->appOrientationChanged(static_cast<int>(angle));
    }
}

Maliit::TextContentType MInputContext::contentType(Qt::InputMethodHints hints) const
{
    Maliit::TextContentType type = Maliit::FreeTextContentType;
    hints &= Qt::ImhExclusiveInputMask;

    if (hints == Qt::ImhFormattedNumbersOnly || hints == Qt::ImhDigitsOnly) {
        type = Maliit::NumberContentType;
    } else if (hints == Qt::ImhDialableCharactersOnly) {
        type = Maliit::PhoneNumberContentType;
    } else if (hints == Qt::ImhEmailCharactersOnly) {
        type = Maliit::EmailContentType;
    } else if (hints == Qt::ImhUrlCharactersOnly) {
        type = Maliit::UrlContentType;
    }

    return type;
}

void MInputContext::setRedirectKeys(bool enabled)
{
    redirectKeys = enabled;
}

void MInputContext::setDetectableAutoRepeat(bool enabled)
{
    Q_UNUSED(enabled);
    if (debug) qWarning() << "Detectable autorepeat not supported.";
}

QMap<QString, QVariant> MInputContext::getStateInformation() const
{
    QMap<QString, QVariant> stateInformation;

    stateInformation["focusState"] = inputMethodAccepted();

    if (!inputMethodAccepted() || !qGuiApp->focusObject()) {
        return stateInformation;
    }

    QInputMethodQueryEvent query(Qt::ImQueryAll);
    QGuiApplication::sendEvent(qGuiApp->focusObject(), &query);

    QVariant queryResult;

    queryResult = query.value(Qt::ImSurroundingText);
    if (queryResult.isValid()) {
        stateInformation["surroundingText"] = queryResult.toString();
    }

    queryResult = query.value(Qt::ImCursorPosition);
    if (queryResult.isValid()) {
        stateInformation["cursorPosition"] = queryResult.toInt();
    }

    queryResult = query.value(Qt::ImAnchorPosition);
    if (queryResult.isValid()) {
        stateInformation["anchorPosition"] = queryResult.toInt();
    }

    queryResult = query.value(Qt::ImHints);
    auto hints = queryResult.value<std::underlying_type<Qt::InputMethodHint>::type>();

    // content type value
    // Deprecated, replaced by just transmitting all hints (see below):
    // FIXME: Remove once MAbstractInputMethod API for this got deprecated/removed.
    stateInformation["contentType"] = contentType(static_cast<Qt::InputMethodHint>(hints));

    stateInformation["autocapitalizationEnabled"] = !(hints & Qt::ImhNoAutoUppercase);
    stateInformation["hiddenText"] = static_cast<bool>(hints & Qt::ImhHiddenText);
    stateInformation["predictionEnabled"] = !(hints & Qt::ImhNoPredictiveText);

    stateInformation["maliit-inputmethod-hints"] = hints;

    queryResult = query.value(Qt::ImEnterKeyType);
    stateInformation["enterKeyType"] = queryResult.value<std::underlying_type<Qt::EnterKeyType>::type>();

    // is text selected
    queryResult = query.value(Qt::ImCurrentSelection);
    if (queryResult.isValid()) {
        stateInformation["hasSelection"] = !(queryResult.toString().isEmpty());
    }

    QWindow *window = qGuiApp->focusWindow();
    if (window) {
        stateInformation["winId"] = static_cast<qulonglong>(window->winId());
    }

    queryResult = query.value(Qt::ImCursorRectangle);
    if (queryResult.isValid()) {
        QRect rect = queryResult.toRect();
        rect = qGuiApp->inputMethod()->inputItemTransform().mapRect(rect);
        if (window) {
            stateInformation["cursorRectangle"] = QRect(window->mapToGlobal(rect.topLeft()), rect.size());
        }
    }

    stateInformation["toolbarId"] = 0; // Global extension id. And bad state parameter name for it.

    return stateInformation;
}

void MInputContext::setSelection(int start, int length)
{
    if (!inputMethodAccepted())
        return;

    QList<QInputMethodEvent::Attribute> attributes;
    attributes << QInputMethodEvent::Attribute(QInputMethodEvent::Selection, start,
                                                length, QVariant());
    QInputMethodEvent event("", attributes);
    QGuiApplication::sendEvent(qGuiApp->focusObject(), &event);
}

void MInputContext::getSelection(QString &selection, bool &valid) const
{
    selection.clear();

    QString selectionText;
    valid = false;

    if (!inputMethodAccepted()) {
        return;
    }

    QInputMethodQueryEvent query(Qt::ImCurrentSelection);
    QGuiApplication::sendEvent(qGuiApp->focusObject(), &query);

    QVariant queryResult = query.value(Qt::ImCurrentSelection);
    valid = queryResult.isValid();
    selectionText = queryResult.toString();

    selection = selectionText;
}

int MInputContext::cursorStartPosition(bool *valid)
{
    int start = -1;
    if (valid) {
        *valid = false;
    }

    if (!inputMethodAccepted()) {
        return start;
    }

    QInputMethodQueryEvent query(Qt::ImCursorPosition | Qt::ImAnchorPosition);
    QGuiApplication::sendEvent(qGuiApp->focusObject(), &query);

    // obtain the cursor absolute position
    QVariant queryResult = query.value(Qt::ImCursorPosition);
    if (queryResult.isValid()) {
        int absCursorPos = queryResult.toInt();

        // Fetch anchor position too but don't require it.
        queryResult = query.value(Qt::ImAnchorPosition);
        int absAnchorPos = queryResult.isValid() ? queryResult.toInt() : absCursorPos;

        // In case of selection, base cursorPos on start of it.
        start = qMin<int>(absCursorPos, absAnchorPos);
        *valid = true;
    }

    return start;
}

void MInputContext::updateInputMethodExtensions()
{
    if (!inputMethodAccepted()) {
        return;
    }
    if (debug) qDebug() << InputContextName << __PRETTY_FUNCTION__;

    QVariantMap extensions = qGuiApp->focusObject()->property("__inputMethodExtensions").toMap();
    QVariant value;
    value = extensions.value("enterKeyIconSource");
    imServer->setExtendedAttribute(0, "/keys", "actionKey", "icon", QVariant(value.toUrl().toString()));

    value = extensions.value("enterKeyText");
    imServer->setExtendedAttribute(0, "/keys", "actionKey", "label", QVariant(value.toString()));

    value = extensions.value("enterKeyEnabled");
    imServer->setExtendedAttribute(0, "/keys", "actionKey", "enabled", value.isValid() ? value.toBool() : true);

    value = extensions.value("enterKeyHighlighted");
    imServer->setExtendedAttribute(0, "/keys", "actionKey", "highlighted", value.isValid() ? value.toBool() : false);
}
