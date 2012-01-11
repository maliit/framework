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


#include <maliit/plugins/attributeextension.h>
#include <maliit/plugins/attributeextension_p.h>
#include <maliit/plugins/keyoverridedata.h>

#include <QDebug>

MAttributeExtensionPrivate::MAttributeExtensionPrivate()
    : id()
{
}

MAttributeExtension::MAttributeExtension(const MAttributeExtensionId &id, const QString &)
    : d_ptr(new MAttributeExtensionPrivate())
{
    Q_D(MAttributeExtension);
    d->id = id;
    d->keyOverrideData = QSharedPointer<MKeyOverrideData>(new MKeyOverrideData());
}

MAttributeExtension::~MAttributeExtension()
{
    delete d_ptr;
}


MAttributeExtensionId MAttributeExtension::id() const
{
    Q_D(const MAttributeExtension);
    return d->id;
}

QSharedPointer<MKeyOverrideData> MAttributeExtension::keyOverrideData() const
{
    Q_D(const MAttributeExtension);
    return d->keyOverrideData;
}
