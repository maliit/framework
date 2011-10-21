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

#include <QtCore>
#include <QWidget> // For WId

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
    Q_DISABLE_COPY(MInputContextConnection)

public:
    explicit MInputContextConnection(QObject *parent = 0);
    virtual ~MInputContextConnection();

    /*!
     * \brief Returns focus state if output parameter \a valid is \c true.
     *
     * By focus state it is meant to be focus state of an input widget
     * on application side. If returned value is valid and is \c true then this
     * means that input widget is focused. Otherwise, if returned value is
     * \c false then no input widget is focused.
     *
     * \param valid An output parameter stating whether return value is valid.
     */
    virtual bool focusState(bool &valid);

    /*!
     * \brief returns content type for focused widget if output parameter valid is true,
     * value matches enum M::TextContentType
     */
    virtual int contentType(bool &valid);

    /*!
     * \brief returns input method correction hint if output parameter valid is true.
     */
    virtual bool correctionEnabled(bool &valid);

    /*!
     * \brief returns input methoContextd word prediction hint if output parameter valid is true.
     */
    virtual bool predictionEnabled(bool &valid);

    /*!
     * \brief returns input method auto-capitalization hint if output parameter valid is true.
     */
    virtual bool autoCapitalizationEnabled(bool &valid);

    /*!
     * \brief get surrounding text and cursor position information
     */
    virtual bool surroundingText(QString &text, int &cursorPosition);

    /*!
     * \brief returns true if there is selecting text
     */
    virtual bool hasSelection(bool &valid);

    /*!
     * \brief get input method mode
     */
    virtual int inputMethodMode(bool &valid);

    /*!
     * \brief get preedit rectangle
     */
    virtual QRect preeditRectangle(bool &valid);

    /*!
     * \brief get cursor rectangle
     */
    virtual QRect cursorRectangle(bool &valid);

    /*!
     * \brief true if text input is being made hidden, e.g. with password fields
     */
    virtual bool hiddenText(bool &valid);

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
                                   int cursorPos = -1);

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
                                  int replaceLength = 0, int cursorPos = -1);

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
                              = MInputMethod::EventRequestBoth);

    /*!
     * \brief notifies about hiding initiated by the input method server side
     */
    virtual void notifyImInitiatedHiding();

    /*!
    * \brief copy selected text
    */
    virtual void copy();

    /*!
    * \brief paste plain text from clipboard
    */
    virtual void paste();

    /*!
     * \brief Set if the input method wants to process all raw key events
     * from hardware keyboard (via \a processKeyEvent calls).
     */
    virtual void setRedirectKeys(bool enabled);

    /*!
     * \brief Set detectable autorepeat for X on/off
     *
     * Detectable autorepeat means that instead of press, release, press, release, press,
     * release... sequence of key events you get press, press, press, release key events
     * when a key is repeated.  The setting is X client specific.  This is intended to be
     * used when key event redirection is enabled with \a setRedirectKeys.
     */
    virtual void setDetectableAutoRepeat(bool enabled);

    /*!
     * \brief set global correction option enable/disable
     */
    virtual void setGlobalCorrectionEnabled(bool);

    /*!
     *\brief Sets selection text start from \a start with \a length in the application widget.
     */
    virtual void setSelection(int start, int length);

    /*!
     * \brief returns the position of the selection anchor.
     *
     * This may be less or greater than cursor position, depending on which side of selection
     * the cursor is. If there is no selection, it returns the same as cursor position.
     */
    virtual int anchorPosition(bool &valid);

    /*!
     * \brief returns the current cursor position within the preedit region
     */
    virtual int preeditClickPos(bool &valid) const;

    /*!
     * \brief returns the selecting text
     */
    virtual QString selection(bool &valid);

    /*!
     * \brief Sets current language of active input method.
     * \param language ICU format locale ID string
     */
    virtual void setLanguage(const QString &language);

    virtual void sendActivationLostEvent();

public: // Inbound communication handlers
    //! ipc method provided to application, makes the application the active one
    void activateContext(unsigned int connectionId);

    //! ipc method provided to the application, shows input method
    void showInputMethod(unsigned int clientId);

    //! ipc method provided to the application, hides input method
    void hideInputMethod(unsigned int clientId);

    //! ipc method provided to the application, signals mouse click on preedit
    void mouseClickedOnPreedit(unsigned int clientId,
                               const QPoint &pos, const QRect &preeditRect);

    //! ipc method provided to the application, sets preedit
    void setPreedit(unsigned int clientId, const QString &text, int cursorPos);

    void updateWidgetInformation(unsigned int clientId,
                                 const QMap<QString, QVariant> &stateInformation,
                                 bool focusChanged);

    //! ipc method provided to the application, resets the input method
    void reset(unsigned int clientId);

    /*!
     * \brief Target application is changing orientation
     */
    void receivedAppOrientationAboutToChange(unsigned int clientId, int angle);

    /*!
     * \brief Target application changed orientation (already finished)
     */
    void receivedAppOrientationChanged(unsigned int clientId, int angle);

    /*! \brief Set copy/paste state for appropriate UI elements in the input method server
     *  \param copyAvailable bool TRUE if text is selected
     *  \param pasteAvailable bool TRUE if clipboard content is not empty
     */
    void setCopyPasteState(unsigned int clientId,
                           bool copyAvailable, bool pasteAvailable);

    /*!
     * \brief Process a key event redirected from hardware keyboard to input method plugin(s).
     *
     * This is called only if one has enabled redirection by calling \a setRedirectKeys.
     */
    void processKeyEvent(unsigned int clientId, QEvent::Type keyType, Qt::Key keyCode,
                         Qt::KeyboardModifiers modifiers, const QString &text, bool autoRepeat,
                         int count, quint32 nativeScanCode, quint32 nativeModifiers, unsigned long time);

    /*!
     * \brief Register an input method attribute extension which is defined in \a fileName with the
     * unique identifier \a id.
     *
     *  The \a id should be unique, and the \a fileName is the absolute file name of the
     *  attribute extension.
     */
    void registerAttributeExtension(unsigned int clientId, int id, const QString &fileName);

    /*!
     * \brief Unregister an input method attribute extension which unique identifier is \a id.
     */
    void unregisterAttributeExtension(unsigned int clientId, int id);

    /*!
     * \brief Sets the \a attribute for the \a target in the extended attribute which has unique \a id to \a value.
     */
    void setExtendedAttribute(unsigned int clientId, int id, const QString &target,
                              const QString &targetItem, const QString &attribute, const QVariant &value);

public Q_SLOTS:
    //! Update \a region covered by virtual keyboard
    virtual void updateInputMethodArea(const QRegion &region);

Q_SIGNALS:
    /* Emitted first */
    void appOrientationAboutToChange(int angle);
    void appOrientationChanged(int angle);
    /* Emitted later */
    void appOrientationAboutToChangeCompleted(int angle);
    void appOrientationChangeCompleted(int angle);

    void focusChanged(WId id);

    //! Emitted when input method request to be shown.
    void showInputMethodRequest();

    //! Emitted when input method request to be hidden.
    void hideInputMethodRequest();

    void resetInputMethodRequest();

    void copyPasteStateChanged(bool copyAvailable, bool pasteAvailable);
    void widgetStateChanged(unsigned int clientId, const QMap<QString, QVariant> &newState,
                            const QMap<QString, QVariant> &oldState, bool focusChanged);

    void attributeExtensionRegistered(unsigned int connectionId, int id, const QString &attributeExtension);
    void attributeExtensionUnregistered(unsigned int connectionId, int id);
    void extendedAttributeChanged(unsigned int connectionId, int id, const QString &target,
                              const QString &targetName,const QString &attribute, const QVariant &value);

    void clientActivated(unsigned int connectionId);
    void clientDisconnected(unsigned int connectionId);
    void activeClientDisconnected();

    void preeditChanged(const QString &text, int cursorPos);
    void mouseClickedOnPreedit(const QPoint &pos, const QRect &preeditRect);

    void recievedKeyEvent(QEvent::Type keyType, Qt::Key keyCode,
                         Qt::KeyboardModifiers modifiers, const QString &text, bool autoRepeat,
                         int count, quint32 nativeScanCode, quint32 nativeModifiers, unsigned long time);

protected:
    unsigned int activeConnection; // 0 means no active connection

    bool detectableAutoRepeat();
    bool globalCorrectionEnabled();
    bool redirectKeysEnabled();

    void handleDisconnection(unsigned int connectionId);
    void handleActivation(unsigned int connectionId);

private:
    /*!
     * \brief get the X window id of the active app window
     */
    WId winId();

private:
    MInputContextConnectionPrivate *d;
    int lastOrientation;

    /* FIXME: rename with m prefix, and provide protected accessors for derived classes */
    QMap<QString, QVariant> widgetState;
    bool mGlobalCorrectionEnabled;
    bool mRedirectionEnabled;
    bool mDetectableAutoRepeat;
    QString preedit;
};
//! \internal_end

#endif
