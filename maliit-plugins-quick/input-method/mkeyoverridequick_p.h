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

#ifndef MALIIT_QUICK_KEY_OVERRIDE_P_H
#define MALIIT_QUICK_KEY_OVERRIDE_P_H

namespace Maliit
{

class KeyOverrideQuickPrivate
{
    Q_DISABLE_COPY(KeyOverrideQuickPrivate)

public:
    KeyOverrideQuickPrivate(const QString &label,
                            const QString &icon,
                            bool highlighted,
                            bool enabled);

    QString actualLabel;
    QString actualIcon;
    bool actualHighlighted;
    bool actualEnabled;

    QString defaultLabel;
    QString defaultIcon;
    bool defaultHighlighted;
    bool defaultEnabled;

    bool labelIsOverriden;
    bool iconIsOverriden;
    bool highlightedIsOverriden;
    bool enabledIsOverriden;
};

} // namespace Maliit

#endif // MALIIT_QUICK_KEY_OVERRIDE_P_H
