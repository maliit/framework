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

#include <QDebug>
#include <QtAlgorithms>

#include "keyoverridequick.h"
#include "keyoverridequick_p.h"

namespace Maliit
{

KeyOverrideQuickPrivate::KeyOverrideQuickPrivate(const QString &label,
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
{}

KeyOverrideQuick::KeyOverrideQuick()
    : QObject(),
      d_ptr(new KeyOverrideQuickPrivate("",
                                        "",
                                        false,
                                        true))
{}

KeyOverrideQuick::~KeyOverrideQuick()
{}

QString KeyOverrideQuick::label() const
{
    Q_D(const KeyOverrideQuick);

    return d->actualLabel;
}


QString KeyOverrideQuick::icon() const
{
    Q_D(const KeyOverrideQuick);

    return d->actualIcon;
}

bool KeyOverrideQuick::highlighted() const
{
    Q_D(const KeyOverrideQuick);

    return d->actualHighlighted;
}

bool KeyOverrideQuick::enabled() const
{
    Q_D(const KeyOverrideQuick);

    return d->actualEnabled;
}

QString KeyOverrideQuick::defaultLabel() const
{
    Q_D(const KeyOverrideQuick);

    return d->defaultLabel;
}


QString KeyOverrideQuick::defaultIcon() const
{
    Q_D(const KeyOverrideQuick);

    return d->defaultIcon;
}

bool KeyOverrideQuick::defaultHighlighted() const
{
    Q_D(const KeyOverrideQuick);

    return d->defaultHighlighted;
}

bool KeyOverrideQuick::defaultEnabled() const
{
    Q_D(const KeyOverrideQuick);

    return d->defaultEnabled;
}

void KeyOverrideQuick::overrideLabel(const QString &label)
{
    setLabel(label, true);
}

void KeyOverrideQuick::overrideIcon(const QString &icon)
{
    setIcon(icon, true);
}

void KeyOverrideQuick::overrideHighlighted(bool highlighted)
{
    setHighlighted(highlighted, true);
}

void KeyOverrideQuick::overrideEnabled(bool enabled)
{
    setEnabled(enabled, true);
}

void KeyOverrideQuick::setDefaultLabel(const QString &label)
{
    Q_D(KeyOverrideQuick);

    if (d->defaultLabel != label) {
        d->defaultLabel = label;
        Q_EMIT defaultLabelChanged(label);
    }
    if (!d->labelIsOverriden) {
        useDefaultLabel();
    }
}

void KeyOverrideQuick::setDefaultIcon(const QString &icon)
{
    Q_D(KeyOverrideQuick);

    if (d->defaultIcon != icon) {
        d->defaultIcon = icon;
        Q_EMIT defaultIconChanged(icon);
    }
    if (!d->iconIsOverriden) {
        useDefaultIcon();
    }
}

void KeyOverrideQuick::setDefaultHighlighted(bool highlighted)
{
    Q_D(KeyOverrideQuick);

    if (d->defaultHighlighted != highlighted) {
        d->defaultHighlighted = highlighted;
        Q_EMIT defaultHighlightedChanged(highlighted);
    }
    if (!d->highlightedIsOverriden) {
        useDefaultHighlighted();
    }
}

void KeyOverrideQuick::setDefaultEnabled(bool enabled)
{
    Q_D(KeyOverrideQuick);

    if (d->defaultEnabled != enabled) {
        d->defaultEnabled = enabled;
        Q_EMIT defaultEnabledChanged(enabled);
    }
    if (!d->enabledIsOverriden) {
        useDefaultEnabled();
    }
}

void KeyOverrideQuick::setLabel(const QString &label, bool overriden)
{
    Q_D(KeyOverrideQuick);

    d->labelIsOverriden = overriden;
    if (d->actualLabel != label) {
        d->actualLabel = label;
        Q_EMIT labelChanged(label);
    }
}

void KeyOverrideQuick::setIcon(const QString &icon, bool overriden)
{
    Q_D(KeyOverrideQuick);

    d->iconIsOverriden = overriden;
    if (d->actualIcon != icon) {
        d->actualIcon = icon;
        Q_EMIT iconChanged(icon);
    }
}

void KeyOverrideQuick::setHighlighted(bool highlighted, bool overriden)
{
    Q_D(KeyOverrideQuick);

    d->highlightedIsOverriden = overriden;
    if (d->actualHighlighted != highlighted) {
        d->actualHighlighted = highlighted;
        Q_EMIT highlightedChanged(highlighted);
    }
}

void KeyOverrideQuick::setEnabled(bool enabled, bool overriden)
{
    Q_D(KeyOverrideQuick);

    d->enabledIsOverriden = overriden;
    if (d->actualEnabled != enabled) {
        d->actualEnabled = enabled;
        Q_EMIT enabledChanged(enabled);
    }
}

void KeyOverrideQuick::useDefaultLabel()
{
    Q_D(KeyOverrideQuick);

    setLabel(d->defaultLabel, false);
}

void KeyOverrideQuick::useDefaultIcon()
{
    Q_D(KeyOverrideQuick);

    setIcon(d->defaultIcon, false);
}

void KeyOverrideQuick::useDefaultHighlighted()
{
    Q_D(KeyOverrideQuick);

    setHighlighted(d->defaultHighlighted, false);
}

void KeyOverrideQuick::useDefaultEnabled()
{
    Q_D(KeyOverrideQuick);

    setEnabled(d->defaultEnabled, false);
}

void KeyOverrideQuick::applyOverride(const QSharedPointer<MKeyOverride>& keyOverride,
                                     const MKeyOverride::KeyOverrideAttributes changedAttributes)
{
    Q_D(KeyOverrideQuick);

    enum {
        UseOverride,
        UseDefault,
        UseEmpty,
        UseActual
    } iconAction(UseEmpty), labelAction(UseEmpty);

    if (keyOverride)
    {
        // label and icon are mutually exclusive.
        // icons are preferred over labels.
        // overrides are preferred over defaults.
        // default value for either icon or label have to be set.
        // otherwise, it is a programming error.

        if (!keyOverride->icon().isEmpty()) {
            iconAction = UseOverride;
        } else if (!keyOverride->label().isEmpty()) {
            labelAction = UseOverride;
        } else if (!d->defaultIcon.isEmpty()) {
            iconAction = UseDefault;
        } else if (!d->defaultLabel.isEmpty()) {
            labelAction = UseDefault;
        } else {
            qCritical() << __PRETTY_FUNCTION__ << "- Both label and icon have no default value.";
        }

        if (changedAttributes & MKeyOverride::Highlighted) {
            overrideHighlighted(keyOverride->highlighted());
        }

        if (changedAttributes & MKeyOverride::Enabled) {
            overrideEnabled(keyOverride->enabled());
        }
    } else {
        // if no key override is passed then we just set defaults.
        // this case should happen only at the beginning or when we change focus
        // to non-input widget from input widget with an attribute extension
        // attached to it. this of course means that if both default label and
        // default icon are empty then the key will be empty. but we allow this,
        // because such situation could happen at the beginning. obviously we
        // expect a plugin to actually set a default value before it is shown.
        if (!d->defaultIcon.isEmpty()) {
            iconAction = UseDefault;
        } else {
            labelAction = UseDefault;
        }

        if (changedAttributes & MKeyOverride::Highlighted) {
            useDefaultHighlighted();
        }

        if (changedAttributes & MKeyOverride::Enabled) {
            useDefaultEnabled();
        }
    }

    switch (iconAction) {
    case UseOverride:
        overrideIcon(keyOverride->icon());
        break;
    case UseDefault:
        useDefaultIcon();
        break;
    case UseEmpty:
        overrideIcon(QString());
        break;
    case UseActual:
        break;
    default:
        qCritical() << __PRETTY_FUNCTION__ << "- unknown enum value for iconAction:" << static_cast<int> (iconAction);
    }

    switch (labelAction) {
    case UseOverride:
        overrideLabel(keyOverride->label());
        break;
    case UseDefault:
        useDefaultLabel();
        break;
    case UseEmpty:
        overrideLabel(QString());
        break;
    case UseActual:
        break;
    default:
        qCritical() << __PRETTY_FUNCTION__ << "- unknown enum value for labelAction:" << static_cast<int> (labelAction);
    }
}

} // namespace Maliit
