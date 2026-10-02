/* * This file is part of Maliit framework *
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 */

#ifndef DBUSWAYLANDINPUTCONTEXTCONNECTION_H
#define DBUSWAYLANDINPUTCONTEXTCONNECTION_H

#include "dbusinputcontextconnection.h"

#include <QScopedPointer>

namespace Maliit {
namespace Wayland {
class InputMethodTransport;
}
}

/*! \internal
 * \ingroup maliitserver
 * \brief Serves D-Bus input contexts and a zwp_input_method_v1 client at once.
 *
 * Both kinds of clients share one activeConnection, so the base class decides
 * which one is active; outbound calls are routed to the active client only.
 */
class DBusWaylandInputContextConnection : public DBusInputContextConnection
{
    Q_OBJECT
    Q_DISABLE_COPY(DBusWaylandInputContextConnection)

public:
    explicit DBusWaylandInputContextConnection(const QSharedPointer<Maliit::Server::DBus::Address> &address);
    virtual ~DBusWaylandInputContextConnection();

    //! \reimp
    virtual void sendPreeditString(const QString &string,
                                   const QList<Maliit::PreeditTextFormat> &preeditFormats,
                                   int replacementStart = 0,
                                   int replacementLength = 0,
                                   int cursorPos = -1);
    virtual void sendCommitString(const QString &string,
                                  int replaceStart = 0,
                                  int replaceLength = 0,
                                  int cursorPos = -1);
    virtual void sendKeyEvent(const QKeyEvent &keyEvent,
                              Maliit::EventRequestType requestType);
    virtual void setSelection(int start, int length);
    virtual QString selection(bool &valid);
    virtual void setLanguage(const QString &language);
    virtual void sendActivationLostEvent();
    //! \reimp_end

private:
    bool waylandClientActive() const;

    const QScopedPointer<Maliit::Wayland::InputMethodTransport> m_transport;
};
//! \internal_end

#endif // DBUSWAYLANDINPUTCONTEXTCONNECTION_H
