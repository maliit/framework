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

#ifndef MALIIT_INPUTMETHOD_H
#define MALIIT_INPUTMETHOD_H

#include <QObject>
#include <QRect>
#include <QScopedPointer>
#include <maliit/namespace.h>

class QKeyEvent;

namespace Maliit {

class InputMethodPrivate;

/*! \ingroup libmaliit
 * \brief Listen to device orientation, language and raw key events.
 */
class InputMethod : public QObject
{
    Q_OBJECT

public:
    //! \brief Get singleton instance
    //!
    //! Example of using the singleton InputMethod instance:
    //! \snippet plainqt/mainwindow.cpp Get singleton InputMethod instance
    //! \return singleton instance
    static InputMethod *instance();

    //! \brief Get current input method area
    //! \return current input method area
    QRect area() const;

    //! \brief Get the orientation of application's active window
    //! \return orientation of application's active window
    Maliit::OrientationAngle orientationAngle() const;

    //! \internal
    void emitKeyPress(const QKeyEvent &event);
    void emitKeyRelease(const QKeyEvent &event);
    void setLanguage(const QString &language);
    //! \internal_end

    //! \brief Current language of active input method
    const QString &language() const;

public Q_SLOTS:
    /*!
     * \brief Application's active window start to rotate to \a newOrientationAngle.
     *
     * \note This method should be called when active window start rotation animation.
     */
    void startOrientationAngleChange(Maliit::OrientationAngle newOrientationAngle);

    /*!
     * \brief Set the orientation angle of application's active window to \a newOrientationAngle
     *
     * \note This method should be called when active window already finished the rotation animation.
     */
    void setOrientationAngle(Maliit::OrientationAngle newOrientationAngle);

    //! \brief Set input method area to \a newRegion
    void setArea(const QRect &newRegion);

Q_SIGNALS:
    /*!
     * \brief Emitted when input method area is changed
     *
     * \warning This signal is consumed by the input widget relocator inside
     * libmeegotouch, and can be emitted at any time, so in order to avoid
     * conflicts, an application that does any custom relayout must not use a
     * queued connection (like Qt::QueuedConnection).
     *
     * \param region new input method area
     */
    void areaChanged(const QRect &region);

    /*!
     * \brief This signal is emitted before the orientation angle of the application's active window is changed.
     * \param orientationAngle New orientation angle which is about to change.
     *
     * \note It is emitted at the start of the rotation animation.
     */
    void orientationAngleAboutToChange(Maliit::OrientationAngle orientationAngle);

    /*!
     * \brief This signal is emitted when the orientation angle of application's active window is changed
     * \param orientationAngle new orientation angle
     *
     * \note It's emitted with every orientation angle change finished.
     */
    void orientationAngleChanged(Maliit::OrientationAngle orientationAngle);

    //! Can be emitted by input method to notify about raw key press event it receives.
    void keyPress(const QKeyEvent &);

    //! Can be emitted by input method to notify about raw key release event it receives.
    void keyRelease(const QKeyEvent &);

    /*!
     * \brief Emitted on input method language change.
     * \sa language()
     */
    void languageChanged(const QString &language);

private:
    InputMethod();
    ~InputMethod();
    Q_DISABLE_COPY(InputMethod)

    const QScopedPointer<InputMethodPrivate> d_ptr;

    Q_DECLARE_PRIVATE(InputMethod)
};

/*!
 * \brief Sends a request to the application's input context to open a software
 * input panel (e.g. the virtual keyboard).
 *
 * \warning In order to gain more screen space, window decorations may be
 * temporarily hidden while the input panel is up.
 * \sa closeInputMethodPanel
 */
void requestInputMethodPanel();

/*!
 * \brief Sends a request to the application's input context to close a software
 * input panel.
 * \sa requestInputMethodPanel
 */
void closeInputMethodPanel();

} // namespace Maliit

#endif // MALIIT_INPUTMETHOD_H
