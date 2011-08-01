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

#ifndef MINPUTCONTEXTCONNECTION_H
#define MINPUTCONTEXTCONNECTION_H

#include "minputmethodnamespace.h"

#include <QRect>
#include <QObject>

class QString;
class QRegion;
class QKeyEvent;

class MInputContextConnectionPrivate;
class MAbstractInputMethod;
class MAttributeExtensionId;

//! \internal
/*!
 * \brief MInputContextConnection is a base class of the input method communication implementation
 * between the input context and the input method server.
 */
class MInputContextConnection: public QObject
{
    Q_OBJECT

public:
    MInputContextConnection();
    virtual ~MInputContextConnection();

    /*!
     * \brief Adds the target which get called on incoming requests
     */
    virtual void addTarget(MAbstractInputMethod *target);

    /*!
     * \brief Removes the handler of the connection
     */
    virtual void removeTarget(MAbstractInputMethod *target);

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
     * \brief get preedit rectangle
     */
    virtual QRect preeditRectangle(bool &valid) = 0;

    /*!
     * \brief get cursor rectangle
     */
    virtual QRect cursorRectangle(bool &valid) = 0;

    /*!
     * \brief true if text input is being made hidden, e.g. with password fields
     */
    virtual bool hiddenText(bool &valid) = 0;

    /*!
     * \brief Updates pre-edit string in the application widget
     *
     * Implement this method to update the pre-edit string
     * \param string    The new pre-edit string
     * \param preeditFormats Selects visual stylings for each part of preedit
     * \param replacementStart The position at which characters are to be replaced relative
     *  from the start of the preedit string.
     * \param replacementLength The number of characters to be replaced in the preedit string.
     * \param cursorPos The cursor position inside preedit
     */
    virtual void sendPreeditString(const QString &string,
                                   const QList<MInputMethod::PreeditTextFormat> &preeditFormats,
                                   int replacementStart = 0,
                                   int replacementLength = 0,
                                   int cursorPos = -1) = 0;

    /*!
     * \brief Updates commit string in the application widget, and set cursor position.
     *
     * Implement this method to update the commit string
     * \param string    The string to be committed
     * \param replaceStart The position at which characters are to be replaced relative
     *  from the start of the preedit string.
     * \param replaceLength The number of characters to be replaced in the preedit string.
     * \param cursorPos The cursor position to be set. the cursorPos is the position relative
     *  to commit string start. Negative values are used as commit string end position
     */
    virtual void sendCommitString(const QString &string, int replaceStart = 0,
                                  int replaceLength = 0, int cursorPos = -1) = 0;

    /*!
     * \brief Sends key event to the application
     *
     * This method is used to deliver the key event to active widget.
     * A \a MInputMethodState::keyPress or \a MInputMethodState::keyRelease
     * event is also emitted. Depending on the value of \a requestType
     * parameter, a Qt::KeyEvent and/or a signal is emitted.
     * \param keyEvent The event to send
     * \param requestType The type of the request: event, signal, or both.
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
    virtual void setGlobalCorrectionEnabled(bool) = 0;

    /*!
     *\brief Sets selection text start from \a start with \a length in the application widget.
     */
    virtual void setSelection(int start, int length) = 0;

    /*!
     * \brief returns the position of the selection anchor.
     *
     * This may be less or greater than cursor position, depending on which side of selection
     * the cursor is. If there is no selection, it returns the same as cursor position.
     */
    virtual int anchorPosition(bool &valid) = 0;

    /*!
     * \brief returns the selecting text
     */
    virtual QString selection(bool &valid) = 0;

public slots:

    //! Update \a region covered by virtual keyboard
    virtual void updateInputMethodArea(const QRegion &region);

public:
    /*!
     * \brief returns the current cursor position within the preedit region
     */
    virtual int preeditClickPos(bool &valid) const = 0;

signals:

    //! \internal
    //! Emitted when input method request to be shown.
    void showInputMethodRequest();

    //! Emitted when input method request to be hidden.
    void hideInputMethodRequest();

    //! Emitted when set toobar to input method.
    void toolbarIdChanged(const MAttributeExtensionId &id);

    //! This signal is emited when a new key override is created.
    void keyOverrideCreated(const MAttributeExtensionId &id);
    //! \internal_end.

protected:
    QSet<MAbstractInputMethod *> targets();

private:
    MInputContextConnectionPrivate *d;
};
//! \internal_end

#endif
