/* * This file is part of Maliit framework *
 *
 * Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#include "inputmethodquick.h"

#include "keyoverridequick.h"
#include "maliitquick.h"

#include <maliit/plugins/abstractinputmethodhost.h>
#include <maliit/plugins/keyoverride.h>

#include "abstractplatform.h"

#include <QtCore>
#include <QtGui>

#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickView>

namespace Maliit
{

namespace
{

const char * const actionKeyName = "actionKey";

QQuickView *createWindow(MAbstractInputMethodHost *host)
{
    QScopedPointer<QQuickView> view(new QQuickView);

    host->registerWindow(view.data(), Maliit::PositionCenterBottom);

    QSurfaceFormat format;
    format.setAlphaBufferSize(8);
    view->setFormat(format);
    view->setColor(QColor(Qt::transparent));

    return view.take();
}

} // unnamed namespace

class InputMethodQuickPrivate
{
    Q_DECLARE_PUBLIC(InputMethodQuick)

public:
    InputMethodQuick *const q_ptr;
    QScopedPointer<QQuickView> surface;
    QRect inputMethodArea;
    int appOrientation;
    bool haveFocus;

    //! current active state
    Maliit::HandlerState activeState;

    //! In practice show() and hide() correspond to application SIP (close)
    //! requests.  We track the current shown/SIP requested state using these variables.
    bool sipRequested;
    bool sipIsInhibited;
    QSharedPointer<KeyOverrideQuick> actionKeyOverride;
    QSharedPointer<MKeyOverride> sentActionKeyOverride;
    bool active;

    bool m_surroundingTextValid;
    QString m_surroundingText;
    int m_cursorPosition;
    int m_anchorPosition;
    bool m_hasSelection;
    int m_contentType;
    bool m_predictionEnabled;
    bool m_autoCapitalizationEnabled;
    bool m_hiddenText;
    QSharedPointer<Maliit::AbstractPlatform> m_platform;

    InputMethodQuickPrivate(MAbstractInputMethodHost *host,
                            InputMethodQuick *im,
                            const QSharedPointer<Maliit::AbstractPlatform> &platform)
        : q_ptr(im)
        , surface(createWindow(host))
        , appOrientation(0)
        , haveFocus(false)
        , activeState(Maliit::OnScreen)
        , sipRequested(false)
        , sipIsInhibited(false)
        , actionKeyOverride(new KeyOverrideQuick())
        , sentActionKeyOverride()
        , active(false)
        , m_surroundingTextValid(false)
        , m_cursorPosition(-1)
        , m_anchorPosition(-1)
        , m_hasSelection(false)
        , m_contentType(MaliitQuick::FreeTextContentType)
        , m_predictionEnabled(true)
        , m_autoCapitalizationEnabled(true)
        , m_hiddenText(false)
        , m_platform(platform)
    {
        Q_ASSERT(surface);

        updateActionKey(MKeyOverride::All);
        surface->engine()->addImportPath(MALIIT_PLUGINS_DATA_DIR);
        surface->engine()->rootContext()->setContextProperty("MInputMethodQuick", im);
    }

    ~InputMethodQuickPrivate()
    {}

    void handleInputMethodAreaUpdate(MAbstractInputMethodHost *host,
                                     const QRegion &region)
    {
        if (not host) {
            return;
        }

        host->setInputMethodArea(region);
    }

    void updateActionKey(const MKeyOverride::KeyOverrideAttributes changedAttributes)
    {
        actionKeyOverride->applyOverride(sentActionKeyOverride, changedAttributes);
    }
};

InputMethodQuick::InputMethodQuick(MAbstractInputMethodHost *host,
                                   const QString &qmlFileName,
                                   const QSharedPointer<Maliit::AbstractPlatform> &platform)
    : MAbstractInputMethod(host)
    , d_ptr(new InputMethodQuickPrivate(host, this, platform))
{
    Q_D(InputMethodQuick);

    d->surface->setSource(QUrl::fromLocalFile(qmlFileName));
    
    propagateScreenSize();
}

InputMethodQuick::~InputMethodQuick()
{}

void InputMethodQuick::handleFocusChange(bool focusIn)
{
    Q_D(InputMethodQuick);
    d->haveFocus = focusIn;
    Q_EMIT focusTargetChanged(focusIn);
}

void InputMethodQuick::show()
{
    Q_D(InputMethodQuick);
    d->sipRequested = true;
    if (d->sipIsInhibited) {
        return;
    }

    handleAppOrientationChanged(d->appOrientation);
    
    if (d->activeState == Maliit::OnScreen) {
        d->surface->setGeometry(QRect(QPoint(), QGuiApplication::primaryScreen()->availableSize()));
        d->surface->show();
        setActive(true);
    }
}

void InputMethodQuick::hide()
{
    Q_D(InputMethodQuick);
    if (!d->sipRequested) {
        return;
    }
    d->sipRequested = false;
    setActive(false);
    d->surface->hide();
    const QRegion r;
    d->handleInputMethodAreaUpdate(inputMethodHost(), r);
}

void InputMethodQuick::update()
{
    Q_D(InputMethodQuick);

    bool emitSurroundingText = false;
    bool emitSurroundingTextValid = false;
    bool emitCursorPosition = false;
    bool emitAnchorPosition = false;
    bool emitSelection = false;
    bool emitContentType = false;
    bool emitAutoCapitalization = false;
    bool emitPredictionEnabled = false;
    bool emitHiddenText = false;

    QString newSurroundingText;
    int newCursorPosition = -1;
    inputMethodHost()->surroundingText(newSurroundingText, newCursorPosition);

    if (newSurroundingText != d->m_surroundingText) {
        d->m_surroundingText = newSurroundingText;
        emitSurroundingText = true;
    }

    bool newSurroundingTextValid = !newSurroundingText.isNull();
    if (newSurroundingTextValid != d->m_surroundingTextValid) {
        d->m_surroundingTextValid = newSurroundingTextValid;
        emitSurroundingTextValid = true;
    }

    if (newCursorPosition != d->m_cursorPosition) {
        d->m_cursorPosition = newCursorPosition;
        emitCursorPosition = true;
    }

    bool valid;
    int newAnchorPosition = inputMethodHost()->anchorPosition(valid);
    if (!valid) {
        newAnchorPosition = -1;
    }
    if (newAnchorPosition != d->m_anchorPosition) {
        d->m_anchorPosition = newAnchorPosition;
        emitAnchorPosition = true;
    }

    bool newHasSelection = inputMethodHost()->hasSelection(valid);
    if (!valid) {
        newHasSelection = false;
    }
    if (newHasSelection != d->m_hasSelection) {
        d->m_hasSelection = newHasSelection;
        emitSelection = true;
    }

    int newContentType = inputMethodHost()->contentType(valid);
    if (!valid) {
        newContentType = MaliitQuick::FreeTextContentType;
    }

    if (newContentType != d->m_contentType) {
        d->m_contentType = newContentType;
        emitContentType = true;
    }

    bool newAutoCapitalizationEnabled = inputMethodHost()->autoCapitalizationEnabled(valid);
    if (!valid) {
        newAutoCapitalizationEnabled = true;
    }
    if (newAutoCapitalizationEnabled != d->m_autoCapitalizationEnabled) {
        d->m_autoCapitalizationEnabled = newAutoCapitalizationEnabled;
        emitAutoCapitalization = true;
    }

    bool newPredictionEnabled = inputMethodHost()->predictionEnabled(valid);
    if (!valid) {
        newPredictionEnabled = true;
    }
    if (newPredictionEnabled != d->m_predictionEnabled) {
        d->m_predictionEnabled = newPredictionEnabled;
        emitPredictionEnabled = true;
    }

    bool newHiddenText = inputMethodHost()->hiddenText(valid);
    if (!valid) {
        newHiddenText = false;
    }
    if (newHiddenText != d->m_hiddenText) {
        d->m_hiddenText = newHiddenText;
        emitHiddenText = true;
    }

    if (emitSurroundingText) {
        Q_EMIT surroundingTextChanged();
    }
    if (emitSurroundingTextValid) {
        Q_EMIT surroundingTextValidChanged();
    }
    if (emitCursorPosition) {
        Q_EMIT cursorPositionChanged();
    }
    if (emitAnchorPosition) {
        Q_EMIT anchorPositionChanged();
    }
    if (emitSelection) {
        Q_EMIT hasSelectionChanged();
    }
    if (emitContentType) {
        Q_EMIT contentTypeChanged();
    }
    if (emitAutoCapitalization) {
        Q_EMIT autoCapitalizationChanged();
    }
    if (emitPredictionEnabled) {
        Q_EMIT predictionEnabledChanged();
    }
    if (emitHiddenText) {
        Q_EMIT hiddenTextChanged();
    }

    Q_EMIT editorStateUpdate();
}

void InputMethodQuick::reset()
{
    Q_EMIT inputMethodReset();
}

void InputMethodQuick::handleAppOrientationChanged(int angle)
{
    Q_D(InputMethodQuick);

    MAbstractInputMethod::handleAppOrientationChanged(angle);

    if (d->appOrientation != angle) {

        d->appOrientation = angle;
        // When emitted, QML Plugin will realice a state
        // change and update InputMethodArea. Don't propagate those changes except if
        // VkB is currently showed
        Q_EMIT appOrientationChanged(d->appOrientation);
        if (d->sipRequested && !d->sipIsInhibited) {
            d->handleInputMethodAreaUpdate(inputMethodHost(), inputMethodArea().toRect());
        }
    }
}

void InputMethodQuick::setState(const QSet<Maliit::HandlerState> &state)
{
    Q_D(InputMethodQuick);

    if (state.isEmpty()) {
        return;
    }

    if (state.contains(Maliit::OnScreen)) {
        d->activeState = Maliit::OnScreen;
        if (d->sipRequested && !d->sipIsInhibited) {
            show(); // Force reparent of client widgets.
        }
    } else {
        setActive(false);
        // Allow client to make use of InputMethodArea
        const QRegion r;
        d->handleInputMethodAreaUpdate(inputMethodHost(), r);
        d->activeState = *state.begin();
    }
}

void InputMethodQuick::handleClientChange()
{
    Q_D(InputMethodQuick);

    if (d->sipRequested) {
        setActive(false);
    }
}

void InputMethodQuick::handleVisualizationPriorityChange(bool inhibitShow)
{
    Q_D(InputMethodQuick);

    if (d->sipIsInhibited == inhibitShow) {
        return;
    }
    d->sipIsInhibited = inhibitShow;

    if (d->sipRequested) {
        if (inhibitShow) {
            setActive(false);
        } else {
            setActive(true);
        }
    }
}


void InputMethodQuick::propagateScreenSize()
{
    const QSize screenSize(QGuiApplication::primaryScreen()->availableSize());

    Q_EMIT screenWidthChanged(screenSize.width());
    Q_EMIT screenHeightChanged(screenSize.height());
}

int InputMethodQuick::screenHeight() const
{
    return QGuiApplication::primaryScreen()->availableSize().height();
}

int InputMethodQuick::screenWidth() const
{
    return QGuiApplication::primaryScreen()->availableSize().width();
}

int InputMethodQuick::appOrientation() const
{
    Q_D(const InputMethodQuick);
    return d->appOrientation;
}

QRectF InputMethodQuick::inputMethodArea() const
{
    Q_D(const InputMethodQuick);
    return d->inputMethodArea;
}

void InputMethodQuick::setInputMethodArea(const QRectF &area)
{
    Q_D(InputMethodQuick);

    if (d->inputMethodArea != area.toRect()) {
        d->inputMethodArea = area.toRect();
        d->handleInputMethodAreaUpdate(inputMethodHost(), d->inputMethodArea);

        Q_EMIT inputMethodAreaChanged(d->inputMethodArea);
    }
}

void InputMethodQuick::setScreenRegion(const QRect &region)
{
    Q_D(InputMethodQuick);
    d->m_platform->setInputRegion(d->surface.data(), region);
}

void InputMethodQuick::sendPreedit(const QString &text,
                                   const QVariant &preeditFormats,
                                   int replacementStart, int replacementLength, int cursorPos)
{
    QList<Maliit::PreeditTextFormat> formatList;

    if (!preeditFormats.isValid()) {
        // Fallback
        formatList.append(Maliit::PreeditTextFormat(0, text.length(), Maliit::PreeditDefault));

    } else if (preeditFormats.type() == QVariant::Int) {
        // format with one type
        Maliit::PreeditTextFormat format(0, text.length(),
                                         static_cast<Maliit::PreeditFace>(preeditFormats.toInt()));
        formatList.append(format);

    } else if (preeditFormats.type() == QVariant::List) {
        // formatting as list of three ints: type, start, length
        QVariantList list = preeditFormats.toList();

        for (int i = 0; i < list.size(); ++i) {
            QVariantList formatTuple = list.at(i).toList();
            Maliit::PreeditFace face = Maliit::PreeditDefault;
            int start = 0;
            int length = 0;

            if (formatTuple.length() < 3) {
                qWarning() << "MInputMethodQuick.sendPreedit() got formatting tuple with less than three parameters";
                continue;
            }

            face = static_cast<Maliit::PreeditFace>(formatTuple.at(0).toInt());
            start = qBound(0, formatTuple.at(1).toInt(), text.length());
            length = qBound(0, formatTuple.at(2).toInt(), text.length() - start);

            formatList.append(Maliit::PreeditTextFormat(start, length, face));
        }
    }

    inputMethodHost()->sendPreeditString(text, formatList, replacementStart, replacementLength, cursorPos);
}

void InputMethodQuick::sendKey(int key, int modifiers, const QString &text, int type)
{
    if (type == MaliitQuick::KeyPress || type == MaliitQuick::KeyClick) {
        QKeyEvent event(QEvent::KeyPress, key, (~(Qt::KeyboardModifiers(Qt::NoModifier))) & modifiers, text);
        inputMethodHost()->sendKeyEvent(event);
    }

    if (type == MaliitQuick::KeyRelease || type == MaliitQuick::KeyClick) {
        QKeyEvent event(QEvent::KeyRelease, key, (~(Qt::KeyboardModifiers(Qt::NoModifier))) & modifiers, text);
        inputMethodHost()->sendKeyEvent(event);
    }
}

void InputMethodQuick::sendCommit(const QString &text, int replaceStart, int replaceLength, int cursorPos)
{
    if (text == "\b") {
        QKeyEvent event(QEvent::KeyPress, Qt::Key_Backspace, Qt::NoModifier);
        inputMethodHost()->sendKeyEvent(event);
    } else
    if ((text == "\r\n") || (text == "\n") || (text == "\r")) {
        QKeyEvent event(QEvent::KeyPress, Qt::Key_Return, Qt::NoModifier);
        inputMethodHost()->sendKeyEvent(event);
    } else {
        inputMethodHost()->sendCommitString(text, replaceStart, replaceLength, cursorPos);
    }
}

void InputMethodQuick::pluginSwitchRequired(int switchDirection)
{
    inputMethodHost()->switchPlugin(
        static_cast<Maliit::SwitchDirection>(switchDirection));
}

void InputMethodQuick::userHide()
{
    hide();
    inputMethodHost()->notifyImInitiatedHiding();
}

void InputMethodQuick::setKeyOverrides(const QMap<QString, QSharedPointer<MKeyOverride> > &overrides)
{
    Q_D(InputMethodQuick);
    const QMap<QString, QSharedPointer<MKeyOverride> >::const_iterator iter(overrides.find(actionKeyName));

    if (d->sentActionKeyOverride) {
        disconnect(d->sentActionKeyOverride.data(), SIGNAL(keyAttributesChanged(const QString &, const MKeyOverride::KeyOverrideAttributes)),
                   this, SLOT(onSentActionKeyAttributesChanged(const QString &, const MKeyOverride::KeyOverrideAttributes)));
        d->sentActionKeyOverride.clear();
    }

    if (iter != overrides.end()) {
        QSharedPointer<MKeyOverride> sentActionKeyOverride(*iter);

        if (sentActionKeyOverride) {
            d->sentActionKeyOverride = sentActionKeyOverride;
            connect(d->sentActionKeyOverride.data(), SIGNAL(keyAttributesChanged(const QString &, const MKeyOverride::KeyOverrideAttributes)),
                    this, SLOT(onSentActionKeyAttributesChanged(const QString &, const MKeyOverride::KeyOverrideAttributes)));
        }
    }
    d->updateActionKey(MKeyOverride::All);
}

QList<MAbstractInputMethod::MInputMethodSubView> InputMethodQuick::subViews(Maliit::HandlerState state) const
{
    Q_UNUSED(state);
    MAbstractInputMethod::MInputMethodSubView sub_view;
    sub_view.subViewId = "";
    sub_view.subViewTitle = "";
    QList<MAbstractInputMethod::MInputMethodSubView> sub_views;
    sub_views << sub_view;
    return sub_views;
}

void InputMethodQuick::onSentActionKeyAttributesChanged(const QString &, const MKeyOverride::KeyOverrideAttributes changedAttributes)
{
    Q_D(InputMethodQuick);

    d->updateActionKey(changedAttributes);
}

KeyOverrideQuick* InputMethodQuick::actionKeyOverride() const
{
    Q_D(const InputMethodQuick);

    return d->actionKeyOverride.data();
}

void InputMethodQuick::activateActionKey()
{
    sendKey(Qt::Key_Return, 0, "\r", MaliitQuick::KeyClick);
}

bool InputMethodQuick::isActive() const
{
    Q_D(const InputMethodQuick);
    return d->active;
}

void InputMethodQuick::setActive(bool enable)
{
    Q_D(InputMethodQuick);
    if (d->active != enable) {
        d->active = enable;
        Q_EMIT activeChanged();
    }
}

bool InputMethodQuick::surroundingTextValid()
{
    Q_D(InputMethodQuick);
    return d->m_surroundingTextValid;
}

QString InputMethodQuick::surroundingText()
{
    // Note: fetching value instead of using member variable for allowing connection side to
    // modify text when sending commit.
    QString text;
    int position;
    inputMethodHost()->surroundingText(text, position);
    return text;
}

int InputMethodQuick::cursorPosition()
{
    // see ::surroundingText()
    QString text;
    int position;
    inputMethodHost()->surroundingText(text, position);
    return position;
}

int InputMethodQuick::anchorPosition()
{
    Q_D(InputMethodQuick);
    return d->m_anchorPosition;
}

bool InputMethodQuick::hasSelection()
{
    Q_D(InputMethodQuick);
    return d->m_hasSelection;
}

int InputMethodQuick::contentType()
{
    Q_D(InputMethodQuick);
    return d->m_contentType;
}

bool InputMethodQuick::predictionEnabled()
{
    Q_D(InputMethodQuick);
    return d->m_predictionEnabled;
}

bool InputMethodQuick::autoCapitalizationEnabled()
{
    Q_D(InputMethodQuick);
    return d->m_autoCapitalizationEnabled;
}

bool InputMethodQuick::hiddenText()
{
    Q_D(InputMethodQuick);
    return d->m_hiddenText;
}

} // namespace Maliit
