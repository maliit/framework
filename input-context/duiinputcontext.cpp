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

#include "duiinputcontext.h"

#include <QInputContext>
#include <QCoreApplication>
#include <QDBusInterface>
#include <QDBusConnectionInterface>
#include <QKeyEvent>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QTextFormat>
#include <QApplication>
#include <QClipboard>

#include <DuiPreeditInjectionEvent>
#include <DuiTheme>
#include <DuiComponentData>
#include <DuiDebug>
#include <DuiLibrary>
#include <DuiInputMethodState>

#include "duiinputcontextadaptor.h"
#include "duipreeditstyle.h"
#include "duitimestamp.h"

DUI_LIBRARY

namespace
{
    const int SoftwareInputPanelHideTimer = 500;

    const QString DBusServiceName("org.maemo.duiinputmethodserver1");
    const QString DBusPath("/org/maemo/duiinputmethodserver1");
    const QString DBusInterface("org.maemo.duiinputmethodserver1");

    const QString DBusCallbackPath("/org/maemo/duiinputcontext");
}

int DuiInputContext::connectionCount = -1;

// Note: this class can also be used on plain Qt applications.
// This means that the functionality _can_ _not_ rely on
// DuiApplication or classes relying on it being initialized.


DuiInputContext::DuiInputContext(QObject *parent)
    : QInputContext(parent),
      active(false),
      inputPanelState(InputPanelHidden),
      iface(0),
      ownsDuiComponentData(false),
      correctionEnabled(false),
      styleContainer(0),
      connectedObject(0),
      pasteAvailable(false),
      copyAllowed(true),
      redirectKeys(false),
      objectPath(QString("%1%2").arg(DBusCallbackPath).arg(++connectionCount))
{
    sipHideTimer.setSingleShot(true);
    sipHideTimer.setInterval(SoftwareInputPanelHideTimer);
    connect(&sipHideTimer, SIGNAL(timeout()), SLOT(hideOnFocusOut()));

    // ensure DuiComponentData is initialized, normally it should be.
    // NOTE: this can be removed if/when DuiTheme is better separated from DuiComponentData
    if (DuiComponentData::instance() == 0) {
        QStringList args = qApp->arguments();
        int argc = 1;
        QString appName = args[0];
        char *argv[1];
        QByteArray appNameArray = appName.toLocal8Bit();
        argv[0] = appNameArray.data();

        DuiComponentData::setLoadDuiInputContext(false);
        new DuiComponentData(argc, argv);
        ownsDuiComponentData = true;
    }

    connectToDBus();

    styleContainer = new DuiPreeditStyleContainer;
    styleContainer->initialize("DefaultStyle", "DuiPreeditStyle", 0);

    connect(DuiInputMethodState::instance(),
            SIGNAL(activeWindowOrientationAngleChanged(Dui::OrientationAngle)),
            this, SLOT(notifyOrientationChange(Dui::OrientationAngle)));
}


DuiInputContext::~DuiInputContext()
{
    delete iface;

    delete styleContainer;

    if (ownsDuiComponentData) {
        delete DuiComponentData::instance();
    }
}


void DuiInputContext::connectToDBus()
{
    QDBusConnection connection = QDBusConnection::sessionBus();

    if (connection.isConnected() == false) {
        duiDebug("DuiInputContext") << "Cannot connect to the DBus session bus";
    }

    QDBusConnectionInterface *connectionInterface = connection.interface();

    // connect methods we are offering to DBus
    new DuiInputContextAdaptor(this);

    bool success = connection.registerObject(dbusObjectPath(), this);

    if (!success) {
        qFatal("DuiInputContext failed to register object via D-Bus: %s",
               dbusObjectPath().toAscii().data());
    }

    // start to follow server changes
    connect(connectionInterface, SIGNAL(serviceOwnerChanged(QString, QString, QString)),
            this, SLOT(serviceChangeHandler(QString, QString, QString)));

    //iface MUST NOT be sibling of object of class derived from QDBusAbstractAdaptor
    //due to bug in Qt
    iface = new QDBusInterface(DBusServiceName, DBusPath, DBusInterface, connection, 0);

    // The input method server might not be running when DuiInputContext is created.
    // The iface validity is rechecked at serviceChangeHandler().
    if (!iface->isValid()) {
        duiDebug("DuiInputContext") << "DuiInputContext was unable to connect to input method server: "
                                    << connection.lastError().message();
    } else {
        registerContextObject();
    }
}

bool DuiInputContext::event(QEvent *event)
{
    if (event->type() == DuiPreeditInjectionEvent::eventNumber()) {
        if (correctionEnabled) {
            DuiPreeditInjectionEvent *injectionEvent
                = dynamic_cast<DuiPreeditInjectionEvent *>(event);

            if (injectionEvent == 0) {
                return false;
            }

            duiDebug("DuiInputContext") << "DuiInputContext got preedit injection:"
                                        << injectionEvent->preedit();
            // send the injected preedit to input method server and back to the widget with proper
            // styling
            iface->call(QDBus::NoBlock, "setPreedit", injectionEvent->preedit());
            updatePreedit(injectionEvent->preedit(), Dui::PreeditDefault);

            event->accept();
            return true;

        } else {
            duiDebug("DuiInputContext")
                    << "DuiInputContext ignored preedit injection because correction is disabled";
            return false;
        }
    }

    return QInputContext::event(event);
}


QString DuiInputContext::identifierName()
{
    duiDebug("DuiInputContext") << "in" << __PRETTY_FUNCTION__;
    return "DuiInputContext";
}


bool DuiInputContext::isComposing() const
{
    duiDebug("DuiInputContext") << "in" << __PRETTY_FUNCTION__;
    return true; // FIXME
}


QString DuiInputContext::language()
{
    duiDebug("DuiInputContext") << "in" << __PRETTY_FUNCTION__;
    return "EN"; // FIXME
}

QString DuiInputContext::dbusObjectPath() const
{
    return objectPath;
}

void DuiInputContext::reset()
{
    duiDebug("DuiInputContext") << "in" << __PRETTY_FUNCTION__;

    // reset input method server
    iface->call(QDBus::NoBlock, "reset");
}


void DuiInputContext::update()
{
    duiDebug("DuiInputContext") << "in" << __PRETTY_FUNCTION__;

    QWidget *focused = focusWidget();

    if (focused == 0) {
        return;
    }

    // get the state information of currently focused widget, and pass it to input method server
    QMap<QString, QVariant> stateInformation = getStateInformation();

    iface->call(QDBus::NoBlock, "updateWidgetInformation", stateInformation);
}


void DuiInputContext::mouseHandler(int x, QMouseEvent *event)
{
    Q_UNUSED(x);

    duiDebug("DuiInputContext") << "in" << __PRETTY_FUNCTION__;
    duiDebug("DuiInputContext") << " event pos: " << event->globalPos() << " cursor pos:" << x;

    // input method server needs to be informed about clicks
    if (event->type() == QEvent::MouseButtonRelease) {

        // Query preedit rectangle and pass it to im server if found.
        // In case of plain QT application, null rectangle will be passed.
        QWidget *focused = focusWidget();
        QRect preeditRect;

        if (focused) {
            Qt::InputMethodQuery query
                = static_cast<Qt::InputMethodQuery>(Dui::PreeditRectangleQuery);
            preeditRect = focused->inputMethodQuery(query).toRect();
        }

        iface->call(QDBus::NoBlock, "mouseClickedOnPreedit", event->globalPos(), preeditRect);
    }
}


void DuiInputContext::setFocusWidget(QWidget *focused)
{
    QObject *focusedObject = focused;
    QGraphicsItem *focusItem = 0;
    bool copyAvailable = false;

    duiDebug("DuiInputContext") << "in" << __PRETTY_FUNCTION__ << focused;
    QInputContext::setFocusWidget(focused);

    // get detailed focus information from inside qgraphicsview
    QGraphicsView *graphicsView = qobject_cast<QGraphicsView *>(focusWidget());

    if (graphicsView) {
        QGraphicsScene *scene = graphicsView->scene();
        focusItem = scene->focusItem();

        if (focusItem) {
            focusedObject = dynamic_cast<QObject *>(focusItem);
        }
    }

    if (focused) {
        // for non-null focus widgets, we'll have this context activated
        if (!active) {
            iface->call(QDBus::NoBlock, "activateContext");
            active = true;

            // Notify whatever application's orientation is currently.
            Dui::OrientationAngle angle
                = DuiInputMethodState::instance()->activeWindowOrientationAngle();
            notifyOrientationChange(angle);
        }

        QMap<QString, QVariant> stateInformation = getStateInformation();
        iface->call(QDBus::NoBlock, "updateWidgetInformation", stateInformation);

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
    }

    // show or hide Copy/Paste button on input method server
    manageCopyPasteState(copyAvailable);

    if (inputPanelState == InputPanelShowPending && focused) {
        iface->call(QDBus::NoBlock, "showInputMethod");
        inputPanelState = InputPanelShown;
    }

    if (connectedObject) {
        disconnect(connectedObject, SIGNAL(copyAvailable(bool)), this, 0);
        connectedObject = 0;
    }

    const char *signalName = "copyAvailable(bool)";

    if (focusedObject) {
        duiDebug("DuiInputContext") << __PRETTY_FUNCTION__ << "signal index" << focusedObject
                                    << focusedObject->metaObject()->indexOfSignal(signalName);
    }

    if (focusedObject && focusedObject->metaObject()
            && focusedObject->metaObject()->indexOfSignal(signalName) != -1) {
        connect(focusedObject, SIGNAL(copyAvailable(bool)),
                this, SLOT(manageCopyPasteState(bool)));
        connectedObject = focusedObject;
    }
}


bool DuiInputContext::filterEvent(const QEvent *event)
{
    bool eaten = false;

    if (event->type() == QEvent::RequestSoftwareInputPanel) {
        duiDebug("DuiInputContext") << "got event" << event->type();
        sipHideTimer.stop();

        if (!active || focusWidget() == 0) {
            // in case SIP request comes without a properly focused widget, we
            // don't ask input method server to be shown. It's done when the next widget
            // is focused, so the widget state information can be set.
            inputPanelState = InputPanelShowPending;

        } else {
            // note: could do this also if panel was hidden
            iface->call(QDBus::NoBlock, "showInputMethod");
            inputPanelState = InputPanelShown;
        }

        eaten = true;

    } else if (event->type() == QEvent::CloseSoftwareInputPanel) {
        duiDebug("DuiInputContext") << "got event" << event->type();
        sipHideTimer.start();
        eaten = true;

    } else if ((event->type() == QEvent::KeyPress) || (event->type() == QEvent::KeyRelease)) {
        const QKeyEvent *key = static_cast<const QKeyEvent *>(event);
        if (redirectKeys) {
            iface->call(QDBus::NoBlock, "processKeyEvent", static_cast<int>(key->type()),
                        key->key(), static_cast<int>(key->modifiers()), key->text(),
                        key->isAutoRepeat(), key->count(),
                        static_cast<int>(key->nativeScanCode()));
            eaten = true;

        }
    }

    return eaten;
}


void DuiInputContext::hideOnFocusOut()
{
    iface->call(QDBus::NoBlock, "hideInputMethod");

    inputPanelState = InputPanelHidden;
}


void DuiInputContext::activationLostEvent()
{
    active = false;
    inputPanelState = InputPanelHidden;
}


void DuiInputContext::imInitiatedHide()
{
    duiDebug("DuiInputContext") << "in" << __PRETTY_FUNCTION__;
    inputPanelState = InputPanelHidden;

    // need to remove focus from the current text entry
    if (focusWidget() != 0) {
        QGraphicsView *graphicsView = qobject_cast<QGraphicsView *>(focusWidget());

        if (graphicsView) {
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


void DuiInputContext::commitString(const QString &string)
{
    duiDebug("DuiInputContext") << "in" << __PRETTY_FUNCTION__;
    duiTimestamp("DuiInputContext", string);
    QInputMethodEvent event;
    event.setCommitString(string);

    //FIXME: quick hack to use Qt4.6
    QWidget *focused = QApplication::focusWidget();
    if (focused) {
        QApplication::sendEvent(focused, &event);
    }
}


void DuiInputContext::updatePreedit(const QString &string, Dui::PreeditFace preeditFace)
{
    duiDebug("DuiInputContext") << "in" << __PRETTY_FUNCTION__ << "preedit:" << string;
    duiTimestamp("DuiInputContext", string);

    // update style mode
    switch (preeditFace) {
    case Dui::PreeditNoCandidates:
        styleContainer->setModeNoCandidates();
        break;

    case Dui::PreeditDefault:
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

    //FIXME: quick hack to use Qt4.6
    QWidget *focused = QApplication::focusWidget();
    if (focused) {
        QApplication::sendEvent(focused, &event);
    }
}


void DuiInputContext::keyEvent(int type, int key, int modifiers, const QString &text,
                               bool autoRepeat, int count)
{
    duiDebug("DuiInputContext") << "in" << __PRETTY_FUNCTION__;

    if (focusWidget() == 0) {
        return;
    }

    // just construct an event instance out of the parameters.
    QKeyEvent event(static_cast<QEvent::Type>(type), key,
                    static_cast<Qt::KeyboardModifiers>(modifiers),
                    text, autoRepeat, count);

    QCoreApplication::sendEvent(focusWidget(), &event);
}


void DuiInputContext::updateInputMethodArea(const QList<QVariant> &data)
{
    QRect rect;
    if (!data.isEmpty())
        rect = data.at(0).toRect();

    DuiInputMethodState::instance()->setInputMethodArea(rect);
}


void DuiInputContext::setGlobalCorrectionEnabled(bool enabled)
{
    correctionEnabled = enabled;
}


QRect DuiInputContext::preeditRectangle(bool &valid) const
{
    Qt::InputMethodQuery query = static_cast<Qt::InputMethodQuery>(Dui::PreeditRectangleQuery);
    QVariant queryResult = focusWidget()->inputMethodQuery(query);

    valid = queryResult.isValid();
    return queryResult.toRect();
}


void DuiInputContext::copy()
{
    bool ok = false;

    if (connectedObject) {
        ok = QMetaObject::invokeMethod(connectedObject, "copy", Qt::DirectConnection);
    }

    duiDebug("DuiInputContext") << __PRETTY_FUNCTION__ << "result=" << ok;

    if (!ok) {
        // send Ctrl-Ckey event because suitable slot was not found
        keyEvent(QEvent::KeyPress, Qt::Key_C, Qt::ControlModifier, "", false, 1);
        keyEvent(QEvent::KeyRelease, Qt::Key_C, Qt::ControlModifier, "", false, 1);
    }

    pasteAvailable = !QApplication::clipboard()->text().isEmpty(); //verify if something was copied
}


void DuiInputContext::paste()
{
    bool ok = false;

    if (connectedObject) {
        ok = QMetaObject::invokeMethod(connectedObject, "paste", Qt::DirectConnection);
    }

    duiDebug("DuiInputContext") << __PRETTY_FUNCTION__ << "result=" << ok;

    if (!ok) {
        // send Ctrl-V key event because suitable slot was not found
        keyEvent(QEvent::KeyPress, Qt::Key_V, Qt::ControlModifier, "", false, 1);
        keyEvent(QEvent::KeyRelease, Qt::Key_V, Qt::ControlModifier, "", false, 1);
    }

    QApplication::clipboard()->clear();
    pasteAvailable = false;
    manageCopyPasteState(false); // there is no chance to have selection after paste
}


void DuiInputContext::registerContextObject()
{
    iface->call(QDBus::NoBlock, "setContextObject", dbusObjectPath());
}


void DuiInputContext::serviceChangeHandler(const QString &name, const QString &oldOwner,
                                           const QString &newOwner)
{
    Q_UNUSED(oldOwner);

    // if the input method server service owner changes, we need to register our
    // callback object again
    if (name == DBusServiceName) {
        active = false;
        redirectKeys = false;
        inputPanelState = InputPanelHidden;

        if (!newOwner.isEmpty()) {
            if (!iface->isValid()) {
                // dbus interface don't seem to become valid if on construction the server
                // wasn't found. workaround this by creating the interface again
                duiDebug("DuiInputContext") << "recreating dbus interface";
                delete iface;
                QDBusConnection connection = QDBusConnection::sessionBus();
                iface = new QDBusInterface(DBusServiceName, DBusPath, DBusInterface, connection, 0);
            }

            duiDebug("DuiInputContext")
                << "InputContext: service owner changed. Registering callback again";
            registerContextObject();
        }
    }
}

void DuiInputContext::manageCopyPasteState(bool copyAvailable)
{
    iface->call(QDBus::NoBlock, "setCopyPasteState", copyAvailable && copyAllowed, pasteAvailable);
}


void DuiInputContext::notifyOrientationChange(Dui::OrientationAngle orientation)
{
    // can get called from signal so cannot be sure we are really currently active
    if (active) {
        iface->call(QDBus::NoBlock, "appOrientationChanged", (int) orientation);
    }
}


Dui::TextContentType DuiInputContext::contentType(Qt::InputMethodHints hints) const
{
    Dui::TextContentType type = Dui::FreeTextContentType;

    hints &= Qt::ImhExclusiveInputMask;
    if (hints == Qt::ImhFormattedNumbersOnly) {
        type = Dui::NumberContentType;
    } else if (hints == Qt::ImhDialableCharactersOnly) {
        type = Dui::PhoneNumberContentType;
    } else if (hints == Qt::ImhEmailCharactersOnly) {
        type = Dui::EmailContentType;
    } else if (hints == Qt::ImhUrlCharactersOnly) {
        type = Dui::UrlContentType;
    }

    return type;
}

void DuiInputContext::setRedirectKeys(bool enabled)
{
    redirectKeys = enabled;
}

QMap<QString, QVariant> DuiInputContext::getStateInformation() const
{
    QMap<QString, QVariant> stateInformation;
    const QWidget *focused = focusWidget();

    if (focused == 0) {
        return stateInformation;
    }

    // visualization priority
    Qt::InputMethodQuery query = static_cast<Qt::InputMethodQuery>(Dui::VisualizationPriorityQuery);
    QVariant queryResult = focused->inputMethodQuery(query);

    if (queryResult.isValid()) {
        stateInformation["visualizationPriority"] = queryResult.toBool();
    }

    // toolbar
    query = static_cast<Qt::InputMethodQuery>(Dui::InputMethodToolbarQuery);
    queryResult = focused->inputMethodQuery(query);

    if (queryResult.isValid()) {
        stateInformation["toolbar"] = queryResult.toString();
    }

    // surrounding text
    query = Qt::ImSurroundingText;
    queryResult = focused->inputMethodQuery(query);

    if (queryResult.isValid()) {
        stateInformation["surroundingText"] = queryResult.toString();
    }

    // cursor pos
    query = Qt::ImCursorPosition;
    queryResult = focused->inputMethodQuery(query);

    if (queryResult.isValid()) {
        stateInformation["cursorPosition"] = queryResult.toInt();
    }

    Qt::InputMethodHints hints;
    const QGraphicsView *graphicsView = qobject_cast<const QGraphicsView *>(focused);

    if (graphicsView) {
        // focused->inputMethodHints() always returns 0
        // therefore we explicitly call inputMethodHints with focused item
        QGraphicsScene *scene = graphicsView->scene();
        QGraphicsItem *focusedObject = scene ? scene->focusItem() : 0;
        if (focusedObject) {
            hints = focusedObject->inputMethodHints();
        }
    } else {
        hints = focused->inputMethodHints();
    }

    // content type value
    stateInformation["contentType"] = contentType(hints);

    // error correction support
    query = static_cast<Qt::InputMethodQuery>(Dui::ImCorrectionEnabledQuery);
    queryResult = focused->inputMethodQuery(query);

    if (queryResult.isValid()) {
        stateInformation["correctionEnabled"] = queryResult.toBool();
    }

    const bool predictionEnabled = !(hints & Qt::ImhNoPredictiveText);
    stateInformation["predictionEnabled"] = predictionEnabled;

    const bool autoCapitalizationEnabled = !(hints & Qt::ImhNoAutoUppercase);
    stateInformation["autocapitalizationEnabled"] = autoCapitalizationEnabled;

    // input method mode
    query = static_cast<Qt::InputMethodQuery>(Dui::ImModeQuery);
    queryResult = focused->inputMethodQuery(query);

    Dui::InputMethodMode inputMethodMode = Dui::InputMethodModeNormal;
    if (queryResult.isValid()) {
        inputMethodMode = static_cast<Dui::InputMethodMode>(queryResult.toInt());
    }
    stateInformation["inputMethodMode"] = inputMethodMode;

    // is text selected
    query = Qt::ImCurrentSelection;
    queryResult = focused->inputMethodQuery(query);

    if (queryResult.isValid())
        stateInformation["hasSelection"] = !(queryResult.toString().isEmpty());

    return stateInformation;
}

