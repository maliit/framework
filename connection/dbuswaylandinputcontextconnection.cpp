/* * This file is part of Maliit framework *
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 */

#include "dbuswaylandinputcontextconnection.h"
#include "waylandinputmethodtransport.h"

#include <QKeyEvent>

namespace {

// TODO: Deduplicate it. Same values as in minputcontextconnection.cpp.
const char * const SurroundingTextAttribute = "surroundingText";
const char * const CursorPositionAttribute = "cursorPosition";

} // unnamed namespace

DBusWaylandInputContextConnection::DBusWaylandInputContextConnection(const QSharedPointer<Maliit::Server::DBus::Address> &address)
    : DBusInputContextConnection(address)
    , m_transport(new Maliit::Wayland::InputMethodTransport(this))
{
}

DBusWaylandInputContextConnection::~DBusWaylandInputContextConnection()
{
}

bool DBusWaylandInputContextConnection::waylandClientActive() const
{
    return activeConnection == Maliit::Wayland::ConnectionId;
}

void DBusWaylandInputContextConnection::sendPreeditString(const QString &string,
                                                          const QList<Maliit::PreeditTextFormat> &preeditFormats,
                                                          int replacementStart,
                                                          int replacementLength,
                                                          int cursorPos)
{
    if (!waylandClientActive()) {
        DBusInputContextConnection::sendPreeditString(string, preeditFormats,
                                                      replacementStart, replacementLength,
                                                      cursorPos);
        return;
    }

    if (!m_transport->context())
        return;

    MInputContextConnection::sendPreeditString(string, preeditFormats,
                                               replacementStart, replacementLength,
                                               cursorPos);

    m_transport->sendPreeditString(string, preeditFormats, replacementStart, replacementLength, cursorPos,
                                   widgetState().value(CursorPositionAttribute).toInt());
}

void DBusWaylandInputContextConnection::sendCommitString(const QString &string,
                                                         int replaceStart,
                                                         int replaceLength,
                                                         int cursorPos)
{
    if (!waylandClientActive()) {
        DBusInputContextConnection::sendCommitString(string, replaceStart, replaceLength, cursorPos);
        return;
    }

    if (!m_transport->context())
        return;

    MInputContextConnection::sendCommitString(string, replaceStart, replaceLength, cursorPos);

    m_transport->sendCommitString(string, replaceStart, replaceLength, cursorPos,
                                  widgetState().value(CursorPositionAttribute).toInt());
}

void DBusWaylandInputContextConnection::sendKeyEvent(const QKeyEvent &keyEvent,
                                                     Maliit::EventRequestType requestType)
{
    if (!waylandClientActive()) {
        DBusInputContextConnection::sendKeyEvent(keyEvent, requestType);
        return;
    }

    if (!m_transport->context())
        return;

    Maliit::Wayland::InputMethodTransport::Key key;
    if (!Maliit::Wayland::InputMethodTransport::keyFromQKeyEvent(keyEvent, &key))
        return;

    MInputContextConnection::sendKeyEvent(keyEvent, requestType);

    m_transport->sendKey(key, keyEvent.timestamp());
}

void DBusWaylandInputContextConnection::setSelection(int start, int length)
{
    if (!waylandClientActive()) {
        DBusInputContextConnection::setSelection(start, length);
        return;
    }

    m_transport->setSelection(start, length,
                              widgetState().value(SurroundingTextAttribute).toString());
}

QString DBusWaylandInputContextConnection::selection(bool &valid)
{
    if (!waylandClientActive())
        return DBusInputContextConnection::selection(valid);

    return m_transport->selection(valid);
}

void DBusWaylandInputContextConnection::setLanguage(const QString &language)
{
    // Always let the D-Bus side record the language: it is sent to the next
    // D-Bus client that becomes active.
    DBusInputContextConnection::setLanguage(language);

    if (waylandClientActive())
        m_transport->setLanguage(language);
}

void DBusWaylandInputContextConnection::sendActivationLostEvent()
{
    if (!waylandClientActive()) {
        DBusInputContextConnection::sendActivationLostEvent();
        return;
    }

    // Another client takes over while the Wayland client may still be
    // focused: don't leave a half-typed word behind as preedit.
    m_transport->sendPreeditString(QString(), QList<Maliit::PreeditTextFormat>(), 0, 0, 0, 0);
}
