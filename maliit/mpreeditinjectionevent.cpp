/* * This file is part of meego-im-framework *
 *
 * Copyright (C) 2010, 2011 Nokia Corporation and/or its subsidiary(-ies).
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

// Based on mpreeditinjectionevent.cpp from libmeegotouch

#include <QString>
#include <QMutex>

#include "mpreeditinjectionevent.h"
#include "mpreeditinjectionevent_p.h"

static int sPreeditEventNumber = -1;
static QMutex sPreeditInjectionMutex;

namespace Maliit {

PreeditInjectionEventPrivate::PreeditInjectionEventPrivate(const QString &preedit, int eventCursorPos)
    : preedit(preedit),
      eventCursorPosition(eventCursorPos),
      replacementStart(0),
      replacementLength(0)
{
    // nothing
}


PreeditInjectionEventPrivate::~PreeditInjectionEventPrivate()
{
    // nothing
}


///////////////////////
// class implementation


PreeditInjectionEvent::PreeditInjectionEvent(const QString &preedit)
    : QEvent(PreeditInjectionEvent::eventNumber()),
      d_ptr(new PreeditInjectionEventPrivate(preedit, -1))
{
    setAccepted(false);
}

PreeditInjectionEvent::PreeditInjectionEvent(const QString &preedit, int eventCursorPosition)
    : QEvent(PreeditInjectionEvent::eventNumber()),
      d_ptr(new PreeditInjectionEventPrivate(preedit, eventCursorPosition))
{
    setAccepted(false);
}

PreeditInjectionEvent::~PreeditInjectionEvent()
{
    delete d_ptr;
}


QString PreeditInjectionEvent::preedit() const
{
    Q_D(const PreeditInjectionEvent);
    return d->preedit;
}

int PreeditInjectionEvent::eventCursorPosition() const
{
    Q_D(const PreeditInjectionEvent);
    return d->eventCursorPosition;
}

void PreeditInjectionEvent::setReplacement(int replacementStart, int replacementLength)
{
    Q_D(PreeditInjectionEvent);
    d->replacementStart = replacementStart;
    d->replacementLength = replacementLength;
}

int PreeditInjectionEvent::replacementStart() const
{
    Q_D(const PreeditInjectionEvent);
    return d->replacementStart;
}

int PreeditInjectionEvent::replacementLength() const
{
    Q_D(const PreeditInjectionEvent);
    return d->replacementLength;
}


// static
QEvent::Type PreeditInjectionEvent::eventNumber()
{
    if (sPreeditEventNumber < 0) {
        // no event number yet registered, do it now
        sPreeditInjectionMutex.lock();

        if (sPreeditEventNumber < 0) {
            sPreeditEventNumber = QEvent::registerEventType();
        }

        sPreeditInjectionMutex.unlock();
    }

    return static_cast<QEvent::Type>(sPreeditEventNumber);
}

}
