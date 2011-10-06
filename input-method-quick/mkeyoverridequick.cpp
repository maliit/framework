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

#include <QtAlgorithms>

#include "mkeyoverridequick.h"
#include "mkeyoverridequick_p.h"

MKeyOverrideQuickPrivate::MKeyOverrideQuickPrivate(const QString &label,
                                                   const QString &icon,
                                                   bool highlighted,
                                                   bool enabled)
    : actualLabel(),
      actualIcon(),
      actualHighlighted(false),
      actualEnabled(false),
      defaultLabel(label),
      defaultIcon(icon),
      defaultHighlighted(highlighted),
      defaultEnabled(enabled),
      labelIsOverriden(false),
      iconIsOverriden(false),
      highlightedIsOverriden(false),
      enabledIsOverriden(false)
{
    // EMPTY
}

MKeyOverrideQuick::MKeyOverrideQuick()
    : QObject(),
      d_ptr(new MKeyOverrideQuickPrivate("",
                                         "",
                                         false,
                                         true))
{
    // EMPTY
}

MKeyOverrideQuick::~MKeyOverrideQuick()
{
    delete d_ptr;
}

QString MKeyOverrideQuick::label() const
{
    Q_D(const MKeyOverrideQuick);

    return d->actualLabel;
}


QString MKeyOverrideQuick::icon() const
{
    Q_D(const MKeyOverrideQuick);

    return d->actualIcon;
}

bool MKeyOverrideQuick::highlighted() const
{
    Q_D(const MKeyOverrideQuick);

    return d->actualHighlighted;
}

bool MKeyOverrideQuick::enabled() const
{
    Q_D(const MKeyOverrideQuick);

    return d->actualEnabled;
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

void MKeyOverrideQuick::overrideLabel(const QString &label)
{
    setLabel(label, true);
}

void MKeyOverrideQuick::overrideIcon(const QString &icon)
{
    setIcon(icon, true);
}

void MKeyOverrideQuick::overrideHighlighted(bool highlighted)
{
    setHighlighted(highlighted, true);
}

void MKeyOverrideQuick::overrideEnabled(bool enabled)
{
    setEnabled(enabled, true);
}

void MKeyOverrideQuick::setDefaultLabel(const QString &label)
{
    Q_D(MKeyOverrideQuick);

    if (d->defaultLabel != label) {
        d->defaultLabel = label;
        Q_EMIT defaultLabelChanged(label);
    }
    if (not d->labelIsOverriden) {
        useDefaultLabel();
    }
}

void MKeyOverrideQuick::setDefaultIcon(const QString &icon)
{
    Q_D(MKeyOverrideQuick);

    if (d->defaultIcon != icon) {
        d->defaultIcon = icon;
        Q_EMIT defaultIconChanged(icon);
    }
    if (not d->iconIsOverriden) {
        useDefaultIcon();
    }
}

void MKeyOverrideQuick::setDefaultHighlighted(bool highlighted)
{
    Q_D(MKeyOverrideQuick);

    if (d->defaultHighlighted != highlighted) {
        d->defaultHighlighted = highlighted;
        Q_EMIT defaultHighlightedChanged(highlighted);
    }
    if (not d->highlightedIsOverriden) {
        useDefaultHighlighted();
    }
}

void MKeyOverrideQuick::setDefaultEnabled(bool enabled)
{
    Q_D(MKeyOverrideQuick);

    if (d->defaultEnabled != enabled) {
        d->defaultEnabled = enabled;
        Q_EMIT defaultEnabledChanged(enabled);
    }
    if (not d->enabledIsOverriden) {
        useDefaultEnabled();
    }
}

void MKeyOverrideQuick::setLabel(const QString &label, bool overriden)
{
    Q_D(MKeyOverrideQuick);

    d->labelIsOverriden = overriden;
    if (d->actualLabel != label) {
        d->actualLabel = label;
        Q_EMIT labelChanged(label);
    }
}

void MKeyOverrideQuick::setIcon(const QString &icon, bool overriden)
{
    Q_D(MKeyOverrideQuick);

    d->iconIsOverriden = overriden;
    if (d->actualIcon != icon) {
        d->actualIcon = icon;
        Q_EMIT iconChanged(icon);
    }
}

void MKeyOverrideQuick::setHighlighted(bool highlighted, bool overriden)
{
    Q_D(MKeyOverrideQuick);

    d->highlightedIsOverriden = overriden;
    if (d->actualHighlighted != highlighted) {
        d->actualHighlighted = highlighted;
        Q_EMIT highlightedChanged(highlighted);
    }
}

void MKeyOverrideQuick::setEnabled(bool enabled, bool overriden)
{
    Q_D(MKeyOverrideQuick);

    d->enabledIsOverriden = overriden;
    if (d->actualEnabled != enabled) {
        d->actualEnabled = enabled;
        Q_EMIT enabledChanged(enabled);
    }
}

void MKeyOverrideQuick::useDefaultLabel()
{
    Q_D(MKeyOverrideQuick);

    setLabel(d->defaultLabel, false);
}

void MKeyOverrideQuick::useDefaultIcon()
{
    Q_D(MKeyOverrideQuick);

    setIcon(d->defaultIcon, false);
}

void MKeyOverrideQuick::useDefaultHighlighted()
{
    Q_D(MKeyOverrideQuick);

    setHighlighted(d->defaultHighlighted, false);
}

void MKeyOverrideQuick::useDefaultEnabled()
{
    Q_D(MKeyOverrideQuick);

    setEnabled(d->defaultEnabled, false);
}
