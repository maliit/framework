/* * This file is part of meego-im-framework *
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



#include <QtAlgorithms>
#include "mkeyoverride.h"
#include "mkeyoverride_p.h"

MKeyOverridePrivate::MKeyOverridePrivate()
    : highlighted(false),
      enabled(true)
{
}

void MKeyOverridePrivate::assign(const MKeyOverridePrivate &other)
{
    keyId     = other.keyId;
    label     = other.label;
    icon      = other.icon;
    highlighted  = other.highlighted;
    enabled   = other.enabled;
}

MKeyOverride::MKeyOverride(const QString &keyId)
    : QObject(),
    d_ptr(new MKeyOverridePrivate)
{
    d_ptr->keyId = keyId;
}

MKeyOverride::MKeyOverride(const MKeyOverride &other)
    : QObject(),
    d_ptr(new MKeyOverridePrivate)
{
    *this = other;
}

MKeyOverride::~MKeyOverride()
{
    delete d_ptr;
}

const MKeyOverride & MKeyOverride::operator=(const MKeyOverride &other)
{
    Q_D(MKeyOverride);

    d->assign(*other.d_ptr);

    return *this;
}

QString MKeyOverride::keyId() const
{
    Q_D(const MKeyOverride);

    return d->keyId;
}

QString MKeyOverride::label() const
{
    Q_D(const MKeyOverride);

    return d->label;
}


QString MKeyOverride::icon() const
{
    Q_D(const MKeyOverride);

    return d->icon;
}

bool MKeyOverride::highlighted() const
{
    Q_D(const MKeyOverride);

    return d->highlighted;
}

bool MKeyOverride::enabled() const
{
    Q_D(const MKeyOverride);

    return d->enabled;
}

void MKeyOverride::setLabel(const QString &label)
{
    Q_D(MKeyOverride);

    if (d->label != label) {
        d->label = label;
        Q_EMIT labelChanged(label);
        Q_EMIT keyAttributesChanged(keyId(), Label);
    }
}

void MKeyOverride::setIcon(const QString &icon)
{
    Q_D(MKeyOverride);

    if (d->icon != icon) {
        d->icon = icon;
        Q_EMIT iconChanged(icon);
        Q_EMIT keyAttributesChanged(keyId(), Icon);
    }
}

void MKeyOverride::setHighlighted(bool highlighted)
{
    Q_D(MKeyOverride);

    if (d->highlighted != highlighted) {
        d->highlighted = highlighted;
        Q_EMIT highlightedChanged(highlighted);
        Q_EMIT keyAttributesChanged(keyId(), Highlighted);
    }
}

void MKeyOverride::setEnabled(bool enabled)
{
    Q_D(MKeyOverride);

    if (d->enabled != enabled) {
        d->enabled = enabled;
        Q_EMIT enabledChanged(enabled);
        Q_EMIT keyAttributesChanged(keyId(), Enabled);
    }
}

