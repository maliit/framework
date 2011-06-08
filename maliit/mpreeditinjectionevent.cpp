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

MPreeditInjectionEventPrivate::MPreeditInjectionEventPrivate(const QString &preedit, int eventCursorPos)
    : preedit(preedit),
      eventCursorPosition(eventCursorPos),
      replacementStart(0),
      replacementLength(0)
{
    // nothing
}


MPreeditInjectionEventPrivate::~MPreeditInjectionEventPrivate()
{
    // nothing
}


///////////////////////
// class implementation


MPreeditInjectionEvent::MPreeditInjectionEvent(const QString &preedit)
    : QEvent(MPreeditInjectionEvent::eventNumber()),
      d_ptr(new MPreeditInjectionEventPrivate(preedit, -1))
{
    setAccepted(false);
}

MPreeditInjectionEvent::MPreeditInjectionEvent(const QString &preedit, int eventCursorPosition)
    : QEvent(MPreeditInjectionEvent::eventNumber()),
      d_ptr(new MPreeditInjectionEventPrivate(preedit, eventCursorPosition))
{
    setAccepted(false);
}

MPreeditInjectionEvent::~MPreeditInjectionEvent()
{
    delete d_ptr;
}


QString MPreeditInjectionEvent::preedit() const
{
    Q_D(const MPreeditInjectionEvent);
    return d->preedit;
}

int MPreeditInjectionEvent::eventCursorPosition() const
{
    Q_D(const MPreeditInjectionEvent);
    return d->eventCursorPosition;
}

void MPreeditInjectionEvent::setReplacement(int replacementStart, int replacementLength)
{
    Q_D(MPreeditInjectionEvent);
    d->replacementStart = replacementStart;
    d->replacementLength = replacementLength;
}

int MPreeditInjectionEvent::replacementStart() const
{
    Q_D(const MPreeditInjectionEvent);
    return d->replacementStart;
}

int MPreeditInjectionEvent::replacementLength() const
{
    Q_D(const MPreeditInjectionEvent);
    return d->replacementLength;
}


// static
QEvent::Type MPreeditInjectionEvent::eventNumber()
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
