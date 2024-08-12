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

#ifndef MABSTRACTINPUTMETHOD_H
#define MABSTRACTINPUTMETHOD_H

#include <QObject>
#include <QEvent>
#include <QVariant>
#include <QMap>

#include <maliit/namespace.h>

QT_BEGIN_NAMESPACE
class QRegion;
class QPoint;
class QRect;
QT_END_NAMESPACE
class MAbstractInputMethodHost;
class MAbstractInputMethodPrivate;
class MKeyOverride;


/*! \ingroup pluginapi
 * \brief A base class for input methods.
 *
 * Defines the interface which input method framework uses for passing commands
 * received from the applications to plugins. Communication in the other
 * direction, from the input method plugin to the framework, is done using the
 * MAbstractInputMethodHost object returned from inputMethodHost().
 */
class MAbstractInputMethod: public QObject
{
    Q_OBJECT

public:
     /*!
     * MInputMethodSubView defines an input method subview with an identifier \a subViewId
     * and a title \a subViewTitle.
     *
     * The subview is a view which provided by this input method. The view could be a view of one
     * language, or one keyboard, it depends on the input method. Each subview has an identifier
     * and title. The subview title will be shown in the input method settings, and
     * should be a localized string.
     */
    struct MInputMethodSubView {
        QString subViewId;
        QString subViewTitle;
    };

    /*! Constructor
     *
     * \param host serves as communication link to framework and application. Managed by framework.
     */
    explicit MAbstractInputMethod(MAbstractInputMethodHost *host);
    virtual ~MAbstractInputMethod();

    /*! \brief Returns input method host.
     */
    MAbstractInputMethodHost *inputMethodHost() const;

    /*! \brief Show request.
     *
     *  The input method should normally show its UI on this call, unless
     *  handleVisualizationPriorityChange(bool) was called with a true value.
     *  \sa handleVisualizationPriorityChange(bool)
     */
    virtual void show();

    /*! \brief Hide request.
     *
     *  The input method must hide itself when this is called.
     */
    virtual void hide();

    /*! \brief Notifies input method about predit string changes.
     *
     *  This method informs the input method about preedit and the cursor
     *  position. The input method can fully control the preedit
     *  and the cursor inside it: whether and where to show
     *  cursor inside preedit, and how the preedit string should be styled.
     *  \sa MAbstractInputMethodHost::sendPreeditString()
     *
     *  \param preeditString preedit string
     *  \param cursorPos the cursor position inside preedit.
     */
    virtual void setPreedit(const QString &preeditString, int cursorPos);

    /*! \brief State update notification.
     *
     *  General update notification. Called in addition to the specific methods.
     */
    virtual void update();

    /*! \brief Reset notification.
     */
    virtual void reset();

    /*! \brief Notifies input method about mouse click on the preedit string.
     *
     * Reimplementing this method is optional. It is used by Meego Keyboard.
     */
    virtual void handleMouseClickOnPreedit(const QPoint &pos, const QRect &preeditRect);

    /*! \brief Notifies input method about focus changes on application side.
     *
     *  Reimplementhing this method is optional.
     *
     *  \param focusIn true - focus has entered a widget, false - focus has left a widget
     */
    virtual void handleFocusChange(bool focusIn);

    /*! \brief Notifies that the focus widget in application changed visualization priority.
     *
     * This method is used by the framework to allow the input method to be dismissed while a widget is focused.
     * Further calls to show() when priority is true should not show the input method.
     * When priority is set to false again, and the input method is not in hidden state,
     * the input method should be shown.
     *
     * \param priority If true, the application has priority, and the input method should not be shown.
     */
    virtual void handleVisualizationPriorityChange(bool priority);

    /*! \brief Target application is about to change orientation.
     *
     * The input method usually changes its own orientation according to this.
     * Note that this method might not be called when the input method shown for the first time.
     * \sa handleAppOrientationChanged(int angle)
     *
     * \param angle The angle in degrees, clockwise. Possible values: 0, 90, 180, 270. 0 is the normal orientation of the display server.
     */
    virtual void handleAppOrientationAboutToChange(int angle);

    /*! \brief Target application already finish changing orientation.
     *
     * \param angle The angle in degrees, clockwise. Possible values: 0, 90, 180, 270. 0 is the normal orientation of the display server.
     */
    virtual void handleAppOrientationChanged(int angle);

    /*!
     * \brief Process a key event redirected from hardware keyboard to input method.
     *
     * This is called only if one has enabled redirection by calling
     * \a MInputContextConnection::setRedirectKeys.
     *
     * Reimplementing this method is optional. It can be used to implement input methods
     * that handle hardware keyboard.
     */
    virtual void processKeyEvent(QEvent::Type keyType, Qt::Key keyCode,
                                 Qt::KeyboardModifiers modifiers, const QString &text,
                                 bool autoRepeat, int count, quint32 nativeScanCode,
                                 quint32 nativeModifiers, unsigned long time);

    /*!
     * \brief This method is called to inform about keyboard status changes
     *
     * For example: Hardware keyboard is opened or closed, BT keyboard is connected or
     * disconnected.
     *
     * \param state set of current states for this plugin
     */
    virtual void setState(const QSet<Maliit::HandlerState> &state);

    /*! \brief This method is called when target client (application) has changed.
     */
    virtual void handleClientChange();

    /*!
     * \brief Switch context to given direction
     *
     * If the input method arranges the subviews horizontally (like Meego Keyboard does) it
     * should first try to change its subviews in the direction indicated. If there are no
     * more subviews in the given direction, MInputMethodHost::switchPlugin(Maliit::SwitchDirection )
     * should be called.
     *
     * \param direction Switching direction
     * \param enableAnimation Contains true if swipe should be animated
     */
    virtual void switchContext(Maliit::SwitchDirection direction, bool enableAnimation);

    /*! \brief Returns all subviews (IDs and titles) which are supported for \a state.
     *
     * Implement this function to return the subviews which are supported by this input
     * method for the specified state. An input method must return at least one subview
     * for a supported state.
     */
    virtual QList<MInputMethodSubView> subViews(Maliit::HandlerState state
                                                 = Maliit::OnScreen) const;

    /*!
     * \brief Sets \a subViewId as the active subview for \a state.
     *
     *  Implement this method to set the active subview. Input method plugins manager will call
     *  this method when active subview for specified state is changed from the input method
     *  settings.
     *
     * \param subViewId the identifier of subview.
     * \param state the state which \a subViewId belongs to.
     */
    virtual void setActiveSubView(const QString &subViewId,
                                  Maliit::HandlerState state = Maliit::OnScreen);

    /*!
     * \brief Returns current active subview ID for \a state.
     *
     *  Implement this method to inform input method plugins manager about current active subview
     *  for specified state.
     */
    virtual QString activeSubView(Maliit::HandlerState state = Maliit::OnScreen) const;

    /*! \brief Show notification to user informing about current language/subview
     *
     * Reimplementing this method is optional.
     */
    virtual void showLanguageNotification();

    /*!
     * \brief Uses custom key overrides which are defined by given parameter.
     *
     * Reimplementing this method is optional. It is used in Meego Keyboard
     * to provide context aware keys that can be customized from the application
     * side.
     *
     * \param overrides Pointer to key override definitions. An empty map means
     * that no key override exists, and that the normal values should be used.
     */
    virtual void setKeyOverrides(const QMap<QString, QSharedPointer<MKeyOverride> > &overrides);

Q_SIGNALS:
    /*!
     * \brief Inform that active subview is changed to \a subViewId for \a state.
     *
     * Must be emitted when plugin changes the active subview for specified state.
     *
     * \param subViewId the identifier of the new subview.
     * \param state the state which \a subViewId belongs to.
     */
    void activeSubViewChanged(const QString &subViewId,
                              Maliit::HandlerState state = Maliit::OnScreen);

private:
    Q_DISABLE_COPY(MAbstractInputMethod)
    Q_DECLARE_PRIVATE(MAbstractInputMethod)

    MAbstractInputMethodPrivate * const d_ptr;
};

#endif
