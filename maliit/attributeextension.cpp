/* * This file is part of meego-im-framework *
 *
 * Copyright (C) 2010, 2011 Nokia Corporation and/or its subsidiary(-ies).
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

// Based on minputmethodstate.cpp from libmeegotouch

#include "attributeextension.h"
#include "attributeextension_p.h"

#include "attributeextensionregistry.h"

namespace Maliit {

AttributeExtensionPrivate::AttributeExtensionPrivate(const QString &fileName) :
    id(createId()),
    fileName(fileName),
    values()
{
}

AttributeExtensionPrivate::~AttributeExtensionPrivate()
{
}

int AttributeExtensionPrivate::createId()
{
    static int idCounter = 0;
    // generate an application local unique identifier for the extension.
    int result = idCounter;
    idCounter++;
    return result;
}

AttributeExtension::AttributeExtension(const QString &fileName)
    : QObject(),
      d_ptr(new AttributeExtensionPrivate(fileName))
{
    AttributeExtensionRegistry::instance()->addExtension(this);
}

AttributeExtension::~AttributeExtension()
{
    AttributeExtensionRegistry::instance()->removeExtension(this);
}

AttributeExtension::ExtendedAttributeMap AttributeExtension::attributes() const
{
    Q_D(const AttributeExtension);

    return d->values;
}

QString AttributeExtension::fileName() const
{
    Q_D(const AttributeExtension);

    return d->fileName;
}

int AttributeExtension::id() const
{
    Q_D(const AttributeExtension);

    return d->id;
}

void AttributeExtension::setAttribute(const QString &key, const QVariant &value)
{
    Q_D(AttributeExtension);

    if (!d->values.contains(key) ||
        d->values.value(key) != value)
    {
        d->values.insert(key, value);
        AttributeExtensionRegistry::instance()->extensionChanged(this, key, value);
    }
}

} // namespace Maliit
