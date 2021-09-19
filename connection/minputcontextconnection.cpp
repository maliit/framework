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

#include "minputcontextconnection.h"

#include <QKeyEvent>

namespace {
    // attribute names for updateWidgetInformation() map
    const char * const FocusStateAttribute = "focusState";
    const char * const ContentTypeAttribute = "contentType";
    const char * const CorrectionAttribute = "correctionEnabled";
    const char * const PredictionAttribute = "predictionEnabled";
    const char * const AutoCapitalizationAttribute = "autocapitalizationEnabled";
    const char * const SurroundingTextAttribute = "surroundingText";
    const char * const AnchorPositionAttribute = "anchorPosition";
    const char * const CursorPositionAttribute = "cursorPosition";
    const char * const HasSelectionAttribute = "hasSelection";
    const char * const InputMethodModeAttribute = "inputMethodMode";
    const char * const WinId = "winId";
    const char * const CursorRectAttribute = "cursorRectangle";
    const char * const HiddenTextAttribute = "hiddenText";
    const char * const PreeditClickPosAttribute = "preeditClickPos";
}

class MInputContextConnectionPrivate
{
public:
    MInputContextConnectionPrivate();
    ~MInputContextConnectionPrivate();
};


MInputContextConnectionPrivate::MInputContextConnectionPrivate()
{
    // nothing
}


MInputContextConnectionPrivate::~MInputContextConnectionPrivate()
{
    // nothing
}


////////////////////////
// actual class

MInputContextConnection::MInputContextConnection(QObject *parent)
    : activeConnection(0)
    , d(new MInputContextConnectionPrivate)
    , lastOrientation(0)
    , mGlobalCorrectionEnabled(false)
    , mRedirectionEnabled(false)
    , mDetectableAutoRepeat(false)
{
    Q_UNUSED(parent);
}


MInputContextConnection::~MInputContextConnection()
{
    delete d;
}

/* Accessors to widgetState */
bool MInputContextConnection::focusState(bool &valid)
{
    QVariant focusStateVariant = mWidgetState[FocusStateAttribute];
    valid = focusStateVariant.isValid();
    return focusStateVariant.toBool();
}

int MInputContextConnection::contentType(bool &valid)
{
    QVariant contentTypeVariant = mWidgetState[ContentTypeAttribute];
    return contentTypeVariant.toInt(&valid);
}

bool MInputContextConnection::correctionEnabled(bool &valid)
{
    QVariant correctionVariant = mWidgetState[CorrectionAttribute];
    valid = correctionVariant.isValid();
    return correctionVariant.toBool();
}


bool MInputContextConnection::predictionEnabled(bool &valid)
{
    QVariant predictionVariant = mWidgetState[PredictionAttribute];
    valid = predictionVariant.isValid();
    return predictionVariant.toBool();
}

bool MInputContextConnection::autoCapitalizationEnabled(bool &valid)
{
    QVariant capitalizationVariant = mWidgetState[AutoCapitalizationAttribute];
    valid = capitalizationVariant.isValid();
    return capitalizationVariant.toBool();
}

QRect MInputContextConnection::cursorRectangle(bool &valid)
{
    QVariant cursorRectVariant = mWidgetState[CursorRectAttribute];
    valid = cursorRectVariant.isValid();
    return cursorRectVariant.toRect();
}

bool MInputContextConnection::hiddenText(bool &valid)
{
    QVariant hiddenTextVariant = mWidgetState[HiddenTextAttribute];
    valid = hiddenTextVariant.isValid();
    return hiddenTextVariant.toBool();
}

bool MInputContextConnection::surroundingText(QString &text, int &cursorPosition)
{
    QVariant textVariant = mWidgetState[SurroundingTextAttribute];
    QVariant posVariant = mWidgetState[CursorPositionAttribute];

    if (textVariant.isValid() && posVariant.isValid()) {
        text = textVariant.toString();
        cursorPosition = posVariant.toInt();
        return true;
    }

    return false;
}

bool MInputContextConnection::hasSelection(bool &valid)
{
    QVariant selectionVariant = mWidgetState[HasSelectionAttribute];
    valid = selectionVariant.isValid();
    return selectionVariant.toBool();
}

int MInputContextConnection::inputMethodMode(bool &valid)
{
    QVariant modeVariant = mWidgetState[InputMethodModeAttribute];
    return modeVariant.toInt(&valid);
}

QRect MInputContextConnection::preeditRectangle(bool &valid)
{
    valid = false;
    return QRect();
}

WId MInputContextConnection::winId()
{
#ifdef Q_WS_WIN
    WId result = 0;
    return result;
#else
    QVariant winIdVariant = mWidgetState[WinId];
    // after transfer by dbus type can change
    switch (winIdVariant.type()) {
    case QVariant::UInt:
        if (sizeof(uint) >= sizeof(WId))
            return winIdVariant.toUInt();
        break;
    case QVariant::ULongLong:
        if (sizeof(qulonglong) >= sizeof(WId))
            return winIdVariant.toULongLong();
        break;
    default:
        if (winIdVariant.canConvert<WId>())
            return winIdVariant.value<WId>();
    }
    return 0;
#endif
}


int MInputContextConnection::anchorPosition(bool &valid)
{
    QVariant posVariant = mWidgetState[AnchorPositionAttribute];
    valid = posVariant.isValid();
    return posVariant.toInt();
}

int MInputContextConnection::preeditClickPos(bool &valid) const
{
    QVariant selectionVariant = mWidgetState[PreeditClickPosAttribute];
    valid = selectionVariant.isValid();
    return selectionVariant.toInt();
}

/* End accessors to widget state */

/* Handlers for inbound communication */
void MInputContextConnection::showInputMethod(unsigned int connectionId)
{
    if (activeConnection != connectionId)
        return;

    Q_EMIT showInputMethodRequest();
}


void MInputContextConnection::hideInputMethod(unsigned int connectionId)
{
    // Only allow this call for current active connection.
    if (activeConnection != connectionId)
        return;

    Q_EMIT hideInputMethodRequest();
}


void MInputContextConnection::mouseClickedOnPreedit(unsigned int connectionId,
                                                            const QPoint &pos, const QRect &preeditRect)
{
    if (activeConnection != connectionId)
        return;

    Q_EMIT mouseClickedOnPreedit(pos, preeditRect);
}


void MInputContextConnection::setPreedit(unsigned int connectionId,
                                                 const QString &text, int cursorPos)
{
    if (activeConnection != connectionId)
        return;

    preedit = text;

    Q_EMIT preeditChanged(text, cursorPos);
}


void MInputContextConnection::reset(unsigned int connectionId)
{
    if (activeConnection != connectionId)
        return;

    preedit.clear();

    Q_EMIT resetInputMethodRequest();

    if (!preedit.isEmpty()) {
        qWarning("Preedit set from InputMethod::reset()!");
        preedit.clear();
    }
}

void
MInputContextConnection::updateWidgetInformation(
    unsigned int connectionId, const QMap<QString, QVariant> &stateInfo,
    bool handleFocusChange)
{
    if (activeConnection != connectionId)
        return;

    QMap<QString, QVariant> oldState = mWidgetState;

    mWidgetState = stateInfo;

#ifndef Q_WS_WIN
    if (handleFocusChange) {
        Q_EMIT focusChanged(winId());
    }
#endif

    Q_EMIT widgetStateChanged(connectionId, mWidgetState, oldState, handleFocusChange);
}

void
MInputContextConnection::receivedAppOrientationAboutToChange(unsigned int connectionId,
                                                                     int angle)
{
    if (activeConnection != connectionId)
        return;

    // Needs to be passed to the MImRotationAnimation listening
    // to this signal first before the plugins. This ensures
    // that the rotation animation can be painted sufficiently early.
    Q_EMIT contentOrientationAboutToChange(angle);

    Q_EMIT contentOrientationAboutToChangeCompleted(angle);
}


void MInputContextConnection::receivedAppOrientationChanged(unsigned int connectionId,
                                                                    int angle)
{
    if (activeConnection != connectionId)
        return;

    // Handle orientation changes through MImRotationAnimation with priority.
    // That's needed for getting the correct rotated pixmap buffers.
    Q_EMIT contentOrientationChanged(angle);

    Q_EMIT contentOrientationChangeCompleted(angle);
}


void MInputContextConnection::setCopyPasteState(unsigned int connectionId,
                                                        bool copyAvailable, bool pasteAvailable)
{
    if (activeConnection != connectionId)
        return;

    Q_EMIT copyPasteStateChanged(copyAvailable, pasteAvailable);
}


void MInputContextConnection::processKeyEvent(
    unsigned int connectionId, QEvent::Type keyType, Qt::Key keyCode,
    Qt::KeyboardModifiers modifiers, const QString &text, bool autoRepeat, int count,
    quint32 nativeScanCode, quint32 nativeModifiers, unsigned long time)
{
    if (activeConnection != connectionId)
        return;

    Q_EMIT receivedKeyEvent(keyType, keyCode,
                            modifiers, text, autoRepeat, count,
                            nativeScanCode, nativeModifiers, time);
}

void MInputContextConnection::registerAttributeExtension(unsigned int connectionId, int id,
                                                         const QString &attributeExtension)
{
    Q_EMIT attributeExtensionRegistered(connectionId, id, attributeExtension);
}

void MInputContextConnection::unregisterAttributeExtension(unsigned int connectionId, int id)
{
    Q_EMIT attributeExtensionUnregistered(connectionId, id);
}

void MInputContextConnection::setExtendedAttribute(
    unsigned int connectionId, int id, const QString &target, const QString &targetName,
    const QString &attribute, const QVariant &value)
{
    Q_EMIT extendedAttributeChanged(connectionId, id, target, targetName, attribute, value);
}

void MInputContextConnection::loadPluginSettings(int connectionId, const QString &descriptionLanguage)
{
    Q_EMIT pluginSettingsRequested(connectionId, descriptionLanguage);
}
/* End handlers for inbound communication */

bool MInputContextConnection::detectableAutoRepeat()
{
    return mDetectableAutoRepeat;
}

void MInputContextConnection::setDetectableAutoRepeat(bool enabled)
{
    mDetectableAutoRepeat = enabled;
}

void MInputContextConnection::setGlobalCorrectionEnabled(bool enabled)
{
    mGlobalCorrectionEnabled = enabled;
}

bool MInputContextConnection::globalCorrectionEnabled()
{
    return mGlobalCorrectionEnabled;
}

void MInputContextConnection::setRedirectKeys(bool enabled)
{
    mRedirectionEnabled = enabled;
}

bool MInputContextConnection::redirectKeysEnabled()
{
    return mRedirectionEnabled;
}

/* */
void MInputContextConnection::sendCommitString(const QString &string, int replaceStart,
                                          int replaceLength, int cursorPos) {

    const int cursorPosition(mWidgetState[CursorPositionAttribute].toInt());
    bool validAnchor(false);

    preedit.clear();

    if (replaceLength == 0  // we don't support replacement
        // we don't support selections
        && anchorPosition(validAnchor) == cursorPosition
        && validAnchor) {
        const int insertPosition(cursorPosition + replaceStart);
        if (insertPosition >= 0) {
            mWidgetState[SurroundingTextAttribute]
                = mWidgetState[SurroundingTextAttribute].toString().insert(insertPosition, string);
            mWidgetState[CursorPositionAttribute] = cursorPos < 0 ? (insertPosition + string.length()) : cursorPos;
            mWidgetState[AnchorPositionAttribute] = mWidgetState[CursorPositionAttribute];
        }
    }
}

void MInputContextConnection::sendKeyEvent(const QKeyEvent &keyEvent,
                                           Maliit::EventRequestType requestType)
{
    if (requestType != Maliit::EventRequestSignalOnly
        && preedit.isEmpty()
        && keyEvent.key() == Qt::Key_Backspace
        && keyEvent.type() == QEvent::KeyPress) {
        QString surrString(mWidgetState[SurroundingTextAttribute].toString());
        const int cursorPosition(mWidgetState[CursorPositionAttribute].toInt());
        bool validAnchor(false);

        if (!surrString.isEmpty()
            && cursorPosition > 0
            // we don't support selections
            && anchorPosition(validAnchor) == cursorPosition
            && validAnchor) {
            mWidgetState[SurroundingTextAttribute] = surrString.remove(cursorPosition - 1, 1);
            mWidgetState[CursorPositionAttribute] = cursorPosition - 1;
            mWidgetState[AnchorPositionAttribute] = cursorPosition - 1;
        }
    }
}
/* */

/* */
void MInputContextConnection::handleDisconnection(unsigned int connectionId)
{
    Q_EMIT clientDisconnected(connectionId);

    if (activeConnection != connectionId) {
        return;
    }

    activeConnection = 0;

    Q_EMIT activeClientDisconnected();
}

void MInputContextConnection::activateContext(unsigned int connectionId)
{
    if (connectionId == activeConnection) {
        return;
    }

    /* Notify current/previously active context that it is no longer active */
    sendActivationLostEvent();

    activeConnection = connectionId;

    /* Notify new input context about state/settings stored in the IM server */
    if (activeConnection) {
        /* Hack: Circumvent if(newValue == oldValue) return; guards */
        mGlobalCorrectionEnabled = !mGlobalCorrectionEnabled;
        setGlobalCorrectionEnabled(!mGlobalCorrectionEnabled);

        mRedirectionEnabled = !mRedirectionEnabled;
        setRedirectKeys(!mRedirectionEnabled);

        mDetectableAutoRepeat = !mDetectableAutoRepeat;
        setDetectableAutoRepeat(!mDetectableAutoRepeat);
    }

    Q_EMIT clientActivated(connectionId);

}
/* */

void MInputContextConnection::sendPreeditString(const QString &string,
                                                const QList<Maliit::PreeditTextFormat> &preeditFormats,
                                                int replaceStart, int replaceLength,
                                                int cursorPos)
{
    Q_UNUSED(preeditFormats);
    Q_UNUSED(replaceStart);
    Q_UNUSED(replaceLength);
    Q_UNUSED(cursorPos);
    if (activeConnection) {
        preedit = string;
    }
}

/* */
void MInputContextConnection::setSelection(int start, int length)
{
    Q_UNUSED(start);
    Q_UNUSED(length);
}


void MInputContextConnection::notifyImInitiatedHiding()
{}


void MInputContextConnection::invokeAction(const QString &action, const QKeySequence &sequence)
{
    Q_UNUSED(action);
    Q_UNUSED(sequence);
}


QString MInputContextConnection::selection(bool &valid)
{
    valid = false;
    return QString();
}

void MInputContextConnection::setLanguage(const QString &language)
{
    Q_UNUSED(language);
}

void MInputContextConnection::sendActivationLostEvent()
{}

void MInputContextConnection::updateInputMethodArea(const QRegion &region)
{
    Q_UNUSED(region);
}

void MInputContextConnection::notifyExtendedAttributeChanged(int ,
                                                             const QString &,
                                                             const QString &,
                                                             const QString &,
                                                             const QVariant &)
{
    // empty default implementation
}

void MInputContextConnection::notifyExtendedAttributeChanged(const QList<int> &,
                                                             int ,
                                                             const QString &,
                                                             const QString &,
                                                             const QString &,
                                                             const QVariant &)
{
    // empty default implementation
}

void MInputContextConnection::pluginSettingsLoaded(int clientId, const QList<MImPluginSettingsInfo> &info)
{
    Q_UNUSED(clientId);
    Q_UNUSED(info);

    // empty default implementation
}


QVariantMap MInputContextConnection::widgetState() const
{
    return mWidgetState;
}

QVariant MInputContextConnection::inputMethodQuery(Qt::InputMethodQuery query, const QVariant &argument) const
{
    switch (query) {
        case Qt::ImEnabled:
            return mWidgetState.value(QStringLiteral("focusState"));
        case Qt::ImCursorRectangle:
            return mWidgetState.value(QStringLiteral("cursorRectangle"));
//        case Qt::ImFont:
//            return QVariant();
        case Qt::ImCursorPosition:
            return mWidgetState.value(QStringLiteral("cursorPosition"));
        case Qt::ImSurroundingText:
            return mWidgetState.value(QStringLiteral("surroundingText"));
        case Qt::ImCurrentSelection:
            return QVariant(); // TODO implement
//        case Qt::ImMaximumTextLength:
        case Qt::ImAnchorPosition:
            return mWidgetState.value(QStringLiteral("anchorPosition"));
        case Qt::ImHints:
            return mWidgetState.value(QStringLiteral("maliit-inputmethod-hints"));
//        case Qt::ImPreferredLanguage:
//        case Qt::ImAbsolutePosition:
//        case Qt::ImTextBeforeCursor:
//        case Qt::ImTextAfterCursor:
        case Qt::ImEnterKeyType:
            return mWidgetState.value(QStringLiteral("enterKeyType"));
//        case Qt::ImAnchorRectangle:
//        case Qt::ImInputItemClipRectangle:
//            return QVariant();
    }
    return QVariant();

}
