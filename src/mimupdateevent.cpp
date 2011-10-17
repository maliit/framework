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
    QStringList changedProperties;
};

MImUpdateEvent::MImUpdateEvent(const QMap<QString, QVariant> &update,
                               const QStringList &changedProperties)
    : MImExtensionEvent(new MImUpdateEventPrivate, MImExtensionEvent::Update)
{
    Q_D(MImUpdateEvent);
    d->update = update;
    d->changedProperties = changedProperties;
}

QVariant MImUpdateEvent::value(const QString &key) const
{
    Q_D(const MImUpdateEvent);
    return d->update.value(key);
}

QStringList MImUpdateEvent::propertiesChanged() const
{
    Q_D(const MImUpdateEvent);
    return d->changedProperties;
}
