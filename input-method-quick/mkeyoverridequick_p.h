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

#ifndef MALIIT_QUICK_KEY_OVERRIDE_P_H
#define MALIIT_QUICK_KEY_OVERRIDE_P_H

class MKeyOverrideQuickPrivate {
public:
    Q_DISABLE_COPY(MKeyOverrideQuickPrivate)

    MKeyOverrideQuickPrivate(const QString &label,
                             const QString &icon,
                             bool highlighted,
                             bool enabled);

    void assign(const MKeyOverrideQuickPrivate &other);

    QString defaultLabel;
    QString defaultIcon;
    bool defaultHighlighted;
    bool defaultEnabled;
};

#endif // MALIIT_QUICK_KEY_OVERRIDE_P_H
