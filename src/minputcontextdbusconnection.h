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

#ifndef MINPUTCONTEXTDBUSCONNECTION_H
#define MINPUTCONTEXTDBUSCONNECTION_H

#include <QDBusContext>
#include <QDBusObjectPath>
#include <QMap>
#include <QEvent>

#include "minputcontextconnection.h"

class QPoint;
class QString;
class QRegion;
class QVariant;

class MInputContextDBusConnectionPrivate;

Q_DECLARE_METATYPE(QRect);

/*!
  \brief MInputContextDBusConnection handles input context connection for DBus based
  input context
 */
class MInputContextDBusConnection: public MInputContextConnection, protected QDBusContext
{
    Q_OBJECT
    // FIXME: dui prefix
    Q_CLASSINFO("D-Bus Interface", "org.maemo.duiinputmethodserver1")

public:
    MInputContextDBusConnection();
    virtual ~MInputContextDBusConnection();

    //! Returns true if the connection to D-Bus is valid and the object is properly usable
    bool isValid();

    //! \reimp
    virtual void sendPreeditString(const QString &string, PreeditFace preeditFace = PreeditDefault);
    virtual void sendCommitString(const QString &string);
    virtual void sendKeyEvent(const QKeyEvent &keyEvent);
    virtual void notifyImInitiatedHiding();

    virtual int contentType(bool &valid);
    virtual bool correctionEnabled(bool &valid);
    virtual bool predictionEnabled(bool &valid);
    virtual bool autoCapitalizationEnabled(bool &valid);
    virtual void setGlobalCorrectionEnabled(bool);
    virtual bool surroundingText(QString &text, int &cursorPosition);
    virtual bool hasSelection(bool &valid);
    virtual int inputMethodMode(bool &valid);
    virtual QRect preeditRectangle(bool &valid);
    virtual void setRedirectKeys(bool enabled);
    virtual void copy();
    virtual void paste();
    //! \reimp_end

public slots:
    // communication w/ minputcontext

    //void setContextObject(QDBusObjectPath callbackObject);
    void setContextObject(QString callbackObject);

    //! sets the input to go to calling connection
    void activateContext();

    //! ipc method provided to the application, shows input method
    void showInputMethod();

    //! ipc method provided to the application, hides input method
    void hideInputMethod();

    //! ipc method provided to the application, signals mouse click on preedit
    void mouseClickedOnPreedit(const QPoint &pos, const QRect &preeditRect);

    //! ipc method provided to the application, sets preedit
    void setPreedit(const QString &text);

    void updateWidgetInformation(const QMap<QString, QVariant> &stateInformation, bool focusChanged);

    //! ipc method provided to the application, resets the input method
    void reset();

    /*!
     * \brief Target application changed orientation
     */
    void appOrientationChanged(int angle);

    virtual void updateInputMethodArea(const QRegion &region);

    /*! \brief Set copy/paste state for appropriate UI elements in the input method server
     *  \param copyAvailable bool TRUE if text is selected
     *  \param pasteAvailable bool TRUE if clipboard content is not empty
     */
    virtual void setCopyPasteState(bool copyAvailable, bool pasteAvailable);

    /*!
     * \brief Process a key event redirected from hardware keyboard to input method plugin(s).
     *
     * This is called only if one has enabled redirection by calling \a setRedirectKeys.
     */
    void processKeyEvent(QEvent::Type keyType, Qt::Key keyCode, Qt::KeyboardModifiers modifiers,
                         const QString &text, bool autoRepeat, int count, quint32 nativeScanCode,
                         quint32 nativeModifiers);

    /*!
     * \brief Register an input method toolbar which is defined in \a fileName with the unique identifier \a id.
     *  The \a id should be unique, and the \a fileName is the absolute file name of the custom toolbar.
     */
    void registerToolbar(int id, const QString &fileName);

    /*!
     * \brief Unregister an input method \a toolbar which unique identifier is \a id.
     */
    void unregisterToolbar(int id);

    /*!
     * \brief Sets the \a attribute for the \a item in the custom toolbar to \a value.
     */
    void setToolbarItemAttribute(int id, const QString &item, const QString &attribute, const QVariant &value);

    /*!
     * \brief get the X window id of the active app window
     */
    int winId(bool &valid);

private:
    /*!
     * \brief Maps the input toolbar identifier from local \a id to global \a globalId.
     * \return false if request is not from a valid client.
     */
    bool toGlobal(int id, qlonglong &globalId);

    //! Updates the transient hint on the framework side to point to the
    //! current application's window id.
    void updateTransientHint();


    Q_DISABLE_COPY(MInputContextDBusConnection)

    MInputContextDBusConnectionPrivate *d;
};


#endif
