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
#include "mtoolbaritemfilter.h"

MToolbarItemFilter::MToolbarItemFilter(const MAttributeExtensionId &newId)
    : m_extensionId(newId)
{
}

QVariant MToolbarItemFilter::property(const QString &name) const
{
    QMap<QString, QVariant>::const_iterator iterator(properties.find(name));
    QVariant result;

    if (iterator != properties.constEnd()) {
        result = *iterator;
    }

    return result;
}

void MToolbarItemFilter::setProperty(const QString &name, const QVariant &value)
{
    if (value.isValid()) {
        properties[name] = value;
    } else {
        properties.remove(name);
    }
}

const MAttributeExtensionId & MToolbarItemFilter::extensionId() const
{
    return m_extensionId;
}

