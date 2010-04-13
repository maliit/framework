/* * This file is part of dui-im-framework *
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
#ifndef DUIKEYBOARDSTATETRACKER_STUB_H
#define DUIKEYBOARDSTATETRACKER_STUB_H

#include <DuiKeyboardStateTracker>
#include <QDebug>

#include "fakeproperty.h"

/**
 * DuiKeyboardStateTracker stub class.
 * To fake DuiKeyboardStateTracker operations, derive from this class
 * and implement the methods you want to fake. Instantiate your
 * derived stub class and assign it to gDuiKeyboardStateTracker
 * global variable.
 */
class DuiKeyboardStateTrackerStub
{
public:
    DuiKeyboardStateTrackerStub();
    virtual void duiKeyboardStateTrackerConstructor(const DuiKeyboardStateTracker *instance);
    virtual bool isPresent();
    virtual bool isOpen();
    virtual void setOpenState(bool state);

protected:
    FakeProperty openProperty;
};

DuiKeyboardStateTrackerStub::DuiKeyboardStateTrackerStub()
    : openProperty("/maemo/InternalKeyboard/Open")
{
}

void DuiKeyboardStateTrackerStub::duiKeyboardStateTrackerConstructor(const DuiKeyboardStateTracker *instance)
{
    openProperty.setValue(QVariant(false));
    QObject::connect(&openProperty, SIGNAL(stateChanged()), instance, SIGNAL(valueChanged()));
}

bool DuiKeyboardStateTrackerStub::isPresent()
{
    return true;
}

bool DuiKeyboardStateTrackerStub::isOpen()
{
    return openProperty.value().toBool();
}

void DuiKeyboardStateTrackerStub::setOpenState(bool state)
{
    openProperty.setValue(QVariant(state));
}

DuiKeyboardStateTrackerStub gDefaultDuiKeyboardStateTrackerStub;

/**
 * This is the stub class instance used by the system. If you want to alter behaviour,
 * derive your stub class from DuiKeyboardStateTrackerStub, implement the methods you want to
 * fake, create an instance of your stub class and assign the instance into this global variable.
 */
DuiKeyboardStateTrackerStub *gDuiKeyboardStateTrackerStub = &gDefaultDuiKeyboardStateTrackerStub;

/**
 * These are the proxy method implementations of DuiKeyboardStateTracker. They will
 * call the stub object methods of the gDuiKeyboardStateTracker.
 */

DuiKeyboardStateTracker::DuiKeyboardStateTracker()
    : d_ptr(0)
{
    gDuiKeyboardStateTrackerStub->duiKeyboardStateTrackerConstructor(this);
}

bool DuiKeyboardStateTracker::isPresent() const
{
    return gDuiKeyboardStateTrackerStub->isPresent();
}

bool DuiKeyboardStateTracker::isOpen() const
{
    return gDuiKeyboardStateTrackerStub->isOpen();
}

#endif
