/* * This file is part of Maliit framework *
 *
 * Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 *
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 */

// This file is based on mkeyboardstatetracker.h from libmeegotouch

#ifndef MIMKEYBOARDSTATETRACKER_H
#define MIMKEYBOARDSTATETRACKER_H

#include <QObject>
#include <QScopedPointer>

class MImHwKeyboardTrackerPrivate;

//! \internal
/*! \ingroup maliitserver
 * \brief Class responsible for tracking the hardware keyboard properties and
 * signaling events.
 *
 * Singleton class. Using isPresent() queries whether the device supports
 * hardware keyboard or not. If hardware keyboard is supported, using isOpen()
 * to query its current state. Signal stateChanged will be emitted when the
 * hardware keyboard state is changed.
 */
class MImHwKeyboardTracker
    : public QObject
{
    Q_OBJECT

public:
    MImHwKeyboardTracker();
    virtual ~MImHwKeyboardTracker();

    //! \brief Returns whether device has a hardware keyboard.
    bool isPresent() const;

    //! \brief Returns whether hardware keyboard is opened.
    bool isOpen() const;

Q_SIGNALS:
    //! \brief Emitted whenever the hardware keyboard state changed.
    void stateChanged();

private:
    const QScopedPointer<MImHwKeyboardTrackerPrivate> d_ptr;

    Q_DISABLE_COPY(MImHwKeyboardTracker)
    Q_DECLARE_PRIVATE(MImHwKeyboardTracker)
};
//! \internal_end

#endif // MIMKEYBOARDSTATETRACKER_H
