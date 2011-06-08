/* * This file is part of meego-im-framework *
 *
 * Copyright (C) 2010, 2011 Nokia Corporation and/or its subsidiary(-ies).
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

// Based on minputmethodstate.h from libmeegotouch

#ifndef MINPUTMETHODSTATE_H
#define MINPUTMETHODSTATE_H

#include <QObject>
#include <QRect>

#include "minputmethodnamespace.h"

class QKeyEvent;

class MInputMethodStatePrivate;

/*!
 * \brief A mediator between MInputContext and applications that use it
 *
 * This class allows MInputContext (technically other input contexts too) to
 * communicate input method area on display to the application and application
 * to communicate its active window's orientation and custom attribute extension to the input context.
 * The application can be a M application or a plain Qt application (which needs
 * to link against libmeegotouch to get access to this class).  The input method area
 * can be used by the application to avoid obstructing the input method.
 */
class MInputMethodState : public QObject
{
    Q_OBJECT

public:
    //! \brief Get singleton instance
    //! \return singleton instance
    static MInputMethodState *instance();

    //! \brief Get current input method area
    //! \return current input method area
    QRect inputMethodArea() const;

    //! \brief Get the orientation of application's active window
    //! \return orientation of application's active window
    MInputMethod::OrientationAngle activeWindowOrientationAngle() const;

    /*!
     * Sends a request to the application's input context to open a software
     * input panel (e.g. the virtual keyboard).
     * Warning: In order to gain more screen space, window decorations may be
     * temporarily hidden while the input panel is up.
     * \sa closeSoftwareInputPanel
     */
    static void requestSoftwareInputPanel();

    /*!
     * Sends a request to the application's input context to close a software
     * input panel.
     * \sa requestSoftwareInputPanel
     */
    static void closeSoftwareInputPanel();

    //! \internal
    void emitKeyPress(const QKeyEvent &event);
    void emitKeyRelease(const QKeyEvent &event);

    // providing target name -> (item name -> (attribute name -> attribute value))
    typedef QMap<QString, QVariant> AttributeMap;
    typedef QMap<QString, AttributeMap> ItemAttributeMap;
    typedef QMap<QString, ItemAttributeMap> ExtendedAttributeMap;

    // returns the identifiers of registered attribute extensions
    QList<int> attributeExtensionIds() const;

    // returns all the registered extended attributes for \a id.
    ExtendedAttributeMap extendedAttributes(int id) const;
    //! \internal_end

public Q_SLOTS:
    /*!
     * \brief Application's active window start to rotate to \a newOrientationAngle.
     *
     * \note This method should be called when active window start rotation animation.
     */
    void startActiveWindowOrientationAngleChange(MInputMethod::OrientationAngle newOrientationAngle);

    /*!
     * \brief Set the orientation angle of application's active window to \a newOrientationAngle
     *
     * \note This method should be called when active window already finished the rotation animation.
     */
    void setActiveWindowOrientationAngle(MInputMethod::OrientationAngle newOrientationAngle);

    //! \brief Set input method area to \a newRegion
    void setInputMethodArea(const QRect &newRegion);

    /*!
     * \brief Registers an attribute extension (toolbar contents, keyboard overrides) which is defined in \a fileName, and will be assigned to a text input widget (software input panel).
     * \param \a fileName is the absolute file name of an attribute extension, or the relative file under
     *      /usr/share/meegoimframework/imattributeextensions/.
     * \note if \a fileName is an empty string, then it means don't load any content, just get a new id.
     * \return the registered unique identifier of this attribute extension.
     */
    int registerAttributeExtension(const QString &fileName = QString());

    /*!
     * \brief Unregister the attribute extension which unique id is \a id.
     * \param \a id is the unique identifier of the registered attribute extension.
     */
    void unregisterAttributeExtension(int id);

    /*!
     * \brief Set the \a attribute of the \a targetItem in the input method registered attribute extension \a target which has the unique \a id to \a value.
     * \param id the unique identifier of a registered extended attribute.
     * \param target a string specifying the target for the attribute.
     * \param targetItem the item name.
     * \param attribute attribute to be changed.
     * \param value new value.
     *
     * \note
     *  Every target name beings with a slash character ('/').
     *  The target "/" is the root target, and shall contain attributes that affect the whole input methods framework.
     *  Target "/keys" is used for virtual keyboard keys, "/toolbar" for toolbar contents.
     *  Additional targets can be added in the future.
     */
    void setExtendedAttribute(int id, const QString &target, const QString &targetItem,
                              const QString &attribute, const QVariant &value);

    /*!
     * \brief Returns the attribute extension definition file name which is registered with the unique identifier \id.
     * \param id the unique identifier of a registered attribute extension.
     * \return the file name of the attribute extension, or empty QString if \a id is invalid.
     */
    QString attributeExtensionFile(int id) const;

Q_SIGNALS:
    /*! \brief Emitted when input method area is changed
     *
     *  If applications want to react to this signal by changing their layout (e.g. to
     *  one that works better when a input method panel is around) they have to do their
     *  layout changes immediately upon invocation of this signal. Therefore queued
     *  connections (like Qt::QueuedConnection) should not be used for that purpose.
     *
     *  \param region new input method area
     */
    void inputMethodAreaChanged(const QRect &region);

    /*!
     * \brief This signal is emitted before the orientation angle of the application's active window is changed.
     * \param orientationAngle New orientation angle which is about to change.
     *
     * \note It is emitted at the start of the rotation animation.
     */
    void activeWindowOrientationAngleAboutToChange(MInputMethod::OrientationAngle orientationAngle);

    /*!
     * \brief This signal is emitted when the orientation angle of application's active window is changed
     * \param orientationAngle new orientation angle
     *
     * \note It's emitted with every orientation angle change finished.
     */
    void activeWindowOrientationAngleChanged(MInputMethod::OrientationAngle orientationAngle);

    //! Can be emitted by input method to notify about raw key press event it receives.
    void keyPress(const QKeyEvent &);

    //! Can be emitted by input method to notify about raw key release event it receives.
    void keyRelease(const QKeyEvent &);

    //! Emitted when an input method attribute extension which is defined in \a fileName with an unique identifier \a id is registered.
    void attributeExtensionRegistered(int id, const QString &fileName);

    //! Emitted when an input method attribute extension with an unique \a id is unregistered.
    void attributeExtensionUnregistered(int id);

    //! Emitted when input method extended attribute is changed.
    void extendedAttributeChanged(int id, const QString &target, const QString &targetItem,
                                  const QString &attribute, const QVariant &value);

private:
    //! Disable default construction
    MInputMethodState();
    ~MInputMethodState();
    Q_DISABLE_COPY(MInputMethodState)

    MInputMethodStatePrivate *const d_ptr;

    Q_DECLARE_PRIVATE(MInputMethodState)
};

#endif
