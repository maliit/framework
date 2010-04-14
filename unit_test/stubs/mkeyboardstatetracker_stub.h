/* * This file is part of m-im-framework *
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

#include <MKeyboardStateTracker>
#include <QDebug>

#include "fakeproperty.h"

/**
 * MKeyboardStateTracker stub class.
 * To fake MKeyboardStateTracker operations, derive from this class
 * and implement the methods you want to fake. Instantiate your
 * derived stub class and assign it to gMKeyboardStateTracker
 * global variable.
 */
class MKeyboardStateTrackerStub
{
public:
    MKeyboardStateTrackerStub();
    virtual void mKeyboardStateTrackerConstructor(const MKeyboardStateTracker *instance);
    virtual bool isPresent();
    virtual bool isOpen();
    virtual void setOpenState(bool state);

protected:
    FakeProperty openProperty;
};

MKeyboardStateTrackerStub::MKeyboardStateTrackerStub()
    : openProperty("/maemo/InternalKeyboard/Open")
{
}

void MKeyboardStateTrackerStub::mKeyboardStateTrackerConstructor(const MKeyboardStateTracker *instance)
{
    openProperty.setValue(QVariant(false));
    QObject::connect(&openProperty, SIGNAL(stateChanged()), instance, SIGNAL(valueChanged()));
}

bool MKeyboardStateTrackerStub::isPresent()
{
    return true;
}

bool MKeyboardStateTrackerStub::isOpen()
{
    return openProperty.value().toBool();
}

void MKeyboardStateTrackerStub::setOpenState(bool state)
{
    openProperty.setValue(QVariant(state));
}

MKeyboardStateTrackerStub gDefaultMKeyboardStateTrackerStub;

/**
 * This is the stub class instance used by the system. If you want to alter behaviour,
 * derive your stub class from MKeyboardStateTrackerStub, implement the methods you want to
 * fake, create an instance of your stub class and assign the instance into this global variable.
 */
MKeyboardStateTrackerStub *gMKeyboardStateTrackerStub = &gDefaultMKeyboardStateTrackerStub;

/**
 * These are the proxy method implementations of MKeyboardStateTracker. They will
 * call the stub object methods of the gMKeyboardStateTracker.
 */

MKeyboardStateTracker::MKeyboardStateTracker()
    : d_ptr(0)
{
    gMKeyboardStateTrackerStub->mKeyboardStateTrackerConstructor(this);
}

bool MKeyboardStateTracker::isPresent() const
{
    return gMKeyboardStateTrackerStub->isPresent();
}

bool MKeyboardStateTracker::isOpen() const
{
    return gMKeyboardStateTrackerStub->isOpen();
}

#endif
