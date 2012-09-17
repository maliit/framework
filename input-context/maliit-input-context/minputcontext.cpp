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

#include "minputcontext.h"
#include "mimserverconnection.h"
#include "maliit/namespaceinternal.h"

#include <QApplication>
#ifdef Q_WS_X11
#include <QX11Info>
#endif
#include <QInputContext>
#include <QCoreApplication>
#include <QKeyEvent>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QGraphicsWidget>
#include <QTextFormat>
#include <QApplication>
#include <QClipboard>
#include <QDebug>
#include <QByteArray>

#include <maliit/inputmethod.h>
#include <maliit/attributeextension.h>
#include <maliit/attributeextensionregistry.h>
#include <maliit/namespace.h>

#ifdef Q_WS_X11
#include <X11/XKBlib.h>
#endif

using Maliit::AttributeExtension;
using Maliit::InputMethod;

namespace
{
    const int SoftwareInputPanelHideTimer = 100;
    const char * const ToolbarTarget("/toolbar");
    const char * const InputContextName(MALIIT_INPUTCONTEXT_NAME);

    //! Extracts a dynamic QObject property from obj, using normalization rules
    //! for propertyName (eases mapping of QML properties to QObject
    //! properties). Valid property names start with "maliit-"
    QVariant extractProperty(const QObject *obj,
                             const char * const propertyName)
    {
        static const QByteArray prefix("maliit-");
        const QByteArray name(propertyName);

        if (not obj || not name.startsWith(prefix)) {
            qCritical() << __PRETTY_FUNCTION__
                        << "Ignoring invalid object or invalid propertyName "
                        << propertyName;
            return QVariant();
        }

        QVariant result = obj->property(name);

        if (not result.isValid()) {
            QByteArray camelCased;
            bool turnUp = false;

            // Function guard guarantees that properyName starts with "maliit-".
            for (int idx = prefix.length(); idx < name.length(); ++idx) {
                const QChar curr = name.at(idx);
                if (curr == '-') {
                    turnUp = true;
                } else {
                    camelCased.append(turnUp ? curr.toUpper() : curr);
                    turnUp = false;
                }
            }

            result = obj->property(camelCased);
        }

        return result;
    }

#ifdef Q_WS_X11
    enum {
        XKeyPress = KeyPress,
        XKeyRelease = KeyRelease
    };
#endif
}

#undef KeyPress
#undef KeyRelease

int MInputContext::connectionCount = -1;

bool MInputContext::debug = false;

// Note: this class can also be used on plain Qt applications.
// This means that the functionality _can_ _not_ rely on
// MApplication or classes relying on it being initialized.


MInputContext::MInputContext(QSharedPointer<MImServerConnection> newImServer, const QString &name, QObject *parent)
    : QInputContext(parent),
      active(false),
      inputPanelState(InputPanelHidden),
      imServer(newImServer),
      correctionEnabled(false),
      preeditCursorPos(-1),
      connectedObject(0),
      pasteAvailable(false),
      copyAvailable(false),
      copyAllowed(true),
      redirectKeys(false),
      currentKeyEventTime(0),
      m_name(name)
{
    QByteArray debugEnvVar = qgetenv("MALIIT_DEBUG");
    if (!debugEnvVar.isEmpty() && debugEnvVar != "0") {
        debug = true;
    }

#ifdef Q_WS_X11
    int opcode = -1;
    int xkbEventBase = -1;
    int xkbErrorBase = -1;
    int xkblibMajor = XkbMajorVersion;
    int xkblibMinor = XkbMinorVersion;
    if (!XkbLibraryVersion(&xkblibMajor, &xkblibMinor)) {
        qCritical("%s xkb query version error!", __PRETTY_FUNCTION__);
        return;
    }

    Display* display = QX11Info::display();
    if (!XkbQueryExtension(display, &opcode, &xkbEventBase, &xkbErrorBase, &xkblibMajor, &xkblibMinor)) {
        qCritical("%s xkb query extension error!", __PRETTY_FUNCTION__);
        return;
    }
#endif

    sipHideTimer.setSingleShot(true);
    sipHideTimer.setInterval(SoftwareInputPanelHideTimer);
    connect(&sipHideTimer, SIGNAL(timeout()), SLOT(hideInputMethod()));

    connect(QApplication::clipboard(), SIGNAL(dataChanged()),
            this, SLOT(handleClipboardDataChange()));

    connectInputMethodServer();
    connectInputMethodExtension();
}

MInputContext::~MInputContext()
{
}

void MInputContext::connectInputMethodExtension()
{
    connect(InputMethod::instance(),
            SIGNAL(orientationAngleAboutToChange(Maliit::OrientationAngle)),
            this, SLOT(notifyOrientationAboutToChange(Maliit::OrientationAngle)));
    connect(InputMethod::instance(),
            SIGNAL(orientationAngleChanged(Maliit::OrientationAngle)),
            this, SLOT(notifyOrientationChanged(Maliit::OrientationAngle)));
}

void MInputContext::connectInputMethodServer()
{
    connect(imServer.data(), SIGNAL(connected()), this, SLOT(onDBusConnection()));
    connect(imServer.data(), SIGNAL(disconnected()), this, SLOT(onDBusDisconnection()));

    /* Hook up incoming communication from input method server */
    connect(imServer.data(), SIGNAL(activationLostEvent()), this, SLOT(activationLostEvent()));

    connect(imServer.data(), SIGNAL(imInitiatedHide()), this, SLOT(imInitiatedHide()));

    connect(imServer.data(),
            SIGNAL(commitString(QString,int,int,int)),
            this, SLOT(commitString(QString,int,int,int)));

    connect(imServer.data(),
            SIGNAL(updatePreedit(QString,QList<Maliit::PreeditTextFormat>,int,int,int)),
            this, SLOT(updatePreedit(QString,QList<Maliit::PreeditTextFormat>,int,int,int)));

    connect(imServer.data(),
            SIGNAL(keyEvent(int,int,int,QString,bool,int,Maliit::EventRequestType)),
            this, SLOT(keyEvent(int,int,int,QString,bool,int,Maliit::EventRequestType)));

    connect(imServer.data(),
            SIGNAL(updateInputMethodArea(QRect)),
            this, SLOT(updateInputMethodArea(QRect)));

    connect(imServer.data(),
            SIGNAL(setGlobalCorrectionEnabled(bool)),
            this, SLOT(setGlobalCorrectionEnabled(bool)));

    connect(imServer.data(),
            SIGNAL(getPreeditRectangle(QRect&,bool&)),
            this, SLOT(getPreeditRectangle(QRect&,bool&)));

    connect(imServer.data(), SIGNAL(invokeAction(QString,QKeySequence)), this, SLOT(onInvokeAction(QString,QKeySequence)));

    connect(imServer.data(), SIGNAL(setRedirectKeys(bool)), this, SLOT(setRedirectKeys(bool)));

    connect(imServer.data(),
            SIGNAL(setDetectableAutoRepeat(bool)),
            this, SLOT(setDetectableAutoRepeat(bool)));

    connect(imServer.data(),
            SIGNAL(setSelection(int,int)),
            this, SLOT(setSelection(int,int)));

    connect(imServer.data(),
            SIGNAL(getSelection(QString&,bool&)),
            this, SLOT(getSelection(QString&, bool&)));

    connect(imServer.data(),
            SIGNAL(setLanguage(QString)),
            this, SLOT(setLanguage(QString)));
}

bool MInputContext::event(QEvent *event)
{
    if (event->type() == Maliit::PreeditInjectionEvent::eventNumber()) {
        /* reinterpret_cast is used instead of dynamic_cast because the event might be a
        MPreeditInjection event. It is layout compatible with Maliit::PreeditInjectionevent
        Note that if an event has the same QEvent:Type, but is not binary compatible, this will crash */
        Maliit::PreeditInjectionEvent *injectionEvent = reinterpret_cast<Maliit::PreeditInjectionEvent *>(event);
        if (injectionEvent == 0) {
            return false;
        }

        if (handlePreeditInjectionEvent(injectionEvent)) {
            event->accept();
            return true;
        } else {
            return false;
        }
    }

    return QInputContext::event(event);
}

bool MInputContext::handlePreeditInjectionEvent(const Maliit::PreeditInjectionEvent *event)
{
    if (correctionEnabled) {
        if (debug) {
            qDebug() << InputContextName << __PRETTY_FUNCTION__
                     << "MInputContext got preedit injection:"
                     << event->preedit()
                     << ", event cursor pos:" << event->eventCursorPosition();
        }
        // send the injected preedit to input method server and back to the widget with proper
        // styling
        // Note: plugin could change the preedit style in imServer->setPreedit().
        // The cursor is hidden for preedit by default. The input method server can decide
        // whether it needs the cursor.
        QList<Maliit::PreeditTextFormat> preeditFormats;
        Maliit::PreeditTextFormat preeditFormat(0, event->preedit().length(),
                                                      Maliit::PreeditDefault);
        preeditFormats << preeditFormat;
        // TODO: updatePreeditInternally() below causes update() to be called which
        // communicates new cursor position (among other things) to the active
        // input method.  The cursor is at the beginning of the pre-edit but the
        // input method does not yet know that there is an active pre-edit,
        // because that is communicated separately by the imServer->setPreedit()
        // call below.  This means, for example, that if the cursor position is
        // such that autocapitalization applies, it will have an effect, and once
        // the setPreedit() below has been called, the effect will be undone.
        // This causes flickering in vkb/sbox but not on the device, so for now
        // we'll leave this to be fixed later.  Please refer to NB#226907.
        updatePreeditInternally(event->preedit(), preeditFormats,
                                event->replacementStart(),
                                event->replacementLength());
        imServer->setPreedit(event->preedit(), event->eventCursorPosition());

        return true;
    } else {
        if (debug) {
            qDebug() << InputContextName << __PRETTY_FUNCTION__
                     << "MInputContext ignored preedit injection because correction is disabled";
        }
        return false;
    }
}

QString MInputContext::identifierName()
{
    return m_name;
}


bool MInputContext::isComposing() const
{
    return !preedit.isEmpty();
}


QString MInputContext::language()
{
    return m_language;
}

void MInputContext::setLanguage(const QString &language)
{
    if (m_language != language) {
        m_language = language;
        InputMethod::instance()->setLanguage(language);
    }
}

void MInputContext::reset()
{
    if (debug) qDebug() << InputContextName << "in" << __PRETTY_FUNCTION__;

    // send existing preedit to widget, documentation unclear whether this is
    // allowed, but trolls gave permission to use it. Most of qt's internal
    // input methods do the same thing.
    const bool hadPreedit = !preedit.isEmpty();
    if (hadPreedit) {
        QList<QInputMethodEvent::Attribute> attributes;
        if (preeditCursorPos >= 0) {
            bool valid = false;
            int start = cursorStartPosition(&valid);
            if (valid) {
                attributes << QInputMethodEvent::Attribute(QInputMethodEvent::Selection,
                                                           start + preeditCursorPos, 0, QVariant());
            }
        }

        QInputMethodEvent event("", attributes);
        event.setCommitString(preedit);
        sendEvent(event);
        preedit.clear();
        preeditCursorPos = -1;
    }

    // reset input method server, preedit requires synchronization.
    // rationale: input method might be autocommitting existing preedit without
    // user interaction.
    imServer->reset(hadPreedit);
}


void MInputContext::update()
{
    if (debug) qDebug() << InputContextName << "in" << __PRETTY_FUNCTION__;

    const QWidget *const focused = focusWidget();

    if (focused == 0) {
        return;
    }

    // A QGraphicsView should only be focused when there is a focused item inside it.
    // But there are cases on unfocusing an item that the item is already unfocused
    // while the QGraphicsView is still focused when update() is called.
    // This case should be handled the same way as if the QGraphicsView would be already
    // unfocused (so we just return here and the widget information is updated on unfocussing
    // of the QGraphicsView in setFocusWidget()).
    const QGraphicsView * const graphicsView = qobject_cast<const QGraphicsView *>(focused);
    if (graphicsView && graphicsView->scene()) {
        if (graphicsView->scene()->focusItem() == 0)
            return;
    }

    // get the state information of currently focused widget, and pass it to input method server
    QMap<QString, QVariant> stateInformation = getStateInformation();

    imServer->updateWidgetInformation(stateInformation, false);
}


void MInputContext::mouseHandler(int x, QMouseEvent *event)
{
    if (debug) {
        qDebug() << InputContextName << "in" << __PRETTY_FUNCTION__;
        qDebug() << InputContextName << " event pos: " << event->globalPos() << " cursor pos:" << x;
    }

    if ((event->type() == QEvent::MouseButtonPress
         || event->type() == QEvent::MouseButtonRelease)
        && (x < 0 || x >= preedit.length())) {
        reset();
        return;
    }

    // input method server needs to be informed about clicks
    if (event->type() == QEvent::MouseButtonRelease) {

        // Query preedit rectangle and pass it to im server if found.
        // In case of plain QT application, null rectangle will be passed.
        QRect preeditRect;

        QWidget *focused = focusWidget();

        if (focused) {
            Qt::InputMethodQuery query
                = static_cast<Qt::InputMethodQuery>(Maliit::PreeditRectangleQuery);
            preeditRect = focused->inputMethodQuery(query).toRect();
        }

        // To preserve the wire protocol, the "x" argument gets
        // transferred in widget state instead of being an extra
        // argument to mouseClickedOnPreedit().
        QMap<QString, QVariant> stateInformation = getStateInformation();
        stateInformation["preeditClickPos"] = x;
        imServer->updateWidgetInformation(stateInformation, false);

        imServer->mouseClickedOnPreedit(event->globalPos(), preeditRect);
    }
}


void MInputContext::setFocusWidget(QWidget *focused)
{
    if (debug) qDebug() << InputContextName << "in" << __PRETTY_FUNCTION__ << focused;

    QObject *focusedObject = focused;
    QGraphicsItem *focusItem = 0;
    QInputContext::setFocusWidget(focused);

    // get detailed focus information from inside qgraphicsview
    QGraphicsView *graphicsView = qobject_cast<QGraphicsView *>(focusWidget());

    if (graphicsView && graphicsView->scene()) {
        focusItem = graphicsView->scene()->focusItem();

        if (focusItem) {
            focusedObject = dynamic_cast<QObject *>(focusItem);
        }
    }

    const QMap<QString, QVariant> stateInformation = getStateInformation();
    if (focused) {
        // for non-null focus widgets, we'll have this context activated
        if (!active) {
            imServer->activateContext();
            active = true;

            Maliit::OrientationAngle angle = InputMethod::instance()->orientationAngle();
            notifyOrientationChanged(angle);
        }

        imServer->updateWidgetInformation(stateInformation, true);

        // check if copyable text is selected
        Qt::InputMethodQuery query = Qt::ImCurrentSelection;
        QVariant queryResult = focused->inputMethodQuery(query);
        if (queryResult.isValid()) {
            copyAvailable = !queryResult.toString().isEmpty();
        }

        if (focusItem) {
            copyAllowed = !(focusItem->inputMethodHints() & Qt::ImhHiddenText);
        } else {
            copyAllowed = !(focused->inputMethodHints() & Qt::ImhHiddenText);
        }

        pasteAvailable = !QApplication::clipboard()->text().isEmpty();

    } else {
        copyAvailable = false;
        copyAllowed = false;
        imServer->updateWidgetInformation(stateInformation, true);

    }

    // show or hide Copy/Paste button on input method server
    notifyCopyPasteState();

    if (inputPanelState == InputPanelShowPending && focused) {
        sipHideTimer.stop();
        imServer->showInputMethod();
        inputPanelState = InputPanelShown;
    }

    if (connectedObject) {
        connectedObject->disconnect(this);
        connectedObject = 0;
    }

    if (focusedObject && focusedObject->metaObject()) {
        if (focusedObject->metaObject()->indexOfSignal("copyAvailable(bool)") != -1) {
            // for MTextEdit
            connect(focusedObject, SIGNAL(copyAvailable(bool)),
                    this, SLOT(handleCopyAvailabilityChange(bool)));
            connectedObject = focusedObject;
        } else if (focusedObject->metaObject()->indexOfSignal("selectedTextChanged()") != -1) {
            // for QML::TextInput
            connect(focusedObject, SIGNAL(selectedTextChanged()),
                    this, SLOT(handleSelectedTextChange()));
            connectedObject = focusedObject;
        } else if (focusedObject->metaObject()->indexOfSignal("selectionChanged()") != -1) {
            // for QLineEdit
            connect(focusedObject, SIGNAL(selectionChanged()),
                    this, SLOT(handleSelectedTextChange()));
            connectedObject = focusedObject;
        }
    }
}

#ifdef Q_WS_X11
bool MInputContext::x11FilterEvent(QWidget */*widget*/, XEvent *event)
{
    if ((event->type == XKeyPress || (event->type == XKeyRelease))) {
        currentKeyEventTime = event->xkey.time;
    }

    return false;               // let filterEvent() to really handle the event
}
#endif

bool MInputContext::filterEvent(const QEvent *event)
{
    bool eaten = false;

    switch (event->type()) {
    case QEvent::RequestSoftwareInputPanel:
        if (debug) qDebug() << "MInputContext got RequestSoftwareInputPanel event";

        if (focusWidget() != 0) {
            sipHideTimer.stop();
        }

        if (!active || focusWidget() == 0) {
            // in case SIP request comes without a properly focused widget, we
            // don't ask input method server to be shown. It's done when the next widget
            // is focused, so the widget state information can be set.
            inputPanelState = InputPanelShowPending;

        } else {
            // note: could do this also if panel was hidden
            imServer->showInputMethod();
            inputPanelState = InputPanelShown;
        }

        eaten = true;
        break;

    case QEvent::CloseSoftwareInputPanel:
        if (debug) qDebug() << "MInputContext got CloseSoftwareInputPanel event";

        sipHideTimer.start();
        eaten = true;
        break;

    case QEvent::KeyPress:
    case QEvent::KeyRelease:
        if (focusWidget() == 0) {
            break;  // Don't Q_EMIT signals without focused widget
        }

        if (event->type() == QEvent::KeyPress) {
            InputMethod::instance()->emitKeyPress(*static_cast<const QKeyEvent*>(event));
        } else {
            InputMethod::instance()->emitKeyRelease(*static_cast<const QKeyEvent*>(event));
        }

        if (redirectKeys) {
            const QKeyEvent *key = static_cast<const QKeyEvent *>(event);
            imServer->processKeyEvent(key->type(), static_cast<Qt::Key>(key->key()),
                                      key->modifiers(), key->text(), key->isAutoRepeat(),
                                      key->count(), key->nativeScanCode(),
                                      key->nativeModifiers(), currentKeyEventTime);
            eaten = true;
        }
        break;

    default:
        if (event->type() == Maliit::PreeditInjectionEvent::eventNumber()) {
            const Maliit::PreeditInjectionEvent *injectionEvent = dynamic_cast<const Maliit::PreeditInjectionEvent *>(event);
            if (injectionEvent == 0) {
                return false;
            }

            eaten = handlePreeditInjectionEvent(injectionEvent);
        }
    }

    return eaten;
}

void MInputContext::hideInputMethod()
{
    imServer->hideInputMethod();

    inputPanelState = InputPanelHidden;
}

void MInputContext::activationLostEvent()
{
    // This method is called when activation was gracefully lost.
    // There is similar cleaning up done in onDBusDisconnection.
    active = false;
    inputPanelState = InputPanelHidden;
}


void MInputContext::imInitiatedHide()
{
    if (debug) qDebug() << InputContextName << "in" << __PRETTY_FUNCTION__;

    inputPanelState = InputPanelHidden;

    // need to remove focus from the current text entry
    if (focusWidget()) {
        QGraphicsView *graphicsView = qobject_cast<QGraphicsView *>(focusWidget());

        if (graphicsView && graphicsView->scene()) {
            // inside qgraphics we remove the focus from item focused there
            QGraphicsScene *scene = graphicsView->scene();
            QGraphicsItem *item = scene->focusItem();

            if (item) {
                QGraphicsItem *focusScopeItem = findFocusScopeItem(item);

                if (focusScopeItem) {
                    // With focus scope, found from QML, Qt gives focus to parent,
                    // would break focus handling so working around by stealing
                    // focus with a dummy item. Also Qt has currently some
                    // problems with FocusScope. Need to check later if we could
                    // just remove focus from focus scope item.
                    // see http://bugreports.qt.nokia.com/browse/QTBUG-19688
                    QGraphicsWidget dummyItem;
                    scene->addItem(&dummyItem);
                    dummyItem.setFlag(QGraphicsItem::ItemIsFocusable);
                    dummyItem.setFocus();
                } else {
                    item->clearFocus();
                }
            }

        } else {
            focusWidget()->clearFocus();
        }
    }
}

QGraphicsItem *MInputContext::findFocusScopeItem(QGraphicsItem *item)
{
    if (item == 0) {
        return 0;
    }

    // Note: ItemIsFocusScope is Qt internal flag used in QML
    QGraphicsItem *focusScopeItem = 0;
    QGraphicsItem *parentItem = item->parentItem();
    while (parentItem) {
        if (parentItem->flags() & QGraphicsItem::ItemIsFocusScope) {
            focusScopeItem = parentItem;
            break;
        }

        parentItem = parentItem->parentItem();
    }
    return focusScopeItem;
}

void MInputContext::commitString(const QString &string, int replacementStart,
                                 int replacementLength, int cursorPos)
{
    if (debug) qDebug() << InputContextName << "in" << __PRETTY_FUNCTION__;

    if (imServer->pendingResets()) {
        return;
    }

    preedit.clear();
    preeditCursorPos = -1;

    int start = -1;
    if (cursorPos >= 0) {
        bool valid = false;
        int currentStart = cursorStartPosition(&valid);
        if (valid) {
            start = cursorPos + currentStart + replacementStart;
        }
    }

    if (start >= 0) {
        QList<QInputMethodEvent::Attribute> attributes;
        attributes << QInputMethodEvent::Attribute(QInputMethodEvent::Selection, start, 0, QVariant());
        QInputMethodEvent event("", attributes);
        event.setCommitString(string, replacementStart, replacementLength);
        sendEvent(event);
    } else {
        QInputMethodEvent event;
        event.setCommitString(string, replacementStart, replacementLength);
        sendEvent(event);
    }
}


void MInputContext::updatePreedit(const QString &string,
                                  const QList<Maliit::PreeditTextFormat> &preeditFormats,
                                  int replacementStart, int replacementLength, int cursorPos)
{
    if (debug) {
        qDebug() << InputContextName << "in" << __PRETTY_FUNCTION__ << "preedit:" << string
                 << ", replacementStart:" << replacementStart
                 << ", replacementLength:" << replacementLength
                 << ", cursorPos:" << cursorPos;
    }

    if (imServer->pendingResets()) {
        return;
    }

    updatePreeditInternally(string, preeditFormats, replacementStart, replacementLength, cursorPos);
}

void MInputContext::updatePreeditInternally(const QString &string,
                                            const QList<Maliit::PreeditTextFormat> &preeditFormats,
                                            int replacementStart, int replacementLength, int cursorPos)
{
    preedit = string;
    preeditCursorPos = cursorPos;

    QList<QInputMethodEvent::Attribute> attributes;
    Q_FOREACH (const Maliit::PreeditTextFormat &preeditFormat, preeditFormats) {

        // set proper formatting
        QTextCharFormat format;
        format.merge(standardFormat(QInputContext::PreeditFormat));

        // update style mode
        switch (preeditFormat.preeditFace) {
        case Maliit::PreeditNoCandidates:
            format.setUnderlineStyle(QTextCharFormat::SpellCheckUnderline);
            format.setUnderlineColor(Qt::red);
            break;
        case Maliit::PreeditUnconvertible:
            format.setForeground(QBrush(QColor(128, 128, 128)));
            break;
        case Maliit::PreeditActive:
            format.setForeground(QBrush(QColor(153, 50, 204)));
            format.setFontWeight(QFont::Bold);
            break;
        case Maliit::PreeditKeyPress:
        case Maliit::PreeditDefault:
            format.setUnderlineStyle(QTextCharFormat::SingleUnderline);
            format.setUnderlineColor(QColor(0, 0, 0));
            break;
        }

        attributes << QInputMethodEvent::Attribute(QInputMethodEvent::TextFormat,
                                                   preeditFormat.start,
                                                   preeditFormat.length, format);
    }

    if (cursorPos >= 0) {
        attributes << QInputMethodEvent::Attribute(QInputMethodEvent::Cursor, cursorPos, 1, QVariant());
    }

    QInputMethodEvent event(string, attributes);
    if (replacementStart || replacementLength) {
        event.setCommitString("", replacementStart, replacementLength);
    }

    sendEvent(event);
}

void MInputContext::keyEvent(int type, int key, int modifiers, const QString &text,
                             bool autoRepeat, int count,
                             Maliit::EventRequestType requestType)
{
    if (debug) qDebug() << InputContextName << "in" << __PRETTY_FUNCTION__;

    // Construct an event instance out of the parameters.
    QEvent::Type eventType = static_cast<QEvent::Type>(type);
    QKeyEvent event(eventType, key,
                    static_cast<Qt::KeyboardModifiers>(modifiers),
                    text, autoRepeat, count);

    if (requestType != Maliit::EventRequestEventOnly) {
        if (eventType == QEvent::KeyPress) {
            InputMethod::instance()->emitKeyPress(event);
        } else if (eventType == QEvent::KeyRelease) {
            InputMethod::instance()->emitKeyRelease(event);
        }
    }

    if (focusWidget() != 0 && requestType != Maliit::EventRequestSignalOnly) {
        QCoreApplication::sendEvent(focusWidget(), &event);
    }
}


void MInputContext::updateInputMethodArea(const QRect &rect)
{
    InputMethod::instance()->setArea(rect);

    Q_EMIT inputMethodAreaChanged(rect);
}


void MInputContext::setGlobalCorrectionEnabled(bool enabled)
{
    correctionEnabled = enabled;
}


void MInputContext::getPreeditRectangle(QRect &rectangle, bool &valid) const
{
    QRect rect;
    valid = false;

    Qt::InputMethodQuery query = static_cast<Qt::InputMethodQuery>(Maliit::PreeditRectangleQuery);

    if (focusWidget()) {
        QVariant queryResult = focusWidget()->inputMethodQuery(query);

        valid = queryResult.isValid();
        rect = queryResult.toRect();
    }

    rectangle = rect; /* Set output */
}

void MInputContext::handleClipboardDataChange()
{
    bool newPasteAvailable = !QApplication::clipboard()->text().isEmpty();

    if (newPasteAvailable != pasteAvailable) {
        pasteAvailable = newPasteAvailable;
        notifyCopyPasteState();
    }
}

void MInputContext::onInvokeAction(const QString &action, const QKeySequence &sequence)
{
    static const Qt::KeyboardModifiers AllModifiers = Qt::ShiftModifier | Qt::ControlModifier | Qt::AltModifier | Qt::MetaModifier | Qt::KeypadModifier;

    bool method_invoked = false;

    if (connectedObject) {
        method_invoked = QMetaObject::invokeMethod(connectedObject, action.toUtf8().constData(), Qt::DirectConnection);
    }

    if (debug) qDebug() << InputContextName << __PRETTY_FUNCTION__ << "action" << action << "invoked:" << method_invoked;

    if (not method_invoked) {
        for (unsigned int i = 0; i < sequence.count(); i++) {
            const int key = sequence[i] & ~AllModifiers;
            const int modifiers = sequence[i] & AllModifiers;
            QString text("");
            if (modifiers == Qt::NoModifier || modifiers == Qt::ShiftModifier) {
                text = QString(key);
            }
            keyEvent(QEvent::KeyPress, key, modifiers, text, false, 1);
            keyEvent(QEvent::KeyRelease, key, modifiers, text, false, 1);
        }
    }
}

void MInputContext::onDBusDisconnection()
{
    if (debug) qDebug() << __PRETTY_FUNCTION__;

    active = false;
    redirectKeys = false;

    InputMethod::instance()->setArea(QRect());
}

void MInputContext::onDBusConnection()
{
    if (debug) qDebug() << __PRETTY_FUNCTION__;

    // There could already be focused item when the connection to the uiserver is
    // established. Show keyboard immediately in that case.
    QWidget *widget = qApp->focusWidget();
    // TODO: Should not call testAttribute explicitly, QInputContext::setFocusWidget()
    // already does it. The right way should be calling QInputContext::setFocusWidget()
    // and then checking whether QInputContext has focusWidget(). But the QT bug NB#181094
    // prevents us.
    if (widget && widget->testAttribute(Qt::WA_InputMethodEnabled)) {
        // Force activation, since setFocusWidget may have been called after
        // onDBusDisconnection set active to false.
        active = false;
        setFocusWidget(widget);
        if (inputPanelState != InputPanelHidden) {
            imServer->showInputMethod();
            inputPanelState = InputPanelShown;
        }
    }
}

void MInputContext::handleSelectedTextChange()
{
    if (connectedObject) {
        bool hasSelectedText = (connectedObject->property("selectionStart").toInt()
                                != connectedObject->property("selectionEnd").toInt());
        handleCopyAvailabilityChange(hasSelectedText);
    }
}

void MInputContext::handleSelectionChanged()
{
    if (connectedObject) {
        bool hasSelectedText = connectedObject->property( "hasSelectedText" ).toBool();
        handleCopyAvailabilityChange(hasSelectedText);
    }
}

void MInputContext::handleCopyAvailabilityChange(bool copyAvailable)
{
    if (this->copyAvailable != copyAvailable) {
        this->copyAvailable = copyAvailable;
        notifyCopyPasteState();
    }
}

void MInputContext::notifyCopyPasteState()
{
    imServer->setCopyPasteState(copyAvailable && copyAllowed, pasteAvailable);
}

void MInputContext::notifyOrientationAboutToChange(Maliit::OrientationAngle angle)
{
    // can get called from signal so cannot be sure we are really currently active
    if (active) {
        imServer->appOrientationAboutToChange(static_cast<int>(angle));
    }
}

void MInputContext::notifyOrientationChanged(Maliit::OrientationAngle angle)
{
    // can get called from signal so cannot be sure we are really currently active
    if (active) {
        imServer->appOrientationChanged(static_cast<int>(angle));
    }
}

Maliit::TextContentType MInputContext::contentType(Qt::InputMethodHints hints) const
{
    Maliit::TextContentType type = Maliit::FreeTextContentType;
    hints &= Qt::ImhExclusiveInputMask;

    if (hints == Qt::ImhFormattedNumbersOnly || hints == Qt::ImhDigitsOnly) {
        type = Maliit::NumberContentType;
    } else if (hints == Qt::ImhDialableCharactersOnly) {
        type = Maliit::PhoneNumberContentType;
    } else if (hints == Qt::ImhEmailCharactersOnly) {
        type = Maliit::EmailContentType;
    } else if (hints == Qt::ImhUrlCharactersOnly) {
        type = Maliit::UrlContentType;
    }

    return type;
}

void MInputContext::setRedirectKeys(bool enabled)
{
    redirectKeys = enabled;
}

void MInputContext::setDetectableAutoRepeat(bool enabled)
{
#ifdef Q_WS_X11
    Bool detectableAutoRepeatSupported(False);
    XkbSetDetectableAutoRepeat(QX11Info::display(), enabled ? True : False,
                               &detectableAutoRepeatSupported);
    if (detectableAutoRepeatSupported == False) {
        qWarning() << "Detectable autorepeat not supported.";
    }
#else
    Q_UNUSED(enabled)
#endif
}

QMap<QString, QVariant> MInputContext::getStateInformation() const
{
    QMap<QString, QVariant> stateInformation;
    const QWidget *focused = focusWidget();

    stateInformation["focusState"] = focused != NULL;

    if (!focused) {
        return stateInformation;
    }

    const QGraphicsView * const graphicsView = qobject_cast<const QGraphicsView *>(focused);
    const QGraphicsObject * const focusedObject =
            (graphicsView && graphicsView->scene())
            ? qgraphicsitem_cast<const QGraphicsObject *>(graphicsView->scene()->focusItem())
            : 0;

    QVariant queryResult;

    // Extract suppressInputMethod (formerly known as VisualizationPriorityQuery):
    queryResult = extractProperty(focused, Maliit::InputMethodQuery::suppressInputMethod);

    if (!queryResult.isValid()) {
        if (focusedObject) {
            queryResult = extractProperty(focusedObject, Maliit::InputMethodQuery::suppressInputMethod);
        }

        if (!queryResult.isValid()) {

            queryResult = focused->inputMethodQuery(
                        static_cast<Qt::InputMethodQuery>(Maliit::VisualizationPriorityQuery));
        }
    }

    if (queryResult.isValid()) {
        stateInformation["visualizationPriority"] = queryResult.toBool();
    }

    queryResult = extractProperty(focused, Maliit::InputMethodQuery::attributeExtensionId);

    if (!queryResult.isValid()) {
        if (focusedObject) {
            // FIXME: Kill off once users of this property were able to switch
            // to Maliit::InputMethodQuery::attributeExtensionId.
            queryResult = focusedObject->property("meego-inputmethod-attribute-extension-id");

            if (!queryResult.isValid()) {
                queryResult = extractProperty(focusedObject, Maliit::InputMethodQuery::attributeExtensionId);
            }
        }

        if (!queryResult.isValid()) {
            queryResult = focused->inputMethodQuery(
                static_cast<Qt::InputMethodQuery>(Maliit::InputMethodAttributeExtensionIdQuery));
        }
    }

    if (queryResult.isValid()) {
        stateInformation["toolbarId"] = queryResult.toInt();
    }

    // toolbar file
    queryResult = extractProperty(focused, Maliit::InputMethodQuery::attributeExtension);

    if (!queryResult.isValid())
    {
      queryResult = focused->inputMethodQuery(
          static_cast<Qt::InputMethodQuery>(Maliit::InputMethodAttributeExtensionQuery));
    }

    if (queryResult.isValid()) {
        stateInformation["toolbar"] = queryResult.toString();
    }

    // surrounding text
    queryResult = focused->inputMethodQuery(Qt::ImSurroundingText);

    if (queryResult.isValid()) {
        stateInformation["surroundingText"] = queryResult.toString();
    }

    // cursor pos
    queryResult = focused->inputMethodQuery(Qt::ImCursorPosition);

    if (queryResult.isValid()) {
        stateInformation["cursorPosition"] = queryResult.toInt();
    }

    // anchor pos
    queryResult = focused->inputMethodQuery(Qt::ImAnchorPosition);

    if (queryResult.isValid()) {
        stateInformation["anchorPosition"] = queryResult.toInt();
    }

    Qt::InputMethodHints hints = focused->inputMethodHints();

    if (focusedObject) {
        // focused->inputMethodHints() always returns 0
        // therefore we explicitly call inputMethodHints with focused item
        hints = focusedObject->inputMethodHints();
    }

    // content type value
    // Deprecated, replaced by just transmitting all hints (see below):
    // FIXME: Remove once MAbstractInputMethod API for this got deprecated/removed.
    stateInformation["contentType"] = contentType(hints);

    queryResult = focused->property(Maliit::InputMethodQuery::correctionEnabledQuery);

    if (!queryResult.isValid())
    {
      queryResult = focused->inputMethodQuery(
          static_cast<Qt::InputMethodQuery>(Maliit::ImCorrectionEnabledQuery));
    }

    if (queryResult.isValid()) {
        stateInformation["correctionEnabled"] = queryResult.toBool();
    }

    // Deprecated, replaced by just transmitting all hints (see below):
    // FIXME: Remove once MAbstractInputMethod API for this got deprecated/removed.
    stateInformation["predictionEnabled"] = !(hints & Qt::ImhNoPredictiveText);
    stateInformation["autocapitalizationEnabled"] = !(hints & Qt::ImhNoAutoUppercase);
    stateInformation["hiddenText"] = static_cast<bool>(hints & Qt::ImhHiddenText);

    stateInformation[Maliit::Internal::inputMethodHints] = QVariant(static_cast<qint64>(hints));

    queryResult = focused->inputMethodQuery(
        static_cast<Qt::InputMethodQuery>(Maliit::ImModeQuery));

    Maliit::InputMethodMode inputMethodMode = Maliit::InputMethodModeNormal;
    if (queryResult.isValid()) {
        inputMethodMode = static_cast<Maliit::InputMethodMode>(queryResult.toInt());
    }
    stateInformation["inputMethodMode"] = inputMethodMode;

    // is text selected
    queryResult = focused->inputMethodQuery(Qt::ImCurrentSelection);

    if (queryResult.isValid())
        stateInformation["hasSelection"] = !(queryResult.toString().isEmpty());

#ifndef Q_WS_WIN
    // window id for transient VKB - it is enough to just query the focused widget here:
    stateInformation["winId"] = static_cast<qulonglong>(focused->window()->effectiveWinId());
#endif

    queryResult = focused->inputMethodQuery(Qt::ImMicroFocus);
    if (queryResult.isValid()) {
        // The replied rectangle is covering the area of the input cursor in qwidget coordinates.
        // Need to map the rectangle to global coordinate.
        QRect cursorRect = queryResult.toRect();
        cursorRect = QRect(focused->mapToGlobal(cursorRect.topLeft()),
                           focused->mapToGlobal(cursorRect.bottomRight()));
        if (isVisible(cursorRect, graphicsView, focusedObject)) {
            stateInformation["cursorRectangle"] = cursorRect;
        }
    }

    // Lookup order for wester numeric input override:
    // 1. focusWidet()->property(.),
    // 2. focusedQGraphicsItem->property(.), after casting to QObject.
    // 3. focusWidget()->inputMethodQuery(.),
    // Ensures that plain Qt and Qt Comoponents can readily use this override,
    // as neither of them implement a Maliit-specific IM query.
    queryResult = extractProperty(focused, Maliit::InputMethodQuery::westernNumericInputEnforced);

    if (!queryResult.isValid())
    {
       if (focusedObject) {
            queryResult = extractProperty(focusedObject,
                                          Maliit::InputMethodQuery::westernNumericInputEnforced);
        }

        if (!queryResult.isValid()) {
            queryResult = focused->inputMethodQuery(
                static_cast<Qt::InputMethodQuery>(Maliit::WesternNumericInputEnforcedQuery));
        }
    }

    stateInformation[Maliit::InputMethodQuery::westernNumericInputEnforced] = queryResult.toBool();

    queryResult = extractProperty(focused, Maliit::InputMethodQuery::translucentInputMethod);

    if (!queryResult.isValid())
    {
       if (focusedObject) {
            queryResult = extractProperty(focusedObject, Maliit::InputMethodQuery::translucentInputMethod);
        }
    }

    stateInformation[Maliit::InputMethodQuery::translucentInputMethod] = queryResult.toBool();

    return stateInformation;
}

// 1. There is a probability, that this will detect cursor visible when it actually is not,
// we consider this as a lesser evil than the opposite.
// 2. Overlapping by input objects like virtual keyboard is not considered here,
// IM software should check it themselves if needed.
bool MInputContext::isVisible(const QRect &cursorRect, const QGraphicsView *view, const QGraphicsItem *item) const
{
    if (view == 0 || item == 0) {
        return false;
    }
    QRect visibleRect(0, 0, view->width(), view->height());

    QGraphicsItem *parent = item->parentItem();
    while (parent) {
        QRect widgetClipRect;

        if (parent->isWidget()) {
            // MApplicationPage does not set clipping for its main viewport
            const QGraphicsWidget *parentWidget = static_cast<QGraphicsWidget*>(parent);

            if (parentWidget->inherits("MApplicationPage")) {
                widgetClipRect = parent->sceneBoundingRect().toRect();
            }
        }

        if (!widgetClipRect.isValid()
            && (parent->flags() & QGraphicsItem::ItemClipsChildrenToShape) != 0) {
            // this heavily relies on wehther the view properly set ItemClipsChildrenToShape
            // sometimes they are not
            // (the MApplicationPage above for example)
            widgetClipRect = parent->sceneBoundingRect().toRect();
        }

        if (!widgetClipRect.isNull()) {
            if (!visibleRect.isValid()) {
                visibleRect = widgetClipRect;
            } else {
                visibleRect &= widgetClipRect;
            }
        }

        parent = parent->parentItem();
    }
    // here would be transition of visibleRect to global coordinates, but they appear to be always the same as scene's

    return visibleRect.isValid() && cursorRect.intersects(visibleRect);
}

void MInputContext::setSelection(int start, int length)
{
    QList<QInputMethodEvent::Attribute> attributes;
    attributes << QInputMethodEvent::Attribute(QInputMethodEvent::Selection, start,
                                                length, QVariant());
    QInputMethodEvent event("", attributes);
    sendEvent(event);
}

void MInputContext::getSelection(QString &selection, bool &valid) const
{
    QString selectionText;
    valid = false;
    if (focusWidget()) {
        QVariant queryResult = focusWidget()->inputMethodQuery(Qt::ImCurrentSelection);
        valid = queryResult.isValid();
        selectionText = queryResult.toString();
    }
    selection = selectionText; /* Set output */
}

int MInputContext::cursorStartPosition(bool *valid)
{
    int start = -1;
    if (valid) {
        *valid = false;
    }

    const QWidget *focused = focusWidget();
    if (focused) {
        // obtain the cursor absolute position
        QVariant queryResult = focused->inputMethodQuery(Qt::ImCursorPosition);
        if (queryResult.isValid()) {
            int absCursorPos = queryResult.toInt();

            // Fetch anchor position too but don't require it.
            queryResult = focused->inputMethodQuery(Qt::ImAnchorPosition);
            int absAnchorPos = queryResult.isValid()
                               ? queryResult.toInt() : absCursorPos;

            // In case of selection, base cursorPos on start of it.
            start = qMin<int>(absCursorPos, absAnchorPos);
            *valid = true;
        }
    }

    return start;
}

