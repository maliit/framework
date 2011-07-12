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

// This file is based on mkeyboardstatetracker_p.h from libmeegotouch

#ifndef MIMHWKEYBOARDTRACKER_P_H
#define MIMHWKEYBOARDTRACKER_P_H

#ifdef HAVE_CONTEXTSUBSCRIBER
#include <QScopedPointer>

class ContextProperty;
#endif

class MImHwKeyboardTracker;

class MImHwKeyboardTrackerPrivate
{
public:
    MImHwKeyboardTrackerPrivate(MImHwKeyboardTracker *q_ptr);
    ~MImHwKeyboardTrackerPrivate();

#ifdef HAVE_CONTEXTSUBSCRIBER
    QScopedPointer<ContextProperty> keyboardOpenProperty;
#endif

    bool present;
};


#endif // MIMHWKEYBOARDTRACKER_P_H
