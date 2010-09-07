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

#include "minputcontext.h"

#include <QX11Info>
#include <QInputContext>
#include <QCoreApplication>
#include <QKeyEvent>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QTextFormat>
#include <QApplication>
#include <QClipboard>

#include <MPreeditInjectionEvent>
#include <MTheme>
#include <MComponentData>
#include <MDebug>
#include <MLibrary>
#include <MInputMethodState>

#ifdef QT_DBUS
#include <QDBusConnection>
#include "minputcontextadaptor.h"
#include "qtdbusimserverproxy.h"
#else
#include "mdbusglibinputcontextadaptor.h"
#include "glibdbusimserverproxy.h"
#endif
#include "mpreeditstyle.h"
#include "mtimestamp.h"

#include <X11/XKBlib.h>
#undef KeyPress
#undef KeyRelease

M_LIBRARY

namespace
{
    const int SoftwareInputPanelHideTimer = 500;
    const QString DBusCallbackPath("/com/meego/inputmethod/inputcontext");
}

int MInputContext::connectionCount = -1;

// Note: this class can also be used on plain Qt applications.
// This means that the functionality _can_ _not_ rely on
// MApplication or classes relying on it being initialized.


MInputContext::MInputContext(QObject *parent)
    : QInputContext(parent),
      active(false),
      inputPanelState(InputPanelHidden),
      imServer(0),
      ownsMComponentData(false),
      correctionEnabled(false),
      styleContainer(0),
      connectedObject(0),
      pasteAvailable(false),
      copyAllowed(true),
      redirectKeys(false),
      objectPath(QString("%1%2").arg(DBusCallbackPath).arg(++connectionCount))
{
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

    sipHideTimer.setSingleShot(true);
    sipHideTimer.setInterval(SoftwareInputPanelHideTimer);
    connect(&sipHideTimer, SIGNAL(timeout()), SLOT(hideOnFocusOut()));

    // ensure MComponentData is initialized, normally it should be.
    // NOTE: this can be removed if/when MTheme is better separated from MComponentData
    if (MComponentData::instance() == 0) {
        QStringList args = qApp->arguments();
        int argc = 1;
        QString appName = args[0];
        char *argv[1];
        QByteArray appNameArray = appName.toLocal8Bit();
        argv[0] = appNameArray.data();

        MComponentData::setLoadMInputContext(false);
        new MComponentData(argc, argv);
        ownsMComponentData = true;
    }

    connectToDBus();

    styleContainer = new MPreeditStyleContainer;
    styleContainer->initialize("DefaultStyle", "MPreeditStyle", 0);

    connect(MInputMethodState::instance(),
            SIGNAL(activeWindowOrientationAngleChanged(M::OrientationAngle)),
            this, SLOT(notifyOrientationChange(M::OrientationAngle)));

    connect(MInputMethodState::instance(),
            SIGNAL(toolbarRegistered(int, QString)),
            this, SLOT(notifyToolbarRegistered(int, QString)));

    connect(MInputMethodState::instance(),
            SIGNAL(toolbarUnregistered(int)),
            this, SLOT(notifyToolbarUnregistered(int)));

    connect(MInputMethodState::instance(),
            SIGNAL(toolbarItemAttributeChanged(int, QString, QString, QVariant)),
            this, SLOT(notifyToolbarItemAttributeChanged(int, QString, QString, QVariant)));
}


MInputContext::~MInputContext()
{
    delete imServer;

    delete styleContainer;

    if (ownsMComponentData) {
        delete MComponentData::instance();
    }
}


void MInputContext::connectToDBus()
{
    qDebug() << __PRETTY_FUNCTION__;
#ifdef QT_DBUS
    imServer = new QtDBusIMServerProxy(dbusObjectPath());

    // connect methods we are offering to DBus
    new MInputContextAdaptor(this);

    if (!QDBusConnection::sessionBus().registerObject(dbusObjectPath(), this)) {
        qCritical("MInputContext failed to register object via D-Bus: %s",
                  dbusObjectPath().toAscii().data());
    }

#else
    MDBusGlibInputContextAdaptor *inputContextAdaptor
        = M_DBUS_GLIB_INPUT_CONTEXT_ADAPTOR(
            g_object_new(M_TYPE_DBUS_GLIB_INPUT_CONTEXT_ADAPTOR, NULL));

    inputContextAdaptor->inputContext = this;
    imServer = new GlibDBusIMServerProxy(G_OBJECT(inputContextAdaptor), DBusCallbackPath);
#endif

    connect(imServer, SIGNAL(dbusConnected()), this, SLOT(onDBusConnection()));
    connect(imServer, SIGNAL(dbusDisconnected()), this, SLOT(onDBusDisconnection()));
}

bool MInputContext::event(QEvent *event)
{
    if (event->type() == MPreeditInjectionEvent::eventNumber()) {
        if (correctionEnabled) {
            MPreeditInjectionEvent *injectionEvent
                = dynamic_cast<MPreeditInjectionEvent *>(event);

            if (injectionEvent == 0) {
                return false;
            }

            mDebug("MInputContext") << "MInputContext got preedit injection:"
                                    << injectionEvent->preedit();
            // send the injected preedit to input method server and back to the widget with proper
            // styling
            imServer->setPreedit(injectionEvent->preedit());
            updatePreedit(injectionEvent->preedit(), PreeditDefault);

            event->accept();
            return true;

        } else {
            mDebug("MInputContext")
                << "MInputContext ignored preedit injection because correction is disabled";
            return false;
        }
    }

    return QInputContext::event(event);
}


QString MInputContext::identifierName()
{
    mDebug("MInputContext") << "in" << __PRETTY_FUNCTION__;
    return "MInputContext";
}


bool MInputContext::isComposing() const
{
    mDebug("MInputContext") << "in" << __PRETTY_FUNCTION__;
    return true; // FIXME
}


QString MInputContext::language()
{
    mDebug("MInputContext") << "in" << __PRETTY_FUNCTION__;
    return "EN"; // FIXME
}

QString MInputContext::dbusObjectPath() const
{
    return objectPath;
}

void MInputContext::reset()
{
    mDebug("MInputContext") << "in" << __PRETTY_FUNCTION__;

    // send existing preedit to widget, documentation unclear whether this is
    // allowed, but trolls gave permission to use it. Most of qt's internal 
    // input methods do the same thing.
    QInputMethodEvent event;
    event.setCommitString(preedit);
    sendEvent(event);
    preedit.clear();

    // reset input method server
    imServer->reset();
}


void MInputContext::update()
{
    mDebug("MInputContext") << "in" << __PRETTY_FUNCTION__;

    const QWidget *const focused = focusWidget();

    if (focused == 0) {
        return;
    }

    // get the state information of currently focused widget, and pass it to input method server
    QMap<QString, QVariant> stateInformation = getStateInformation();

    imServer->updateWidgetInformation(stateInformation, false);
}


void MInputContext::mouseHandler(int x, QMouseEvent *event)
{
    Q_UNUSED(x);

    mDebug("MInputContext") << "in" << __PRETTY_FUNCTION__;
    mDebug("MInputContext") << " event pos: " << event->globalPos() << " cursor pos:" << x;

    // input method server needs to be informed about clicks
    if (event->type() == QEvent::MouseButtonRelease) {

        // Query preedit rectangle and pass it to im server if found.
        // In case of plain QT application, null rectangle will be passed.
        QWidget *focused = focusWidget();
        QRect preeditRect;

        if (focused) {
            Qt::InputMethodQuery query
                = static_cast<Qt::InputMethodQuery>(M::PreeditRectangleQuery);
            preeditRect = focused->inputMethodQuery(query).toRect();
        }

        imServer->mouseClickedOnPreedit(event->globalPos(), preeditRect);
    }
}


void MInputContext::setFocusWidget(QWidget *focused)
{
    QObject *focusedObject = focused;
    QGraphicsItem *focusItem = 0;
    bool copyAvailable = false;

    mDebug("MInputContext") << "in" << __PRETTY_FUNCTION__ << focused;
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

            // Notify whatever application's orientation is currently.
            M::OrientationAngle angle
                = MInputMethodState::instance()->activeWindowOrientationAngle();
            notifyOrientationChange(angle);
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

        // FIXME: paste status doesn't update if paste becomes available from application side
        pasteAvailable = !QApplication::clipboard()->text().isEmpty();

    } else {
        copyAllowed = false;
        imServer->updateWidgetInformation(stateInformation, true);
    }

    // show or hide Copy/Paste button on input method server
    manageCopyPasteState(copyAvailable);

    if (inputPanelState == InputPanelShowPending && focused) {
        imServer->showInputMethod();
        inputPanelState = InputPanelShown;
    }

    if (connectedObject) {
        disconnect(connectedObject, SIGNAL(copyAvailable(bool)), this, 0);
        connectedObject = 0;
    }

    const char *signalName = "copyAvailable(bool)";

    if (focusedObject) {
        mDebug("MInputContext") << __PRETTY_FUNCTION__ << "signal index" << focusedObject
                                << focusedObject->metaObject()->indexOfSignal(signalName);
    }

    if (focusedObject && focusedObject->metaObject()
            && focusedObject->metaObject()->indexOfSignal(signalName) != -1) {
        connect(focusedObject, SIGNAL(copyAvailable(bool)),
                this, SLOT(manageCopyPasteState(bool)));
        connectedObject = focusedObject;
    }
}


bool MInputContext::filterEvent(const QEvent *event)
{
    bool eaten = false;

    switch (event->type()) {
    case QEvent::RequestSoftwareInputPanel:
        mDebug("MInputContext") << "got event" << event->type();
        sipHideTimer.stop();

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
        mDebug("MInputContext") << "got event" << event->type();
        sipHideTimer.start();
        eaten = true;
        break;

    case QEvent::KeyPress:
    case QEvent::KeyRelease:
        if (redirectKeys) {
            const QKeyEvent *key = static_cast<const QKeyEvent *>(event);
            imServer->processKeyEvent(key->type(), static_cast<Qt::Key>(key->key()),
                                      key->modifiers(), key->text(), key->isAutoRepeat(),
                                      key->count(), key->nativeScanCode(),
                                      key->nativeModifiers());
            eaten = true;
        }
        break;

    default:
        break;
    }

    return eaten;
}


void MInputContext::hideOnFocusOut()
{
    imServer->hideInputMethod();

    inputPanelState = InputPanelHidden;
}


void MInputContext::activationLostEvent()
{
    active = false;
    inputPanelState = InputPanelHidden;
}


void MInputContext::imInitiatedHide()
{
    mDebug("MInputContext") << "in" << __PRETTY_FUNCTION__;
    inputPanelState = InputPanelHidden;

    // need to remove focus from the current text entry
    if (focusWidget()) {
        QGraphicsView *graphicsView = qobject_cast<QGraphicsView *>(focusWidget());

        if (graphicsView && graphicsView->scene()) {
            // inside qgraphics we remove the focus from item focused there
            QGraphicsScene *scene = graphicsView->scene();
            QGraphicsItem *item = scene->focusItem();
            if (item) {
                item->clearFocus();
            }

        } else {
            focusWidget()->clearFocus();
        }
    }
}


void MInputContext::commitString(const QString &string)
{
    mDebug("MInputContext") << "in" << __PRETTY_FUNCTION__;
    mTimestamp("MInputContext", string);

    preedit.clear();

    QInputMethodEvent event;
    event.setCommitString(string);

    sendEvent(event);
}


void MInputContext::updatePreedit(const QString &string, PreeditFace preeditFace)
{
    mDebug("MInputContext") << "in" << __PRETTY_FUNCTION__ << "preedit:" << string;
    mTimestamp("MInputContext", string);

    preedit = string;

    // update style mode
    switch (preeditFace) {
    case PreeditNoCandidates:
        styleContainer->setModeNoCandidates();
        break;

    case PreeditKeyPress:
        styleContainer->setModeKeyPress();
        break;

    case PreeditDefault:
    default:
        styleContainer->setModeDefault();
    }

    // set proper formatting
    QTextCharFormat format;
    format.merge(standardFormat(QInputContext::PreeditFormat));
    format.setUnderlineStyle((*styleContainer)->underline());
    QColor color = (*styleContainer)->backgroundColor();

    if (color.isValid()) {
        format.setBackground(color);
    }

    color = (*styleContainer)->fontColor();

    if (color.isValid()) {
        format.setForeground(color);
    }

    QList<QInputMethodEvent::Attribute> attributes;
    attributes << QInputMethodEvent::Attribute(QInputMethodEvent::TextFormat, 0,
               string.length(), format);

    QInputMethodEvent event(string, attributes);

    sendEvent(event);
}


void MInputContext::keyEvent(int type, int key, int modifiers, const QString &text,
                             bool autoRepeat, int count)
{
    mDebug("MInputContext") << "in" << __PRETTY_FUNCTION__;

    if (focusWidget() == 0) {
        return;
    }

    // just construct an event instance out of the parameters.
    QKeyEvent event(static_cast<QEvent::Type>(type), key,
                    static_cast<Qt::KeyboardModifiers>(modifiers),
                    text, autoRepeat, count);

    QCoreApplication::sendEvent(focusWidget(), &event);
}


void MInputContext::updateInputMethodArea(const QList<QVariant> &data)
{
    QRect rect;
    if (!data.isEmpty())
        rect = data.at(0).toRect();

    MInputMethodState::instance()->setInputMethodArea(rect);
}


void MInputContext::setGlobalCorrectionEnabled(bool enabled)
{
    correctionEnabled = enabled;
}


QRect MInputContext::preeditRectangle(bool &valid) const
{
    Qt::InputMethodQuery query = static_cast<Qt::InputMethodQuery>(M::PreeditRectangleQuery);
    QVariant queryResult = focusWidget()->inputMethodQuery(query);

    valid = queryResult.isValid();
    return queryResult.toRect();
}


void MInputContext::copy()
{
    bool ok = false;

    if (connectedObject) {
        ok = QMetaObject::invokeMethod(connectedObject, "copy", Qt::DirectConnection);
    }

    mDebug("MInputContext") << __PRETTY_FUNCTION__ << "result=" << ok;

    if (!ok) {
        // send Ctrl-Ckey event because suitable slot was not found
        keyEvent(QEvent::KeyPress, Qt::Key_C, Qt::ControlModifier, "", false, 1);
        keyEvent(QEvent::KeyRelease, Qt::Key_C, Qt::ControlModifier, "", false, 1);
    }

    pasteAvailable = !QApplication::clipboard()->text().isEmpty(); //verify if something was copied
}


void MInputContext::paste()
{
    bool ok = false;

    if (connectedObject) {
        ok = QMetaObject::invokeMethod(connectedObject, "paste", Qt::DirectConnection);
    }

    mDebug("MInputContext") << __PRETTY_FUNCTION__ << "result=" << ok;

    if (!ok) {
        // send Ctrl-V key event because suitable slot was not found
        keyEvent(QEvent::KeyPress, Qt::Key_V, Qt::ControlModifier, "", false, 1);
        keyEvent(QEvent::KeyRelease, Qt::Key_V, Qt::ControlModifier, "", false, 1);
    }

    QApplication::clipboard()->clear();
    pasteAvailable = false;
    manageCopyPasteState(false); // there is no chance to have selection after paste
}


void MInputContext::onDBusDisconnection()
{
    qDebug() << __PRETTY_FUNCTION__;
    active = false;
    redirectKeys = false;
    inputPanelState = InputPanelHidden;
    MInputMethodState::instance()->setInputMethodArea(QRect());
}

void MInputContext::onDBusConnection()
{
    qDebug() << __PRETTY_FUNCTION__;
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
        imServer->showInputMethod();
        inputPanelState = InputPanelShown;
    }
}

void MInputContext::manageCopyPasteState(bool copyAvailable)
{
    imServer->setCopyPasteState(copyAvailable && copyAllowed, pasteAvailable);
}


void MInputContext::notifyOrientationChange(M::OrientationAngle orientation)
{
    // can get called from signal so cannot be sure we are really currently active
    if (active) {
        imServer->appOrientationChanged(static_cast<int>(orientation));
    }
}


void MInputContext::notifyToolbarRegistered(int id, const QString &fileName)
{
    imServer->registerToolbar(id, fileName);
}

void MInputContext::notifyToolbarUnregistered(int id)
{
    imServer->unregisterToolbar(id);
}

void MInputContext::notifyToolbarItemAttributeChanged(int id, const QString &item,
                                                      const QString &attribute,
                                                      const QVariant& value)
{
    imServer->setToolbarItemAttribute(id, item,
                                      attribute, value);
}

M::TextContentType MInputContext::contentType(Qt::InputMethodHints hints) const
{
    M::TextContentType type = M::FreeTextContentType;
    hints &= Qt::ImhExclusiveInputMask;

    if (hints == Qt::ImhFormattedNumbersOnly || hints == Qt::ImhDigitsOnly) {
        type = M::NumberContentType;
    } else if (hints == Qt::ImhDialableCharactersOnly) {
        type = M::PhoneNumberContentType;
    } else if (hints == Qt::ImhEmailCharactersOnly) {
        type = M::EmailContentType;
    } else if (hints == Qt::ImhUrlCharactersOnly) {
        type = M::UrlContentType;
    }

    return type;
}

void MInputContext::setRedirectKeys(bool enabled)
{
    redirectKeys = enabled;
}

void MInputContext::setDetectableAutoRepeat(bool enabled)
{
    Bool detectableAutoRepeatSupported(False);
    XkbSetDetectableAutoRepeat(QX11Info::display(), enabled ? True : False,
                               &detectableAutoRepeatSupported);
    if (detectableAutoRepeatSupported == False) {
        qWarning() << "Detectable autorepeat not supported.";
    }
}

QMap<QString, QVariant> MInputContext::getStateInformation() const
{
    QMap<QString, QVariant> stateInformation;
    const QWidget *focused = focusWidget();

    stateInformation["focusState"] = focused != NULL;

    if (!focused) {
        return stateInformation;
    }

    // visualization priority
    QVariant queryResult = focused->inputMethodQuery(
        static_cast<Qt::InputMethodQuery>(M::VisualizationPriorityQuery));

    if (queryResult.isValid()) {
        stateInformation["visualizationPriority"] = queryResult.toBool();
    }

    // toolbar id
    queryResult = focused->inputMethodQuery(
        static_cast<Qt::InputMethodQuery>(M::InputMethodToolbarIdQuery));

    if (queryResult.isValid()) {
        stateInformation["toolbarId"] = queryResult.toInt();
    }

    // toolbar file
    queryResult = focused->inputMethodQuery(
        static_cast<Qt::InputMethodQuery>(M::InputMethodToolbarQuery));

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

    Qt::InputMethodHints hints = focused->inputMethodHints();
    const QGraphicsView *const graphicsView = qobject_cast<const QGraphicsView *>(focused);

    if (graphicsView) {
        // focused->inputMethodHints() always returns 0
        // therefore we explicitly call inputMethodHints with focused item
        const QGraphicsScene *const scene = graphicsView->scene();
        const QGraphicsItem *const focusedObject = scene ? scene->focusItem() : 0;
        if (focusedObject) {
            hints = focusedObject->inputMethodHints();
        }
    }

    // content type value
    stateInformation["contentType"] = contentType(hints);

    // error correction support
    queryResult = focused->inputMethodQuery(
        static_cast<Qt::InputMethodQuery>(M::ImCorrectionEnabledQuery));

    if (queryResult.isValid()) {
        stateInformation["correctionEnabled"] = queryResult.toBool();
    }

    stateInformation["predictionEnabled"] = !(hints & Qt::ImhNoPredictiveText);
    stateInformation["autocapitalizationEnabled"] = !(hints & Qt::ImhNoAutoUppercase);

    // input method mode
    queryResult = focused->inputMethodQuery(
        static_cast<Qt::InputMethodQuery>(M::ImModeQuery));

    M::InputMethodMode inputMethodMode = M::InputMethodModeNormal;
    if (queryResult.isValid()) {
        inputMethodMode = static_cast<M::InputMethodMode>(queryResult.toInt());
    }
    stateInformation["inputMethodMode"] = inputMethodMode;

    // is text selected
    queryResult = focused->inputMethodQuery(Qt::ImCurrentSelection);

    if (queryResult.isValid())
        stateInformation["hasSelection"] = !(queryResult.toString().isEmpty());

    // window id for transient VKB - it is enough to just query the focused widget here:
    stateInformation["winId"] = static_cast<int>(focused->effectiveWinId());

    return stateInformation;
}

