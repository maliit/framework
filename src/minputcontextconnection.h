/* * This file is part of dui-im-framework *
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

#ifndef DUIINPUTCONTEXTCONNECTION_H
#define DUIINPUTCONTEXTCONNECTION_H

#include "duipreeditface.h"
#include <QRect>
#include <QObject>

class QString;
class QRegion;
class QKeyEvent;

class DuiInputContextConnectionPrivate;
class DuiInputMethodBase;


/*!
 * \brief DuiInputContextConnection is a base class of the input method communication implementation
 * between the input context and the input method server.
 */
class DuiInputContextConnection: public QObject
{
    Q_OBJECT

public:
    DuiInputContextConnection();
    virtual ~DuiInputContextConnection();

    /*!
     * \brief Adds the target which get called on incoming requests
     */
    virtual void addTarget(DuiInputMethodBase *target);

    /*!
     * \brief Removes the handler of the connection
     */
    virtual void removeTarget(DuiInputMethodBase *target);

    /*!
     * \brief returns content type for focused widget if output parameter valid is true,
     * value matches enum Dui::TextContentType
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
     * Implement this method to update the pre-edit string
     * \param string    The new pre-edit string
     * \param preeditFace Selects visual styling for preedit
     */
    virtual void sendPreeditString(const QString &string,
                                   PreeditFace preeditFace = PreeditDefault) = 0;

    /*!
     * \brief Updates commit string in the application widget
     *
     * Implement this method to update the commit string
     * \param string    The string to be committed
     */
    virtual void sendCommitString(const QString &string) = 0;

    /*!
     * \brief Sends key event to the application
     */
    virtual void sendKeyEvent(const QKeyEvent &keyEvent) = 0;

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
     * \brief set global correction option enable/disable
     */
    virtual void setGlobalCorrectionEnabled(bool) = 0;

public slots:

    //! Update \a region covered by virtual keyboard
    virtual void updateInputMethodArea(const QRegion &region);

protected:
    QSet<DuiInputMethodBase *> targets();

private:
    DuiInputContextConnectionPrivate *d;
};


#endif
