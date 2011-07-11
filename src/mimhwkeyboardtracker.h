/* * This file is part of meego-im-framework *
 *
 * Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

// This file is based on mkeyboardstatetracker.h from libmeegotouch

#ifndef MIMKEYBOARDSTATETRACKER_H
#define MIMKEYBOARDSTATETRACKER_H

#include <QObject>
#include <QScopedPointer>

//! \internal
/*!
 * Class responsible for tracking the hardware keyboard properties and signaling events.
 * It is designed as singleton. Using isPresent() can query whether the device supports
 * hardware keyboard or not. If hardware keyboard is supported, using isOpen() to query
 * its current state. Signal stateChanged will be emitted when hardware keyboard state is changed.
 */

class MImHwKeyboardTrackerPrivate;

class MImHwKeyboardTracker
    : public QObject
{
    Q_OBJECT

public:
    //! \brief Returns the singleton instance.
    static MImHwKeyboardTracker *instance();

    //! \brief Returns whether device has a hardware keyboard.
    bool isPresent() const;

    //! \brief Returns whether hardware keyboard is opened.
    bool isOpen() const;

Q_SIGNALS:
    //! \brief Emitted whenever the hardware keyboard state changed.
    void stateChanged();

private:
    MImHwKeyboardTracker();
    virtual ~MImHwKeyboardTracker();

    const QScopedPointer<MImHwKeyboardTrackerPrivate> d_ptr;

    Q_DISABLE_COPY(MImHwKeyboardTracker)
    Q_DECLARE_PRIVATE(MImHwKeyboardTracker)
};
//! \internal_end

#endif // MIMKEYBOARDSTATETRACKER_H
