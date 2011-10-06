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

#include "mkeyoverridequick.h"
#include "mkeyoverridequick_p.h"

MKeyOverrideQuickPrivate::MKeyOverrideQuickPrivate(const QString &label,
                                                   const QString &icon,
                                                   bool highlighted,
                                                   bool enabled)
    : defaultLabel(label),
      defaultIcon(icon),
      defaultHighlighted(highlighted),
      defaultEnabled(enabled)
{
    // EMPTY
}

void MKeyOverrideQuickPrivate::assign(const MKeyOverrideQuickPrivate &other)
{
    defaultLabel        = other.defaultLabel;
    defaultIcon         = other.defaultIcon;
    defaultHighlighted  = other.defaultHighlighted;
    defaultEnabled      = other.defaultEnabled;
}

MKeyOverrideQuick::MKeyOverrideQuick(const QString &keyId)
    : MKeyOverride(keyId),
      d_ptr(new MKeyOverrideQuickPrivate(label(),
                                         icon(),
                                         highlighted(),
                                         enabled()))
{
    // EMPTY
}

MKeyOverrideQuick::MKeyOverrideQuick(const MKeyOverrideQuick &other)
    : MKeyOverride(other),
      d_ptr(new MKeyOverrideQuickPrivate(other.defaultLabel(),
                                         other.defaultIcon(),
                                         other.defaultHighlighted(),
                                         other.defaultEnabled()))
{
    // EMPTY
}

MKeyOverrideQuick::~MKeyOverrideQuick()
{
    delete d_ptr;
}

const MKeyOverrideQuick & MKeyOverrideQuick::operator=(const MKeyOverrideQuick &other)
{
    Q_D(MKeyOverrideQuick);

    d->assign(*other.d_ptr);

    return *this;
}

QString MKeyOverrideQuick::defaultLabel() const
{
    Q_D(const MKeyOverrideQuick);

    return d->defaultLabel;
}


QString MKeyOverrideQuick::defaultIcon() const
{
    Q_D(const MKeyOverrideQuick);

    return d->defaultIcon;
}

bool MKeyOverrideQuick::defaultHighlighted() const
{
    Q_D(const MKeyOverrideQuick);

    return d->defaultHighlighted;
}

bool MKeyOverrideQuick::defaultEnabled() const
{
    Q_D(const MKeyOverrideQuick);

    return d->defaultEnabled;
}

void MKeyOverrideQuick::setDefaultLabel(const QString &label)
{
    Q_D(MKeyOverrideQuick);

    if (d->defaultLabel != label) {
        d->defaultLabel = label;
        Q_EMIT defaultLabelChanged(label);
    }
}

void MKeyOverrideQuick::setDefaultIcon(const QString &icon)
{
    Q_D(MKeyOverrideQuick);

    if (d->defaultIcon != icon) {
        d->defaultIcon = icon;
        Q_EMIT defaultIconChanged(icon);
    }
}

void MKeyOverrideQuick::setDefaultHighlighted(bool highlighted)
{
    Q_D(MKeyOverrideQuick);

    if (d->defaultHighlighted != highlighted) {
        d->defaultHighlighted = highlighted;
        Q_EMIT defaultHighlightedChanged(highlighted);
    }
}

void MKeyOverrideQuick::setDefaultEnabled(bool enabled)
{
    Q_D(MKeyOverrideQuick);

    if (d->defaultEnabled != enabled) {
        d->defaultEnabled = enabled;
        Q_EMIT defaultEnabledChanged(enabled);
    }
}

void MKeyOverrideQuick::useDefaults()
{
    Q_D(MKeyOverrideQuick);

    setLabel(d->defaultLabel);
    setIcon(d->defaultIcon);
    setHighlighted(d->defaultHighlighted);
    setEnabled(d->defaultEnabled);
}
