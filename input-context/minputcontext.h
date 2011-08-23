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

#ifndef MINPUTCONTEXT_H
#define MINPUTCONTEXT_H

#ifdef HAVE_MEEGOTOUCH
#include <MNamespace>
#endif

#include <minputmethodnamespace.h>
#include <maliit/namespace.h>

#include <QObject>
#include <QInputContext>
#include <QTimer>
#include <QPointer>

class MPreeditStyleContainer;
class GlibDBusIMServerProxy;
class QGraphicsItem;
class QGraphicsView;
typedef GlibDBusIMServerProxy DBusIMServerProxy;

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
    explicit MInputContext(QObject *parent = 0);

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
    virtual bool x11FilterEvent(QWidget *widget, XEvent *event);
    //! \reimp_end

    // methods to be used from input method server side:

    // \brief Notifies about lost activation.
    virtual void activationLostEvent();

    //! \brief Notifies about hiding initiated by the input method server side
    virtual void imInitiatedHide();

    /*!
     * \brief Commits a string to current focus widget, and set cursor position.
     * \param string    The new string committed
     * \param replacementStart The position at which characters are to be replaced relative
     *  from the start of the preedit string.
     * \param replacementLength The number of characters to be replaced in the preedit string.
     * \param cursorPos The cursor position to be set. the cursorPos is the position relative
     *  to commit string start. Negative values are used as commit string end position
     *
     * Note: If \a replacementLength is 0, \a replacementStart gives the insertion position
     * for the inserted \a string.
     * For example, if the replacement starting at -1 with a length of 2, then application will
     * remove the last character before the preedit string and the first character afterwards,
     * and insert the commit string directly before the preedit string.
     */
    virtual void commitString(const QString &string, int replacementStart = 0,
                              int replacementLength = 0, int cursorPos = -1);

    /*!
     * \brief Updates preedit string of the current focus widget
     * \param string    The new string
     * \param preeditFormats The formats for each part of preedit.
     * \param replacementStart The position at which characters are to be replaced relative
     *  from the start of the preedit string.
     * \param replacementLength The number of characters to be replaced in the preedit string.
     * \param cursorPos Cursor position. If it is less than 0, then the cursor will be hidden.
     *
     */
    virtual void updatePreedit(const QString &string, const QList<MInputMethod::PreeditTextFormat> &preeditFormats,
                               int replacementStart = 0, int replacementLength = 0, int cursorPos = -1);

    //! \brief Sends a non-printable key event. Parameters as in QKeyEvent constructor
    virtual void keyEvent(int type, int key, int modifiers, const QString &text, bool autoRepeat,
                          int count, MInputMethod::EventRequestType requestType
                          = MInputMethod::EventRequestBoth);

    //!
    // \brief Updates the input method window area
    // \param rect Bounding rectangle of the input method area
    virtual void updateInputMethodArea(const QRect &rect);

    /*!
     * \brief set global correction option enable/disable
     */
    virtual void setGlobalCorrectionEnabled(bool);

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

    /*!
     * \brief Set detectable autorepeat for X on/off
     *
     * Detectable autorepeat means that instead of press, release, press, release, press,
     * release... sequence of key events you get press, press, press, release key events
     * when a key is repeated.  The setting is X client specific.  This is intended to be
     * used when key event redirection is enabled with \a setRedirectKeys.
     */
    virtual void setDetectableAutoRepeat(bool enabled);

    /*!
     * \brief Sets selection which start from \start with \a length in the focus widget.
     *
     * \param start the start index
     * \param length the length of selection
     * Note: The cursor will be moved after the commit string has been committed, and the
     * preedit string will be located at the new edit position.
     */
    virtual void setSelection(int start, int length);

    /*!
     * \brief get selecting text
     * \param valid validity for the return value
     */
    QString selection(bool &valid) const;

    /*!
     * \brief Updates current language of active input method in input context.
     * \param language ICU format locale ID string
     * \sa QInputContext::language()
     */
    void setLanguage(const QString &language);

    static bool debug;

Q_SIGNALS:
    //! \internal
    /*! 
     * \brief Emitted when input method area is changed
     * \param region new input method area
     */
    void inputMethodAreaChanged(const QRect &region);
    //! \internal_end

private slots:
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

#ifdef HAVE_MEEGOTOUCH
    //! Notify input method plugin about the application's active window prepare to change to a new \a orientation angle.
    void notifyOrientationAboutToChange(M::OrientationAngle orientation);

    //! Notify input method plugin about new \a orientation angle of application's active window.
    //! \note this method is called when the orientation change is finished
    void notifyOrientationChanged(M::OrientationAngle orientation);
#endif

    //! Notify input method plugin about the application's active window prepare to change to a new \a orientation angle.
    void notifyOrientationAboutToChange(Maliit::OrientationAngle orientation);

    //! Notify input method plugin about new \a orientation angle of application's active window.
    //! \note this method is called when the orientation change is finished
    void notifyOrientationChanged(Maliit::OrientationAngle orientation);

    //! Notify input method plugin about a new attribute extension which is defined in \a fileName and with the unique \a id to be registered.
    //! \note empty string for \a fileName is acceptable, it means don't load any content, just register a new id.
    void notifyAttributeExtensionRegistered(int id, const QString &fileName);

    //! Notify input method plugin about an attribute extension which has a unique identifier \a id to be unregistered.
    void notifyAttributeExtensionUnregistered(int id);

    //! Notify input method plugin about the \attribute of the \a item in the custom toolbar which has the unique \a id is changed to \a value.
    void notifyToolbarItemAttributeChanged(int id, const QString &item, const QString &attribute, const QVariant& value);

    //! Notify input method plugin about the \attribute of the \a keyId in the extended attribute which has the unique \a id is changed to \a value.
    void notifyExtendedAttributeChanged(int id, const QString &target, const QString &targetItem, const QString &attribute, const QVariant& value);

    //! Notify input method plugin about the \attribute of the \a key in the extended attribute which has the unique \a id is changed to \a value.
    void notifyExtendedAttributeChanged(int id, const QString &key, const QVariant& value);

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
                                 const QList<MInputMethod::PreeditTextFormat> &preeditFormats,
                                 int replacementStart = 0, int replacementLength = 0, int cursorPos = -1);

    void connectToDBus();

    void notifyCopyPasteState();

    //! returns content type corresponding to specified hints
    MInputMethod::TextContentType contentType(Qt::InputMethodHints hints) const;

    //! returns the D-Bus object path for this instance
    QString dbusObjectPath() const;

    //! returns state for currently focused widget, key is attribute name.
    QMap<QString, QVariant> getStateInformation() const;

    //! registers existing attribute extensions to input method server and updates their state
    void registerExistingAttributeExtensions();

    //! finds focus scope item for item
    static QGraphicsItem *findFocusScopeItem(QGraphicsItem *item);

    bool isVisible(const QRect &rect, const QGraphicsView *view, const QGraphicsItem *item) const;

    template<typename PreeditInjectionEvent>
    bool handlePreeditInjectionEvent(const PreeditInjectionEvent *event);

    bool active; // is connection active
    InputPanelState inputPanelState; // state for the input method server's software input panel

    /*! Timer for hiding the current Software Input Panel.
     *  This is mainly for switching directly between widgets
     *  that has input method enabled.
     *  The delayed hiding is handled in input-context because
     *  this has to work also with plain qt apps.
     */
    QTimer sipHideTimer;

    DBusIMServerProxy *imServer;

    bool correctionEnabled;

    MPreeditStyleContainer *styleContainer;
    QString preedit;

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
};

#endif
