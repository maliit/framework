/* * This file is part of Maliit framework *
 *
 * Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 *
 * Contact: maliit-discuss@lists.maliit.org
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 */

#include <QtCore>

#include <maliit/plugins/updateevent_p.h>
#include <maliit/plugins/updatereceiver.h>
#include <maliit/plugins/updateevent.h>
#include <maliit/namespace.h>
#include <maliit/namespaceinternal.h>

// Choosing inheritance instead of typedef, as we might want to add
// receiver-specific data later.
class MImUpdateReceiverPrivate
    : public MImUpdateEventPrivate
{};

MImUpdateReceiver::MImUpdateReceiver(QObject *parent)
    : QObject(parent)
    , d_ptr(new MImUpdateReceiverPrivate)
{}

MImUpdateReceiver::~MImUpdateReceiver()
{
    // TODO: Find out why using QScopedPointer for the PIMPL fails to compile.
    delete d_ptr;
}

void MImUpdateReceiver::process(MImUpdateEvent *ev)
{
    if (!ev) {
        return;
    }

    // TODO: Replace this ever-growing, switch-like structure with a more
    // concise solution, based on Qt reflection (via QMetaObject and Qt
    // properties).
    Q_D(MImUpdateReceiver);
    d->changedProperties = ev->d_func()->changedProperties;
    d->update = ev->d_func()->update;

    bool changed = false;
    bool value = ev->westernNumericInputEnforced(&changed);

    if (changed) {
        Q_EMIT westernNumericInputEnforcedChanged(value);
    }

    changed = false;
    value = ev->preferNumbers(&changed);

    if (changed) {
        Q_EMIT preferNumbersChanged(value);
    }

    changed = false;
    value = ev->translucentInputMethod(&changed);

    if (changed) {
        Q_EMIT translucentInputMethodChanged(value);
    }
}

bool MImUpdateReceiver::westernNumericInputEnforced() const
{
    Q_D(const MImUpdateReceiver);
    return d->extractProperty(Maliit::InputMethodQuery::westernNumericInputEnforced).toBool();
}

bool MImUpdateReceiver::preferNumbers() const
{
    Q_D(const MImUpdateReceiver);
    return d->isFlagSet(Qt::ImhPreferNumbers);
}

bool MImUpdateReceiver::translucentInputMethod() const
{
    Q_D(const MImUpdateReceiver);
    return d->extractProperty(Maliit::InputMethodQuery::translucentInputMethod).toBool();
}
