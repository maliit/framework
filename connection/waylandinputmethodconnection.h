/* * This file is part of Maliit framework *
 *
 * Copyright (C) 2012 Canonical Ltd
 *
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 */

#ifndef WAYLANDINPUTMETHODCONNECTION_H
#define WAYLANDINPUTMETHODCONNECTION_H

#include <maliit/namespace.h>
#include "minputcontextconnection.h"

#include <QtCore>

Q_DECLARE_LOGGING_CATEGORY(lcWaylandConnection)

class WaylandInputMethodConnectionPrivate;

/*! \internal
 * \ingroup maliitserver
 * \brief Input method communication implementation between the Weston
 * and the input method server.
 */
class WaylandInputMethodConnection : public MInputContextConnection
{
    Q_OBJECT
    Q_DISABLE_COPY(WaylandInputMethodConnection)
    Q_DECLARE_PRIVATE(WaylandInputMethodConnection)

public:
    explicit WaylandInputMethodConnection();
    virtual ~WaylandInputMethodConnection();

    virtual void sendPreeditString(const QString &string,
                                   const QList<Maliit::PreeditTextFormat> &preedit_formats,
                                   int replacement_start = 0,
                                   int replacement_length = 0,
                                   int cursor_pos = -1);
    virtual void sendCommitString(const QString &string,
                                  int replace_start = 0,
                                  int replace_length = 0,
                                  int cursor_pos = -1);
    virtual void sendKeyEvent(const QKeyEvent &key_event,
                              Maliit::EventRequestType request_type);
    virtual void setSelection(int start,
                              int length);
    virtual QString selection(bool &valid);

private:
    const QScopedPointer<WaylandInputMethodConnectionPrivate> d_ptr;
};
//! \internal_end

#endif
