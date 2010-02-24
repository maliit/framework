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

#ifndef DUIINPUTCONTEXT_H
#define DUIINPUTCONTEXT_H

#include <QObject>
#include <QInputContext>
#include <QTimer>
#include <QPointer>

#include <duinamespace.h>

class DuiPreeditStyleContainer;
class QDBusInterface;
class QDBusConnectionInterface;


/*!
 * \brief DuiInputContext implements application side input method support for Dui input method server.
 *
 * The base class QInputContext abstracts input method specific composing of input. It acts
 * in between the input method and the widget receiving the composed input. DuiInputContext
 * extends this functionality by connecting to Dui input method server and accepting input it
 * creates. This input is forwarded to currently focused widget. DuiInputContext also notifies
 * Dui input method server about the current state of input widgets in the application side.
 */
class DuiInputContext: public QInputContext
{
    Q_OBJECT
    friend class Ut_DuiInputContext;

public:
    class WidgetInfo
    {
    public:
        WidgetInfo();

        bool valid;
        bool correctionEnabled;
        bool predictionEnabled;
        bool autoCapitalizationEnabled;
        Dui::TextContentType contentType;
        Dui::InputMethodMode inputMethodMode;
        QRect preeditRectangle;
    };

    //! \brief Constructor
    explicit DuiInputContext(QObject *parent = 0);

    //! \brief Destructor
    virtual ~DuiInputContext();

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
    //! \reimp_end

    // methods to be used from input method server side:

    // \brief Notifies about lost activation.
    virtual void activationLostEvent();

    //! \brief Notifies about hiding initiated by the input method server side
    virtual void imInitiatedHide();

    //!
    // \brief Commits a string to current focus widget
    // \param string    The new string committed
    virtual void commitString(const QString &string);

    //!
    // \brief Updates preedit string of the current focus widget
    // \param string    The new string
    // \param preeditFace Selects style for preedit string
    virtual void updatePreedit(const QString &string, Dui::PreeditFace preeditFace);

    //! \brief Sends a non-printable key event. Parameters as in QKeyEvent constructor
    virtual void keyEvent(int type, int key, int modifiers, const QString &text, bool autoRepeat,
                          int count);

    //!
    // \brief Updates the input method window area
    // \param rectList  List of rectangles which are part of the input method area
    virtual void updateInputMethodArea(const QList<QVariant> &rectList);

    /*!
     * \brief set global correction option enable/disable
     */
    virtual void setGlobalCorrectionEnabled(bool);

    /*!
     * \brief get focused widget information
     */
    virtual WidgetInfo getFocusWidgetInfo();

    /*!
     * \brief get surrounding text and cursor position information
     */
    virtual bool surroundingText(QString &text, int &cursorPosition);

    /*!
     * \brief Returns true if there is selection text
     * \param valid if the query is failed, \a valid will be set to false, otherwise true.
     */
    virtual bool hasSelection(bool &valid) const;

    /*! get rectangle covering preedit
     * \param valid validity for the return value
     */
    virtual QRect preeditRectangle(bool &valid) const;

    /*!
     * \brief Sends copy command to text editor.
     * This method tries to call "copy" slot in the focused widget
     * and sends QKeyEvent corresponding to Ctrl-C if slot can not be called.
     */
    void copy();

    /*!
     * \brief Sends paste command to text editor.
     * This method tries to call "paste" slot in the focused widget
     * and sends QKeyEvent corresponding to Ctrl-V if slot can not be called.
     */
    void paste();

    /*!
     * \brief Set if the input method wants to process all raw key events
     * from hardware keyboard (via \a processKeyEvent calls).
     */
    virtual void setRedirectKeys(bool enabled);

private slots:
    //! hides input method
    void hideOnFocusOut();

    //! looks after service owner changes and re-registers to changed input method server
    void serviceChangeHandler(const QString &name, const QString &oldOwner,
                              const QString &newOwner);

    /*!
     * \brief Notifies input method server of copy/paste availability.
     * \param copyAvailable bool Specifies wheter copy is available
     */
    void manageCopyPasteState(bool copyAvailable);

    //! Notify input method plugin about new \a orientation angle of application's active window.
    void notifyOrientationChange(Dui::OrientationAngle orientation);

private:
    Q_DISABLE_COPY(DuiInputContext)

    enum InputPanelState {
        InputPanelShowPending,   // input panel showing requested, but activation pending
        InputPanelShown,         // panel showing
        InputPanelHidden         // panel hidden
    };

    void connectToDBus();

    //! registers the application to the input method server
    void registerContextObject();

    // returns true if given widget supports input methods, widget can be null
    bool isInputEnabled(QWidget *widget) const;

    // returns content type corresponding to specified hints
    Dui::TextContentType contentType(Qt::InputMethodHints hints) const;

    //! returns state for currently focused widget, key is attribute name.
    QMap<QString, QVariant> getStateInformation() const;

    bool active; // is connection active
    InputPanelState inputPanelState; // state for the input method server's software input panel

    /*! Timer for hiding the current Software Input Panel.
     *  This is mainly for switching directly between widgets
     *  that has input method enabled.
     *  The delayed hiding is handled in input-context because
     *  this has to work also with plain qt apps.
     */
    QTimer sipHideTimer;

    QDBusInterface *iface;
    bool ownsDuiComponentData;

    bool correctionEnabled;

    DuiPreeditStyleContainer *styleContainer;

    QPointer<QObject> connectedObject;
    bool pasteAvailable;
    bool copyAllowed;
    //! redirect all hw key events to the input method or not
    bool redirectKeys;
};

#endif
