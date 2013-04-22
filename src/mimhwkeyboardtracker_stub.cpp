/* * This file is part of Maliit framework *
 *
 * Copyright (C) 2013 Gianni Valdambrini <aleister@develer.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 */


#include "mimhwkeyboardtracker.h"


class MImHwKeyboardTrackerPrivate
    : public QObject
{
public:
    explicit MImHwKeyboardTrackerPrivate();
};


MImHwKeyboardTrackerPrivate::MImHwKeyboardTrackerPrivate()
{

}


MImHwKeyboardTracker::MImHwKeyboardTracker()
    : QObject(),
      d_ptr(new MImHwKeyboardTrackerPrivate)
{
}

MImHwKeyboardTracker::~MImHwKeyboardTracker()
{
}

bool MImHwKeyboardTracker::isPresent() const
{
    return false;
}

bool MImHwKeyboardTracker::isOpen() const
{
   return false;
}
