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

// This file is based on mkeyboardstatetracker_p.h from libmeegotouch

#ifndef MIMHWKEYBOARDTRACKER_P_H
#define MIMHWKEYBOARDTRACKER_P_H

#include <QFile>

class MImHwKeyboardTracker;

class MImHwKeyboardTrackerPrivate
    : public QObject
{
    Q_OBJECT

public:
    explicit MImHwKeyboardTrackerPrivate(MImHwKeyboardTracker *q_ptr);
    ~MImHwKeyboardTrackerPrivate();

    void detectEvdev();
    void tryEvdevDevice(const char *device);

    QFile *evdevFile;
    int evdevTabletModePending;
    bool evdevTabletMode;

    bool present;

public Q_SLOTS:
    void evdevEvent();

Q_SIGNALS:
    void stateChanged();
};


#endif // MIMHWKEYBOARDTRACKER_P_H
