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

#ifndef MIMSERVERCONNECTION_H
#define MIMSERVERCONNECTION_H

#include "minputmethodnamespace.h"

#include <QtCore>

class MImServerConnectionPrivate;

class MImServerConnection : public QObject
{
    Q_OBJECT

public:
    //! \brief Constructor
    explicit MImServerConnection(QObject *parent = 0);

    virtual bool pendingResets();

    /* Outgoing communication */
    virtual void activateContext();

    virtual void showInputMethod();

    virtual void hideInputMethod();

    virtual void mouseClickedOnPreedit(const QPoint &pos, const QRect &preeditRect);

    virtual void setPreedit(const QString &text, int cursorPos);

    virtual void updateWidgetInformation(const QMap<QString, QVariant> &stateInformation,
                                 bool focusChanged);

    virtual void reset(bool requireSynchronization);

    virtual void appOrientationAboutToChange(int angle);

    virtual void appOrientationChanged(int angle);

    virtual void setCopyPasteState(bool copyAvailable, bool pasteAvailable);

    virtual void processKeyEvent(QEvent::Type keyType, Qt::Key keyCode,
                         Qt::KeyboardModifiers modifiers,
                         const QString &text, bool autoRepeat, int count,
                         quint32 nativeScanCode, quint32 nativeModifiers, unsigned long time);

    virtual void registerAttributeExtension(int id, const QString &fileName);

    virtual void unregisterAttributeExtension(int id);

    virtual void setExtendedAttribute(int id, const QString &target, const QString &targetItem,
                              const QString &attribute, const QVariant &value);

public:
    Q_SIGNAL void connected();
    Q_SIGNAL void disconnected();

    /* Incoming communication */
    Q_SIGNAL void activationLostEvent();

    //! \brief Notifies about hiding initiated by the input method server side
    Q_SIGNAL void imInitiatedHide();

    /*!
     * \brief Commits a string to current focus widget, and set cursor position.
     * \param string    The new string committed
     * \param replacementStart The position at which characters are to be replaced relative
     *  from the start of the preedit string.
     * \param replacementLength The number of characters to be replaced in the preedit string.
     * \param cursorPos The cursor position to be set. the cursorPos is the position relative
     *  to commit string start. Negative values are used as commit string end position
     *
     * Note: If \a replacementLength is 0, \a replacementStart gives the insertion position
     * for the inserted \a string.
     * For example, if the replacement starting at -1 with a length of 2, then application will
     * remove the last character before the preedit string and the first character afterwards,
     * and insert the commit string directly before the preedit string.
     */
    Q_SIGNAL void commitString(const QString &string, int replacementStart = 0,
                              int replacementLength = 0, int cursorPos = -1);

    /*!
     * \brief Updates preedit string of the current focus widget
     * \param string    The new string
     * \param preeditFormats The formats for each part of preedit.
     * \param replacementStart The position at which characters are to be replaced relative
     *  from the start of the preedit string.
     * \param replacementLength The number of characters to be replaced in the preedit string.
     * \param cursorPos Cursor position. If it is less than 0, then the cursor will be hidden.
     *
     */
    Q_SIGNAL void updatePreedit(const QString &string, const QList<MInputMethod::PreeditTextFormat> &preeditFormats,
                               int replacementStart = 0, int replacementLength = 0, int cursorPos = -1);

    //! \brief Sends a non-printable key event. Parameters as in QKeyEvent constructor
    Q_SIGNAL void keyEvent(int type, int key, int modifiers, const QString &text, bool autoRepeat,
                          int count, MInputMethod::EventRequestType requestType
                          = MInputMethod::EventRequestBoth);

    //!
    // \brief Updates the input method window area
    // \param rect Bounding rectangle of the input method area
    Q_SIGNAL void updateInputMethodArea(const QRect &rect);

    /*!
     * \brief set global correction option enable/disable
     */
    Q_SIGNAL void setGlobalCorrectionEnabled(bool);

    /*! \brief Get rectangle covering preedit
     * \param valid validity for the return value
     * \param rectangle the preedit rectangle.
     *
     * Warning: If multiple slots are connected to this signal, the last slot to be
     * called will be able to overwrite value set by previously called slots.
     */
    Q_SIGNAL void getPreeditRectangle(QRect &rectangle, bool &valid) const;

    /*!
     * \brief Sends copy command to text editor.
     * This method tries to call "copy" slot in the focused widget
     * and sends QKeyEvent corresponding to Ctrl-C if slot can not be called.
     */
    Q_SIGNAL void copy();

    /*!
     * \brief Sends paste command to text editor.
     * This method tries to call "paste" slot in the focused widget
     * and sends QKeyEvent corresponding to Ctrl-V if slot can not be called.
     */
    Q_SIGNAL void paste();

    /*!
     * \brief Set if the input method wants to process all raw key events
     * from hardware keyboard (via \a processKeyEvent calls).
     */
    Q_SIGNAL void setRedirectKeys(bool enabled);

    /*!
     * \brief Set detectable autorepeat for X on/off
     *
     * Detectable autorepeat means that instead of press, release, press, release, press,
     * release... sequence of key events you get press, press, press, release key events
     * when a key is repeated.  The setting is X client specific.  This is intended to be
     * used when key event redirection is enabled with \a setRedirectKeys.
     */
    Q_SIGNAL void setDetectableAutoRepeat(bool enabled);

    /*!
     * \brief Sets selection which start from \start with \a length in the focus widget.
     *
     * \param start the start index
     * \param length the length of selection
     * Note: The cursor will be moved after the commit string has been committed, and the
     * preedit string will be located at the new edit position.
     */
    Q_SIGNAL void setSelection(int start, int length);

    /*!
     * \brief get selecting text
     * \param valid validity for the return value
     * \param selection the current selection
     *
     * Warning: If multiple slots are connected to this signal, the last slot to be
     * called will be able to overwrite value set by previously called slots.
     */
    Q_SIGNAL void getSelection(QString &selection, bool &valid) const;

private:
    Q_DISABLE_COPY(MImServerConnection)

    MImServerConnectionPrivate *d;
};

#endif
