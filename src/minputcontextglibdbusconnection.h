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

#ifndef MINPUTCONTEXTGLIBDBUSCONNECTION_H
#define MINPUTCONTEXTGLIBDBUSCONNECTION_H

#include "minputcontextconnection.h"
#include "mtoolbarid.h"

#include <QByteArray>
#include <QMap>
#include <QSet>
#include <QEvent>
#include <QString>
#include <QVariant>
#include <glib.h>

#include <dbus/dbus-glib-lowlevel.h>
#include <dbus/dbus-glib.h>
#include <dbus/dbus.h>

class QPoint;
class QRegion;
struct MDBusGlibICConnection;
struct MIMSDBusActivater;

//! \internal
/*! \brief Peer-to-peer DBus input context connection based on glib dbus bindings
 */
class MInputContextGlibDBusConnection : public MInputContextConnection
{
    Q_OBJECT

public:
    MInputContextGlibDBusConnection();
    virtual ~MInputContextGlibDBusConnection();

    void handleDBusDisconnection(MDBusGlibICConnection *connection);

    //! \reimp
    virtual void sendPreeditString(const QString &string,
                                   const QList<MInputMethod::PreeditTextFormat> &preeditFormats,
                                   int replacementStart = 0, int replacementLength = 0,
                                   int cursorPos = -1);
    virtual void sendCommitString(const QString &string, int replaceStart = 0,
                                  int replaceLength = 0, int cursorPos = -1);
    virtual void sendKeyEvent(const QKeyEvent &keyEvent,
                              MInputMethod::EventRequestType requestType);
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
    virtual QRect cursorRectangle(bool &valid);
    virtual bool hiddenText(bool &valid);
    virtual void setRedirectKeys(bool enabled);
    virtual void setDetectableAutoRepeat(bool enabled);
    virtual void copy();
    virtual void paste();
    virtual void setSelection(int start, int length);
    virtual void setOrientationAngleLocked(bool lock);
    virtual int anchorPosition(bool &valid);
    virtual QString selection(bool &valid);
    //! \reimp_end

public slots:
    // communication w/ minputcontext

    //! sets the input to go to calling connection
    void activateContext(MDBusGlibICConnection *obj);

    //! ipc method provided to the application, shows input method
    void showInputMethod();

    //! ipc method provided to the application, hides input method
    void hideInputMethod();

    //! ipc method provided to the application, signals mouse click on preedit
    void mouseClickedOnPreedit(const QPoint &pos, const QRect &preeditRect);

    //! ipc method provided to the application, sets preedit
    void setPreedit(const QString &text, int cursorPos);

    void updateWidgetInformation(MDBusGlibICConnection *connection,
                                 const QMap<QString, QVariant> &stateInformation,
                                 bool focusChanged);

    //! ipc method provided to the application, resets the input method
    void reset();

    /*!
     * \brief Target application is changing orientation
     */
    void appOrientationAboutToChange(int angle);

    /*!
     * \brief Target application changed orientation (already finished)
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
                         quint32 nativeModifiers, unsigned long time);

    /*!
     * \brief Register an input method toolbar which is defined in \a fileName with the
     * unique identifier \a id.
     *
     *  The \a id should be unique, and the \a fileName is the absolute file name of the
     *  custom toolbar.
     */
    void registerToolbar(MDBusGlibICConnection *connection, int id, const QString &fileName);

    /*!
     * \brief Unregister an input method \a toolbar which unique identifier is \a id.
     */
    void unregisterToolbar(MDBusGlibICConnection *connection, int id);

    /*!
     * \brief Sets the \a attribute for the \a item in the custom toolbar to \a value.
     */
    void setToolbarItemAttribute(MDBusGlibICConnection *connection, int id, const QString &item,
                                 const QString &attribute, const QVariant &value);

    /*!
     * \brief get the X window id of the active app window
     */
    WId winId(bool &valid);

private:
    //! Updates the transient hint on the framework side to point to the
    //! current application's window id.
    void updateTransientHint();

    MDBusGlibICConnection *activeContext;
    QMap<QString, QVariant> widgetState;
    bool globalCorrectionEnabled;
    bool redirectionEnabled;
    bool detectableAutoRepeat;
    MToolbarId toolbarId; //current toolbar id
    QSet<MToolbarId> toolbarIds; //all toolbar ids
    QByteArray socketAddress;

    DBusServer *server;

    Q_DISABLE_COPY(MInputContextGlibDBusConnection)
};
//! \internal_end


#endif
