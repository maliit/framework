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
#include "mimupdateevent_p.h"
#include "mimextensionevent_p.h"
#include "maliit/namespace.h"

MImUpdateEventPrivate::MImUpdateEventPrivate(const QMap<QString, QVariant> &newUpdate,
                                             const QStringList &newChangedProperties,
                                             const Qt::InputMethodHints &newLastHints)
    : update(newUpdate)
    , changedProperties(newChangedProperties)
    , lastHints(newLastHints)
{}

bool MImUpdateEventPrivate::isFlagSet(Qt::InputMethodHint hint,
                                      bool *changed) const
{
    bool result = false;

    if (update.contains(Maliit::Internal::inputMethodHints)) {
        const Qt::InputMethodHints hints(static_cast<Qt::InputMethodHints>(
                                             update.value(Maliit::Internal::inputMethodHints).toLongLong()));

        result = (hints & hint);
    }

    if (changed) {
        *changed = (result != (lastHints & hint));
    }

    return result;
}

MImUpdateEvent::MImUpdateEvent(const QMap<QString, QVariant> &update,
                               const QStringList &changedProperties)
    : MImExtensionEvent(new MImUpdateEventPrivate(update, changedProperties, Qt::InputMethodHints()),
                        MImExtensionEvent::Update)
{}

MImUpdateEvent::MImUpdateEvent(const QMap<QString, QVariant> &update,
                               const QStringList &changedProperties,
                               const Qt::InputMethodHints &lastHints)
    : MImExtensionEvent(new MImUpdateEventPrivate(update, changedProperties, lastHints),
                        MImExtensionEvent::Update)
{}

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

Qt::InputMethodHints MImUpdateEvent::hints(bool *changed) const
{
    Q_D(const MImUpdateEvent);

    if (changed) {
        *changed = d->changedProperties.contains(Maliit::Internal::inputMethodHints);
    }

    return static_cast<Qt::InputMethodHints>(d->update.value(Maliit::Internal::inputMethodHints).toLongLong());
}

bool MImUpdateEvent::westernNumericInputEnforced(bool *changed) const
{
    Q_D(const MImUpdateEvent);

    if (changed) {
        *changed = d->changedProperties.contains(Maliit::InputMethodQuery::westernNumericInputEnforced);
    }

    return d->update.value(Maliit::InputMethodQuery::westernNumericInputEnforced).toBool();
}

bool MImUpdateEvent::preferNumbers(bool *changed) const
{
    Q_D(const MImUpdateEvent);
    return d->isFlagSet(Qt::ImhPreferNumbers, changed);
}
