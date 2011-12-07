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
#include "surface.h"

#include <QRect>
#include <QObject>
#include <QPixmap>

class QString;
class QRegion;
class QKeyEvent;

class MImPluginDescription;
class MImSubViewDescription;
class MAbstractInputMethodHostPrivate;

/*! \ingroup maliitserver
 * \brief Provides an interface for input method instances to connect to the environment.
 *
 * MAbstractInputMethodHost provides methods MAbstractInputMethod instances can use
 * for interacting with the application that is using input method services and
 * the input method framework state itself.
 *
 * \note this is not meant to be derived by the input method framework users;
 * the concrete implementations are provided by the framework.
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
     * \brief get preedit rectangle
     */
    virtual QRect preeditRectangle(bool &valid) = 0;

    /*!
     * \brief get cursor rectangle
     */
    virtual QRect cursorRectangle(bool &valid) = 0;

    /*!
     * \brief returns the position of the selection anchor.
     *
     * This may be less or greater than cursor position, depending on which side of selection
     * the cursor is. If there is no selection, it returns the same as cursor position.
     */
    virtual int anchorPosition(bool &valid) = 0;

    /*!
     * \brief true if text input is being made hidden, e.g. with password fields
     */
    virtual bool hiddenText(bool &valid);

    /*!
     * \brief returns the selecting text
     */
    virtual QString selection(bool &valid) = 0;

    /*!
     * \brief returns a pixmap that needs to be drawn as the background of the
     *        input method. Pixmap contains the application's window contents.
     */
    QPixmap background() const;

Q_SIGNALS:
    //! This signal is emitted when input method plugins are loaded or unloaded
    void pluginsChanged();

public Q_SLOTS:
    /*!
     * \brief Updates pre-edit string in the application widget
     *
     * \param string    The new pre-edit string
     * \param preeditFormats Selects visual stylings for each part of preedit
     * \param replacementStart The position at which characters are to be replaced relative
     *  from the start of the preedit string.
     * \param replacementLength The number of characters to be replaced in the preedit string.
     * \param cursorPos The cursur position inside preedit
     */
    virtual void sendPreeditString(const QString &string,
                                   const QList<MInputMethod::PreeditTextFormat> &preeditFormats,
                                   int replacementStart = 0,
                                   int replacementLength = 0,
                                   int cursorPos = -1) = 0;

    /*!
     * \brief Updates commit string in the application widget, and set cursor position.
     *
     * \param string    The string to be committed
     * \param replaceStart The position at which characters are to be replaced relative
     *  from the start of the preedit string.
     * \param replaceLength The number of characters to be replaced in the preedit string.
     * \param cursorPos The cursor position to be set. the cursorPos is the position relative
     *  to commit string start. Negative values are used as commit string end position.
     *  Cursor position is applied AFTER committing text. This means the position might be
     *  different than intended because of active validators etc.
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
     * \param signalOnly only the signal should be emitted.
     */
    virtual void sendKeyEvent(const QKeyEvent &keyEvent,
                              MInputMethod::EventRequestType requestType
                               = MInputMethod::EventRequestBoth) = 0;

    /*!
     * \brief Notifies about hiding initiated by the input method.
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

    /*!
     * \brief Sets input mode indicator state.
     * \param mode Input mode indicator state.
     * \sa InputModeIndicator.
     */
    virtual void setInputModeIndicator(MInputMethod::InputModeIndicator mode) = 0;

    /*!
     * Asks environment to change active plugin according to \a direction.
     */
    virtual void switchPlugin(MInputMethod::SwitchDirection direction) = 0;

    /*!
     * Asks environment to change active plugin to specified one.
     * \param pluginName Name for plugin which will be activated
     */
    virtual void switchPlugin(const QString &pluginName) = 0;

    /*!
     * Reserves screen area for input method. Mouse events on top of this
     * area do not fall through to the application
     *
     * \param region the new region
     */
    virtual void setScreenRegion(const QRegion &region) = 0;

    /*!
     * Sets part of the screen area covered by the input method that
     * should be avoided by the application receiving input in order not to be
     * obscured.
     *
     * For now this region must be so simple that its bounding box can be
     * effectively used as the avoidance area.
     *
     * \param region the new region
     */
    virtual void setInputMethodArea(const QRegion &region) = 0;

    /*!
     *\brief Sets selection text from \a start with \a length in the application widget.
     */
    virtual void setSelection(int start, int length) = 0;

    /*!
     * \brief Locks application orientation.
     *
     * \deprecated Not implemented
     */
    virtual void setOrientationAngleLocked(bool lock) = 0;

public:
    /*!
     * \brief Return information about loaded input method plugins which could work in specified \a state.
     *
     * \sa MImPluginDesription
     */
    virtual QList<MImPluginDescription> pluginDescriptions(MInputMethod::HandlerState state) const = 0;

    /*!
     * \brief returns the current cursor position within the preedit region
     */
    virtual int preeditClickPos(bool &valid) const { Q_UNUSED(valid); return 0; }

    /*!
     * \brief Return information about enabled subviews which are neighbors
     * (previous and next) of current active subview.
     *
     * Previous subview is described by first list item, next
     * subview is defined by last list item.
     * Returned list is empty if there is exactly one enabled subview.
     *
     * \sa MImSubViewDescription
     */
    virtual QList<MImSubViewDescription> surroundingSubViewDescriptions(MInputMethod::HandlerState state) const;

    /*!
     * \brief Sets current input method language.
     * \param language ICU format locale ID string
     *
     * This can be used as a hint to determine text direction in input fields, for example.
     */
    virtual void setLanguage(const QString &language);

    /*!
      */
    virtual std::tr1::shared_ptr<Maliit::Server::SurfaceFactory> surfaceFactory();

private:
    Q_DISABLE_COPY(MAbstractInputMethodHost)
    Q_DECLARE_PRIVATE(MAbstractInputMethodHost)

    MAbstractInputMethodHostPrivate *d;
};

#endif

