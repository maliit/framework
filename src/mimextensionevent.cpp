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

#include "mimextensionevent.h"

class MImExtensionEventPrivate
{
public:
    MImExtensionEvent::Type type;
};

MImExtensionEvent::MImExtensionEvent(Type type)
    : d_ptr(new MImExtensionEventPrivate)
{
    d_ptr->type = type;
}

MImExtensionEvent::~MImExtensionEvent()
{
    Q_D(MImExtensionEvent);
    delete d;
}

MImExtensionEvent::Type MImExtensionEvent::type() const
{
    Q_D(const MImExtensionEvent);
    return d->type;
}
