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
    const char * const ToolbarIdAttribute = "toolbarId";
    const char * const ToolbarAttribute = "toolbar";
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
    QVariant focusStateVariant = widgetState[FocusStateAttribute];
    valid = focusStateVariant.isValid();
    return focusStateVariant.toBool();
}

int MInputContextConnection::contentType(bool &valid)
{
    QVariant contentTypeVariant = widgetState[ContentTypeAttribute];
    return contentTypeVariant.toInt(&valid);
}

bool MInputContextConnection::correctionEnabled(bool &valid)
{
    QVariant correctionVariant = widgetState[CorrectionAttribute];
    valid = correctionVariant.isValid();
    return correctionVariant.toBool();
}


bool MInputContextConnection::predictionEnabled(bool &valid)
{
    QVariant predictionVariant = widgetState[PredictionAttribute];
    valid = predictionVariant.isValid();
    return predictionVariant.toBool();
}

bool MInputContextConnection::autoCapitalizationEnabled(bool &valid)
{
    QVariant capitalizationVariant = widgetState[AutoCapitalizationAttribute];
    valid = capitalizationVariant.isValid();
    return capitalizationVariant.toBool();
}

QRect MInputContextConnection::cursorRectangle(bool &valid)
{
    QVariant cursorRectVariant = widgetState[CursorRectAttribute];
    valid = cursorRectVariant.isValid();
    return cursorRectVariant.toRect();
}

bool MInputContextConnection::hiddenText(bool &valid)
{
    QVariant hiddenTextVariant = widgetState[HiddenTextAttribute];
    valid = hiddenTextVariant.isValid();
    return hiddenTextVariant.toBool();
}

bool MInputContextConnection::surroundingText(QString &text, int &cursorPosition)
{
    QVariant textVariant = widgetState[SurroundingTextAttribute];
    QVariant posVariant = widgetState[CursorPositionAttribute];

    if (textVariant.isValid() && posVariant.isValid()) {
        text = textVariant.toString();
        cursorPosition = posVariant.toInt();
        return true;
    }

    return false;
}

bool MInputContextConnection::hasSelection(bool &valid)
{
    QVariant selectionVariant = widgetState[HasSelectionAttribute];
    valid = selectionVariant.isValid();
    return selectionVariant.toBool();
}

int MInputContextConnection::inputMethodMode(bool &valid)
{
    QVariant modeVariant = widgetState[InputMethodModeAttribute];
    return modeVariant.toInt(&valid);
}

QRect MInputContextConnection::preeditRectangle(bool &valid)
{
    valid = false;
    return QRect();
}

WId MInputContextConnection::winId()
{
    QVariant winIdVariant = widgetState[WinId];
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
}

int MInputContextConnection::anchorPosition(bool &valid)
{
    QVariant posVariant = widgetState[AnchorPositionAttribute];
    valid = posVariant.isValid();
    return posVariant.toInt();
}

int MInputContextConnection::preeditClickPos(bool &valid) const
{
    QVariant selectionVariant = widgetState[PreeditClickPosAttribute];
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
    QMap<QString, QVariant> oldState = widgetState;

    widgetState = stateInfo;

    if (handleFocusChange) {
        Q_EMIT focusChanged(winId());
    }

    Q_EMIT widgetStateChanged(connectionId, widgetState, oldState, handleFocusChange);
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
    Q_EMIT appOrientationAboutToChange(angle);

    Q_EMIT appOrientationAboutToChangeCompleted(angle);
}


void MInputContextConnection::receivedAppOrientationChanged(unsigned int connectionId,
                                                                    int angle)
{
    if (activeConnection != connectionId)
        return;

    // Handle orientation changes through MImRotationAnimation with priority.
    // That's needed for getting the correct rotated pixmap buffers.
    Q_EMIT appOrientationChanged(angle);

    Q_EMIT appOrientationChangeCompleted(angle);
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

    Q_EMIT recievedKeyEvent(keyType, keyCode,
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

    const int cursorPosition(widgetState[CursorPositionAttribute].toInt());
    bool validAnchor(false);

    preedit.clear();

    if (replaceLength == 0  // we don't support replacement
        // we don't support selections
        && anchorPosition(validAnchor) == cursorPosition
        && validAnchor) {
        const int insertPosition(cursorPosition + replaceStart);
        if (insertPosition >= 0) {
            widgetState[SurroundingTextAttribute]
                = widgetState[SurroundingTextAttribute].toString().insert(insertPosition, string);
            widgetState[CursorPositionAttribute] = cursorPos < 0 ? (insertPosition + string.length()) : cursorPos;
            widgetState[AnchorPositionAttribute] = widgetState[CursorPositionAttribute];
        }
    }
}

void MInputContextConnection::sendKeyEvent(const QKeyEvent &keyEvent,
                                           MInputMethod::EventRequestType requestType)
{
    if (requestType != MInputMethod::EventRequestSignalOnly
        && preedit.isEmpty()
        && keyEvent.key() == Qt::Key_Backspace
        && keyEvent.type() == QEvent::KeyPress) {
        QString surrString(widgetState[SurroundingTextAttribute].toString());
        const int cursorPosition(widgetState[CursorPositionAttribute].toInt());
        bool validAnchor(false);

        if (!surrString.isEmpty()
            && cursorPosition > 0
            // we don't support selections
            && anchorPosition(validAnchor) == cursorPosition
            && validAnchor) {
            widgetState[SurroundingTextAttribute] = surrString.remove(cursorPosition - 1, 1);
            widgetState[CursorPositionAttribute] = cursorPosition - 1;
            widgetState[AnchorPositionAttribute] = cursorPosition - 1;
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
                                                const QList<MInputMethod::PreeditTextFormat> &preeditFormats,
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


void MInputContextConnection::copy()
{}

void MInputContextConnection::paste()
{}


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
