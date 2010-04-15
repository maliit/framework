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

#ifndef MINPUTMETHODBASE_H
#define MINPUTMETHODBASE_H

#include <QObject>
#include <QEvent>
#include <MNamespace>
#include "mimhandlerstate.h"
#include "mimdirection.h"

class QRegion;
class QPoint;
class QRect;
class QGraphicsScene;
class MInputContextConnection;
class MInputMethodBasePrivate;

/*!
 * \brief MInputMethodBase is a base class for input method servers.
 * 
 * It defines the interface which input context connection classes can use for
 * passing commands received from the applications
 */
class MInputMethodBase: public QObject
{
    Q_OBJECT

public:
    /*! Constructor for input method base
     * \param icConnection input context connection class, not owned by input method base
     */
    MInputMethodBase(MInputContextConnection *icConnection, QObject *parent = 0);

    ~MInputMethodBase();

    /*! \brief Returns input context connection associated with this input method base
     */
    MInputContextConnection *inputContextConnection() const;

    /*! \brief Shows the input method.
     */
    virtual void show();

    /*! \brief Hides the input method
     */
    virtual void hide();

    /*! \brief Sets preedit string of the input method server
     */
    virtual void setPreedit(const QString &preeditString);

    /*! \brief Update input method server state
     */
    virtual void update();

    /*! \brief Resets input method server state
     */
    virtual void reset();

    /*! \brief Notifies input method server about mouse click on the preedit string
     */
    virtual void mouseClickedOnPreedit(const QPoint &pos, const QRect &preeditRect);

    /*! \brief Notifies that the focus widget in application changed visualization priority
     */
    virtual void visualizationPriorityChanged(bool priority);

    /*! \brief Target application changed orientation. Input method usually changes its
     *         own orientation according to this.
     */
    virtual void appOrientationChanged(int angle);

    /*! \brief Set copy/paste state for appropriate UI elements
     *  \param copyAvailable bool TRUE if text is selected
     *  \param pasteAvailable bool TRUE if clipboard content is not empty
     */
    virtual void setCopyPasteState(bool copyAvailable, bool pasteAvailable);

    /*!
     * \brief use a custom toolbar
     */
    virtual void setToolbar(const QString &);

    /*!
     * \brief Process a key event redirected from hardware keyboard to input method.
     *
     * This is called only if one has enabled redirection by calling
     * \a MInputContextConnection::setRedirectKeys.
     */
    virtual void processKeyEvent(QEvent::Type keyType, Qt::Key keyCode,
                                 Qt::KeyboardModifiers modifiers, const QString &text,
                                 bool autoRepeat, int count, quint32 nativeScanCode,
                                 quint32 nativeModifiers);

    /* \brief This method is called when keyboard status is changed:
     * hardware keyboard is opened or closed, BT keyboard is connected
     * or disconnected
     * \param state const QList<MIMHandlerState>& list of current states
     * for this plugin
     */
    virtual void setState(const QSet<MIMHandlerState> &state);

    /*! \brief This method is called when target client is changed.
     */
    virtual void clientChanged();

    /*!
     * \brief Switch context to given direction
     * \param direction Switching direction
     * \param enableAnimation Contains true if swipe should be animated
     */
    virtual void switchContext(M::InputMethodSwitchDirection direction, bool enableAnimation);


signals:
    /*!
     * Inform that the screen area covered by the input method has been changed.
     *
     * \param region the new region
     */
    void regionUpdated(const QRegion &region);

    /*!
     * Inform that the part of the screen area covered by the input method that
     * should be avoided by the screen area receiving input in order not to be
     * obscured has been changed.
     *
     * For now this region must be so simple that its bounding box can be
     * effectively used as the avoidance area.
     *
     * \param region the new region
     */
    void inputMethodAreaUpdated(const QRegion &region);

    /*!
     * Inform that plugin should be switched in according to \a direction.
     */
    void pluginSwitchRequired(M::InputMethodSwitchDirection direction);

    /*!
     * Inform that active plugin should be replaced with specified one.
     * \param pluginName Name orf plugin which will be activated
     */
    void pluginSwitchRequired(const QString &pluginName);

    /*!
     * Inform that active plugin should be replaced with another one
     * specified by settings.
     */
    void pluginSwitchRequired();

private:
    Q_DISABLE_COPY(MInputMethodBase)

    MInputMethodBasePrivate *d;
};

#endif
