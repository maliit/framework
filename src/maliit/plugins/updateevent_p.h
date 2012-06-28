/* * This file is part of maliit-framework *
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

#ifndef MIMUPDATEEVENT_P_H
#define MIMUPDATEEVENT_P_H

#include <maliit/plugins/extensionevent_p.h>

#include <QtCore>

class MImUpdateEventPrivate
    : public MImExtensionEventPrivate
{
public:
    QMap<QString, QVariant> update;
    QStringList changedProperties;
    Qt::InputMethodHints lastHints;

    explicit MImUpdateEventPrivate();

    explicit MImUpdateEventPrivate(const QMap<QString, QVariant> &newUpdate,
                                   const QStringList &newChangedProperties,
                                   const Qt::InputMethodHints &newLastHints);

    bool isFlagSet(Qt::InputMethodHint hint,
                   bool *changed = 0) const;

    QVariant extractProperty(const QString &key,
                             bool *changed = 0) const;
};

#endif // MIMUPDATEEVENT_P_H
