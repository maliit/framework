/* * This file is part of maliit-framework *
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
#include "inputmethod.h"

#include <QDebug>

namespace Maliit {

AttributeExtensionPrivate::AttributeExtensionPrivate(const QString &fileName) :
    id(createId()),
    fileName(fileName),
    values(),
    registry(AttributeExtensionRegistry::instance())
{
}

AttributeExtensionPrivate::AttributeExtensionPrivate(int id) :
    id(id),
    fileName(),
    values(),
    registry(AttributeExtensionRegistry::instance())
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
    Q_D(AttributeExtension);

    if (AttributeExtensionRegistry *r = d->registry.data()) {
        r->addExtension(this);
    }
}

AttributeExtension::AttributeExtension(int id, bool registerExtension)
    : QObject(),
      d_ptr(new AttributeExtensionPrivate(id))
{
    Q_D(AttributeExtension);

    AttributeExtensionRegistry *r = d->registry.data();

    if (r && registerExtension) {
        r->addExtension(this);
    }
}

QSharedPointer<AttributeExtension> AttributeExtension::create(int id)
{
    QSharedPointer<AttributeExtension> res(new AttributeExtension(id, false));

    AttributeExtensionRegistry::instance()->addExtension(res);

    return res;
}

AttributeExtension::~AttributeExtension()
{
    Q_D(AttributeExtension);

    if (AttributeExtensionRegistry *r = d->registry.data()) {
        r->removeExtension(this);
    }
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

QString AttributeExtension::key(const QString &target,
                                const QString &targetItem,
                                const QString &attribute)
{
    return QString::fromLatin1("%1/%2/%3").arg(target, targetItem, attribute);
}

void AttributeExtension::updateAttribute(const QString &key,
                                         const QVariant &value)
{
    Q_D(AttributeExtension);

    d->values[key] = value;

    Q_EMIT extendedAttributeChanged(key, value);
}

void AttributeExtension::setAttribute(const QString &key, const QVariant &value)
{
    Q_D(AttributeExtension);

    if (!d->values.contains(key) ||
        d->values.value(key) != value)
    {
        d->values.insert(key, value);
        if (AttributeExtensionRegistry *r = d->registry.data()) {
            r->extensionChanged(this, key, value);
        }
    }
}

} // namespace Maliit
