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

#include "attributeextensionregistry.h"
#include "attributeextensionregistry_p.h"

namespace Maliit {

AttributeExtensionRegistryPrivate::AttributeExtensionRegistryPrivate() :
    extensions()
{
}

AttributeExtensionRegistryPrivate::~AttributeExtensionRegistryPrivate()
{
}

AttributeExtensionRegistry::AttributeExtensionRegistry() :
    QObject(),
    d_ptr(new AttributeExtensionRegistryPrivate)
{
}

AttributeExtensionRegistry::~AttributeExtensionRegistry()
{
}

AttributeExtensionRegistry *AttributeExtensionRegistry::instance()
{
    static AttributeExtensionRegistry singleton;

    return &singleton;
}

void AttributeExtensionRegistry::addExtension(AttributeExtension *extension)
{
    Q_D(AttributeExtensionRegistry);

    d->extensions.push_back(QWeakPointer<AttributeExtension>(extension));
    emit extensionRegistered(extension->id(), extension->fileName());
}

void AttributeExtensionRegistry::removeExtension(AttributeExtension *extension)
{
    Q_D(AttributeExtensionRegistry);

    d->extensions.removeAll(QWeakPointer<AttributeExtension>(extension));
    emit extensionUnregistered(extension->id());
}

void AttributeExtensionRegistry::extensionChanged(AttributeExtension *extension, const QString &key, const QVariant &value)
{
    emit extensionChanged(extension->id(), key, value);
}

ExtensionList AttributeExtensionRegistry::extensions() const
{
    Q_D(const AttributeExtensionRegistry);

    return d->extensions;
}

} // namespace Maliit
