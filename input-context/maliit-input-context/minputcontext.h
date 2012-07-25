/* * This file is part of Maliit framework *
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 *
 * Contact: maliit-discuss@lists.maliit.org
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 */

#ifndef MINPUTCONTEXT_H
#define MINPUTCONTEXT_H

#include <maliit/namespace.h>

#include <QObject>
#include <QInputContext>
#include <QTimer>
#include <QPointer>

class QGraphicsItem;
class QGraphicsView;
class MImServerConnection;

/*!
 * \brief On the application side, MInputContext implements input method
 *        support for the Meegotouch input method server.
 *
 * The base class QInputContext abstracts input method specific composing of
 * input. It acts in between the input method and the widget receiving the
 * composed input. MInputContext extends this functionality by connecting to
 * Meegotouch input method server and accepting input it creates. This input
 * is forwarded to the currently focused widget. MInputContext also notifies
 * the Meegotouch input method server about the current state of input widgets
 * on the application side.
 */
class MInputContext : public QInputContext
{
    Q_OBJECT
    friend class Ut_MInputContext;

public:
    //! \brief Constructor
    explicit MInputContext(QSharedPointer<MImServerConnection> imServer, const QString &name, QObject *parent = 0);

    //! \brief Destructor
    virtual ~MInputContext();

    //! \reimp
    virtual bool event(QEvent *event);
    virtual QString identifierName();
    virtual bool isComposing() const;
    virtual QString language();
    virtual void reset();
    virtual void update();
    virtual void mouseHandler(int x, QMouseEvent *event);
    virtual void setFocusWidget(QWidget *w); // note: qt marks this as internal
    virtual bool filterEvent(const QEvent *event);

#ifdef Q_WS_X11
    virtual bool x11FilterEvent(QWidget *widget, XEvent *event);
#endif
    //! \reimp_end

public Q_SLOTS:
    /* Hooked up to the input method server.
     * See MImServerConnection for documentation. */
    void activationLostEvent();
    void imInitiatedHide();

    void commitString(const QString &string, int replacementStart = 0,
                              int replacementLength = 0, int cursorPos = -1);

    void updatePreedit(const QString &string, const QList<Maliit::PreeditTextFormat> &preeditFormats,
                               int replacementStart = 0, int replacementLength = 0, int cursorPos = -1);

    void keyEvent(int type, int key, int modifiers, const QString &text, bool autoRepeat,
                          int count, Maliit::EventRequestType requestType
                          = Maliit::EventRequestBoth);

    void updateInputMethodArea(const QRect &rect);

    void setGlobalCorrectionEnabled(bool);

    void getPreeditRectangle(QRect &rectangle, bool &valid) const;

    Q_SLOT void onInvokeAction(const QString &action, const QKeySequence &sequence);

    void setRedirectKeys(bool enabled);

    void setDetectableAutoRepeat(bool enabled);

    void setSelection(int start, int length);

    void getSelection(QString &selection, bool &valid) const;

    void setLanguage(const QString &language);
    /* End input method server connection slots. */

public:
    static bool debug;

Q_SIGNALS:
    //! \internal
    /*! 
     * \brief Emitted when input method area is changed
     * \param region new input method area
     */
    void inputMethodAreaChanged(const QRect &region);
    //! \internal_end

private Q_SLOTS:
    //! hides input method
    void hideInputMethod();

    void onDBusDisconnection();
    void onDBusConnection();

    /*!
     * \brief Notifies input method server of copy availability.
     * \param copyAvailable bool Specifies wheter copy is available
     */
    void handleCopyAvailabilityChange(bool copyAvailable);

    /*!
     * \brief Notifies input method server when the QML TextInput element has selected text changes
     */
    void handleSelectedTextChange();

    /*!
     * \brief Notifies input method server when the QLineEdit object's selection changed
     */
    void handleSelectionChanged();

    //! Notify input method plugin about the application's active window prepare to change to a new \a orientation angle.
    void notifyOrientationAboutToChange(Maliit::OrientationAngle orientation);

    //! Notify input method plugin about new \a orientation angle of application's active window.
    //! \note this method is called when the orientation change is finished
    void notifyOrientationChanged(Maliit::OrientationAngle orientation);

    //! Checks whether there is someting to paste in the clipboard and notifies input method plugin about it.
    void handleClipboardDataChange();

private:
    Q_DISABLE_COPY(MInputContext)

    enum InputPanelState {
        InputPanelShowPending,   // input panel showing requested, but activation pending
        InputPanelShown,         // panel showing
        InputPanelHidden         // panel hidden
    };

    void updatePreeditInternally(const QString &string,
                                 const QList<Maliit::PreeditTextFormat> &preeditFormats,
                                 int replacementStart = 0, int replacementLength = 0, int cursorPos = -1);

    /* Hook up signals on the imServer to our slots. Used in constructor. */
    void connectInputMethodServer();
    /* Hook up signals and slots on the input method extension instance,
     * (MInputMethodState and Maliit::InputMethod). Used in constructor. */
    void connectInputMethodExtension();

    void notifyCopyPasteState();

    //! returns content type corresponding to specified hints
    Maliit::TextContentType contentType(Qt::InputMethodHints hints) const;

    //! returns the D-Bus object path for this instance
    QString dbusObjectPath() const;

    //! returns state for currently focused widget, key is attribute name.
    QMap<QString, QVariant> getStateInformation() const;

    //! finds focus scope item for item
    static QGraphicsItem *findFocusScopeItem(QGraphicsItem *item);

    bool isVisible(const QRect &rect, const QGraphicsView *view, const QGraphicsItem *item) const;

    template<typename PreeditInjectionEvent>
    bool handlePreeditInjectionEvent(const PreeditInjectionEvent *event);

    //! Gets cursor start position, relative to widget surrounding text.
    //! Parameter valid set to false on failure.
    int cursorStartPosition(bool *valid);

    bool active; // is connection active
    InputPanelState inputPanelState; // state for the input method server's software input panel

    /*! Timer for hiding the current Software Input Panel.
     *  This is mainly for switching directly between widgets
     *  that has input method enabled.
     *  The delayed hiding is handled in input-context because
     *  this has to work also with plain qt apps.
     */
    QTimer sipHideTimer;

    QSharedPointer<MImServerConnection> imServer;

    bool correctionEnabled;

    QString preedit;
    int preeditCursorPos;

    QPointer<QObject> connectedObject;
    bool pasteAvailable;
    bool copyAvailable;
    bool copyAllowed;
    //! redirect all hw key events to the input method or not
    bool redirectKeys;
    const QString objectPath; //!< D-Bus object path for this instance.
    static int connectionCount; //!< Counter to allow multiple IC objects being registered via D-Bus

    unsigned long currentKeyEventTime;

    QString m_language;
    QString m_name;
};

#endif
