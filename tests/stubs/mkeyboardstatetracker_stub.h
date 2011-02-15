/* * This file is part of meego-im-framework *
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
#ifndef MKEYBOARDSTATETRACKER_STUB_H
#define MKEYBOARDSTATETRACKER_STUB_H

#include <QDebug>

#include "fakeproperty.h"
#include "mimhwkeyboardtracker.h"

/**
 * MKeyboardStateTracker stub class.
 * To fake MKeyboardStateTracker operations, derive from this class
 * and implement the methods you want to fake. Instantiate your
 * derived stub class and assign it to gMKeyboardStateTracker
 * global variable.
 */
class MImHwKeyboardTrackerStub
{
public:
    MImHwKeyboardTrackerStub();

    virtual void mimHwKeyboardTrackerConstructor(const MImHwKeyboardTracker *instance);

    virtual bool isPresent();
    virtual bool isOpen();
    virtual void setOpenState(bool state);

protected:
    FakeProperty openProperty;
};

MImHwKeyboardTrackerStub::MImHwKeyboardTrackerStub()
    : openProperty("/maemo/InternalKeyboard/Open")
{
}

void MImHwKeyboardTrackerStub::mimHwKeyboardTrackerConstructor(const MImHwKeyboardTracker *instance)
{
    openProperty.setValue(QVariant(false));
    QObject::connect(&openProperty, SIGNAL(valueChanged()), instance, SIGNAL(stateChanged()));
}

bool MImHwKeyboardTrackerStub::isPresent()
{
    return true;
}

bool MImHwKeyboardTrackerStub::isOpen()
{
    return openProperty.value().toBool();
}

void MImHwKeyboardTrackerStub::setOpenState(bool state)
{
    openProperty.setValue(QVariant(state));
}

MImHwKeyboardTrackerStub gDefaultMKeyboardStateTrackerStub;

/**
 * This is the stub class instance used by the system. If you want to alter behaviour,
 * derive your stub class from MKeyboardStateTrackerStub, implement the methods you want to
 * fake, create an instance of your stub class and assign the instance into this global variable.
 */
MImHwKeyboardTrackerStub *gMKeyboardStateTrackerStub = &gDefaultMKeyboardStateTrackerStub;

/**
 * These are the proxy method implementations of MKeyboardStateTracker. They will
 * call the stub object methods of the gMKeyboardStateTracker.
 */

MImHwKeyboardTracker::MImHwKeyboardTracker()
{
    gMKeyboardStateTrackerStub->mimHwKeyboardTrackerConstructor(this);
}

bool MImHwKeyboardTracker::isPresent() const
{
    return gMKeyboardStateTrackerStub->isPresent();
}

bool MImHwKeyboardTracker::isOpen() const
{
    return gMKeyboardStateTrackerStub->isOpen();
}

#endif
