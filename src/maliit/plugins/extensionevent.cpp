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

#include <maliit/plugins/extensionevent.h>
#include <maliit/plugins/extensionevent_p.h>

MImExtensionEventPrivate::~MImExtensionEventPrivate()
{}

MImExtensionEvent::MImExtensionEvent(Type type)
    : d_ptr(new MImExtensionEventPrivate)
{
    d_ptr->type = type;
}

MImExtensionEvent::MImExtensionEvent(MImExtensionEventPrivate *dd,
                                     Type type)
    : d_ptr(dd)
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
