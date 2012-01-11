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

#include <maliit/plugins/updateevent.h>
#include <maliit/plugins/updateevent_p.h>
#include <maliit/plugins/extensionevent_p.h>
#include <maliit/namespace.h>
#include <maliit/namespaceinternal.h>

MImUpdateEventPrivate::MImUpdateEventPrivate()
    : update()
    , changedProperties()
    , lastHints(Qt::ImhNone)
{}

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

QVariant MImUpdateEventPrivate::extractProperty(const QString &key,
                                                bool *changed) const
{
    if (changed) {
        *changed = changedProperties.contains(key);
    }

    return update.value(key);
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
    return static_cast<Qt::InputMethodHints>(
        d->extractProperty(Maliit::Internal::inputMethodHints, changed).toLongLong());
}

bool MImUpdateEvent::westernNumericInputEnforced(bool *changed) const
{
    Q_D(const MImUpdateEvent);
    return d->extractProperty(Maliit::InputMethodQuery::westernNumericInputEnforced, changed).toBool();
}

bool MImUpdateEvent::preferNumbers(bool *changed) const
{
    Q_D(const MImUpdateEvent);
    return d->isFlagSet(Qt::ImhPreferNumbers, changed);
}

bool MImUpdateEvent::translucentInputMethod(bool *changed) const
{
    Q_D(const MImUpdateEvent);
    return d->extractProperty(Maliit::InputMethodQuery::translucentInputMethod, changed).toBool();
}
