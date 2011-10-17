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

#include "mimupdateevent.h"
#include "mimextensionevent_p.h"

class MImUpdateEventPrivate
    : public MImExtensionEventPrivate
{
public:
    QMap<QString, QVariant> update;
<<<<<<< HEAD
    QStringList changedProperties;
};

MImUpdateEvent::MImUpdateEvent(const QMap<QString, QVariant> &update,
                               const QStringList &changedProperties)
=======
    QStringList changedKeys;
};

MImUpdateEvent::MImUpdateEvent(const QMap<QString, QVariant> &update,
                               const QStringList &changedKeys)
>>>>>>> New: MImUpdateEvent can be processed by plugins to detect widget changes
    : MImExtensionEvent(new MImUpdateEventPrivate, MImExtensionEvent::Update)
    , d_ptr(static_cast<MImUpdateEventPrivate * const>(MImExtensionEvent::d_ptr))
{
    Q_D(MImUpdateEvent);
    d->update = update;
<<<<<<< HEAD
    d->changedProperties = changedProperties;
=======
    d->changedKeys = changedKeys;
>>>>>>> New: MImUpdateEvent can be processed by plugins to detect widget changes
}

QVariant MImUpdateEvent::value(const QString &key) const
{
    Q_D(const MImUpdateEvent);
    return d->update.value(key);
}

<<<<<<< HEAD
QStringList MImUpdateEvent::propertiesChanged() const
{
    Q_D(const MImUpdateEvent);
    return d->changedProperties;
=======
QStringList MImUpdateEvent::changedKeys() const
{
    Q_D(const MImUpdateEvent);
    return d->changedKeys;
>>>>>>> New: MImUpdateEvent can be processed by plugins to detect widget changes
}
