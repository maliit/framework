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
#include <QVariant>
#include <QMap>
#include <MNamespace>

#include "minputmethodnamespace.h"

class QRegion;
class QPoint;
class QRect;
class QGraphicsScene;
class MInputContextConnection;
class MInputMethodBasePrivate;
class MToolbarData;

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
     /*!
      * This enum contains possible values for all the modes that are shown in the
      * Input mode indicator.
      *
      * \sa sendInputModeIndicator().
      */
    enum InputModeIndicator {
        NoIndicator,    //!< No indicator should be shown
        LatinLower,     //!< Latin lower case mode
        LatinUpper,     //!< Latin upper case mode
        LatinLocked,    //!< Latin caps locked mode
        CyrillicLower,  //!< Cyrillic lower case mode
        CyrillicUpper,  //!< Cyrillic upper case mode
        CyrillicLocked, //!< Cyrillic caps locked mode
        Arabic,         //!< Arabic mode
        Pinyin,         //!< Pinyin mode
        Zhuyin,         //!< Zhuyin mode
        Cangjie,        //!< Cangjie mode
        NumAndSymLatched,   //!< Number and Symbol latched mode
        NumAndSymLocked,//!< Number and Symbol locked mode
        DeadKeyAcute,   //!< Dead key acute mode
        DeadKeyCaron,   //!< Dead key caron mode
        DeadKeyCircumflex,  //!< Dead key circumflex mode
        DeadKeyDiaeresis,   //!< Dead key diaeresis mode
        DeadKeyGrave,   //!< Dead key grave mode
        DeadKeyTilde    //!< Dead key tilde mode
    };

     /*!
     * MInputMethodSubView defines an input method subview with an identifier \a subViewId
     * and a title \a subViewTitle.
     *
     * The subview is a view which provided by this input method. The view could be a view of one
     * language, or one keyboard, it depends on the input method. Each subview has an identifier
     * and title.
     */
    struct MInputMethodSubView {
        QString subViewId;
        QString subViewTitle;
    };

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

    /*! \brief Notifies input method server about changed focus
     *  \param focusIn true - focus has entered a widget, false - focus has left a widget
     */
    virtual void focusChanged(bool focusIn);

    /*! \brief Notifies that the focus widget in application changed visualization priority
     */
    virtual void visualizationPriorityChanged(bool priority);

    /*! \brief Target application changed orientation. Input method usually changes its
     *         own orientation according to this.
     */
    virtual void appOrientationChanged(int angle);

    /*!
     * \brief Uses a custom toolbar which is defined by given parameter.
     * \param toolbar Pointer to toolbar definition.
     */
    virtual void setToolbar(QSharedPointer<const MToolbarData> toolbar);

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

    /* \brief This method is called to inform about keyboard status changes
     *
     * That is, hardware keyboard is opened or closed, BT keyboard is connected or
     * disconnected
     * \param state set of current states for this plugin
     */
    virtual void setState(const QSet<MInputMethod::HandlerState> &state);

    /*! \brief This method is called when target client is changed.
     */
    virtual void clientChanged();

    /*!
     * \brief Switch context to given direction
     * \param direction Switching direction
     * \param enableAnimation Contains true if swipe should be animated
     */
    virtual void switchContext(MInputMethod::SwitchDirection direction, bool enableAnimation);

    /*!
     * \brief Sends input mode indicate state.
     * Default implementation is sending the input mode indicate state \a mode to application framework.
     * \param mode Input mode indicator state.
     * \sa InputModeIndicator.
     */
    void sendInputModeIndicator(InputModeIndicator mode);

    /*! \brief Returns all subviews (IDs and titles) which are supported for \a state.
     *
     * Implement this function to return the subviews which are supported by this input
     * method for the specified state. The subview titles will be shown in the input method settings.
     */
    virtual QList<MInputMethodSubView> subViews(MInputMethod::HandlerState state
                                                 = MInputMethod::OnScreen) const;

    /*!
     * \brief Sets \a subViewId as the active subview for \a state.
     * \param subViewId the identifier of subview.
     * \param state the state which \a subViewId belongs to.
     *
     *  Implement this method to set the active subview. Input method plugins manager will call
     *  this method when active subview for specified state is changed from the input method
     *  settings.
     */
    virtual void setActiveSubView(const QString &subViewId,
                                  MInputMethod::HandlerState state = MInputMethod::OnScreen);

    /*!
     * \brief Returns current active subview ID for \a state.
     *
     *  Implement this method to inform input method plugins manager about current active subview
     *  for specified state.
     */
    virtual QString activeSubView(MInputMethod::HandlerState state = MInputMethod::OnScreen) const;

    //! Show notification informing about current language
    virtual void showLanguageNotification();

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
    void pluginSwitchRequired(MInputMethod::SwitchDirection direction);

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

    /*!
     * Inform that input method settings should be shown.
     * Emitted when the plugin requires input method settings.
     */
    void settingsRequested();

    /*!
     * Inform that active subview is changed to \a subViewId for \a state.
     * \param subViewId the identifier of the new subview.
     * \param state the state which \a subViewId belongs to.
     * Emitted when plugin changes the active subview for specified state.
     */
    void activeSubViewChanged(const QString &subViewId,
                              MInputMethod::HandlerState state = MInputMethod::OnScreen);

private:
    Q_DISABLE_COPY(MInputMethodBase)
    Q_DECLARE_PRIVATE(MInputMethodBase)
    Q_PRIVATE_SLOT(d_func(), void _q_handleIndicatorServiceChanged(const QString &, const QString &, const QString &));

    MInputMethodBasePrivate * const d_ptr;
};

#endif
