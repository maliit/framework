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

#ifndef MABSTRACTINPUTMETHODHOST_H
#define MABSTRACTINPUTMETHODHOST_H

#include "minputmethodnamespace.h"

#include <QRect>
#include <QObject>

class QString;
class QRegion;
class QKeyEvent;

class MInputMethodBase;
class MToolbarId;

class MAbstractInputMethodHostPrivate;

/*!
 * \brief Provides an interface for input method instances to connect to the environment.
 *
 * MAbstractInputMethodHost provides methods MInputMethodBase instances can use
 * for interacting with the application that is using input method services and
 * the input method framework state itself.
 * Note: this is not meant to be derived by the input method framework users.
 * 
 */
class MAbstractInputMethodHost: public QObject
{
    Q_OBJECT

public:
    explicit MAbstractInputMethodHost(QObject *parent = 0);
    virtual ~MAbstractInputMethodHost();

    /*!
     * \brief returns content type for focused widget if output parameter valid is true,
     * value matches enum M::TextContentType
     */
    virtual int contentType(bool &valid) = 0;

    /*!
     * \brief returns input method correction hint if output parameter valid is true.
     */
    virtual bool correctionEnabled(bool &valid) = 0;

    /*!
     * \brief returns input method word prediction hint if output parameter valid is true.
     */
    virtual bool predictionEnabled(bool &valid) = 0;

    /*!
     * \brief returns input method auto-capitalization hint if output parameter valid is true.
     */
    virtual bool autoCapitalizationEnabled(bool &valid) = 0;

    /*!
     * \brief get surrounding text and cursor position information
     */
    virtual bool surroundingText(QString &text, int &cursorPosition) = 0;

    /*!
     * \brief returns true if there is selecting text
     */
    virtual bool hasSelection(bool &valid) = 0;

    /*!
     * \brief get input method mode
     */
    virtual int inputMethodMode(bool &valid) = 0;

    /*!
     * \brief get input method mode
     */
    virtual QRect preeditRectangle(bool &valid) = 0;

    /*!
     * \brief Updates pre-edit string in the application widget
     *
     * \param string    The new pre-edit string
     * \param preeditFace Selects visual styling for preedit
     */
    virtual void sendPreeditString(const QString &string,
                                   MInputMethod::PreeditFace preeditFace
                                    = MInputMethod::PreeditDefault) = 0;

    /*!
     * \brief Updates commit string in the application widget
     *
     * \param string    The string to be committed
     */
    virtual void sendCommitString(const QString &string) = 0;

    /*!
     * \brief Sends key event to the application
     *
     * This method is used to deliver the key event to active widget.
     * A \a MInputMethodState::keyPress or \a MInputMethodState::keyRelease
     * event is also emitted. Depending on the value of \a requestType
     * parameter, a Qt::KeyEvent and/or a signal is emitted.
     * \param keyEvent The event to send
     * \param signalOnly only the signal should be emitted.
     */
    virtual void sendKeyEvent(const QKeyEvent &keyEvent,
                              MInputMethod::EventRequestType requestType
                               = MInputMethod::EventRequestBoth) = 0;

    /*!
     * \brief notifies about hiding initiated by the input method server side
     */
    virtual void notifyImInitiatedHiding() = 0;

    /*!
    * \brief copy selected text
    */
    virtual void copy() = 0;

    /*!
    * \brief paste plain text from clipboard
    */
    virtual void paste() = 0;

    /*!
     * \brief Set if the input method wants to process all raw key events
     * from hardware keyboard (via \a processKeyEvent calls).
     */
    virtual void setRedirectKeys(bool enabled) = 0;

    /*!
     * \brief Set detectable autorepeat for X on/off
     *
     * Detectable autorepeat means that instead of press, release, press, release, press,
     * release... sequence of key events you get press, press, press, release key events
     * when a key is repeated.  The setting is X client specific.  This is intended to be
     * used when key event redirection is enabled with \a setRedirectKeys.
     */
    virtual void setDetectableAutoRepeat(bool enabled) = 0;

    /*!
     * \brief set global correction option enable/disable
     */
    virtual void setGlobalCorrectionEnabled(bool enabled) = 0;

private:
    Q_DISABLE_COPY(MAbstractInputMethodHost)
    Q_DECLARE_PRIVATE(MAbstractInputMethodHost)

    MAbstractInputMethodHostPrivate *d;
};


#endif
