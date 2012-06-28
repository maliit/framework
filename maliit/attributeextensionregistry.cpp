/* * This file is part of Maliit framework *
 *
 * Copyright (C) 2010, 2011 Nokia Corporation and/or its subsidiary(-ies).
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

// Based on minputmethodstate.cpp from libmeegotouch

#include "attributeextensionregistry.h"
#include "attributeextensionregistry_p.h"
#include "connectionfactory.h"
#include <QDebug>
#include <algorithm>

namespace Maliit {

namespace {

struct IdPredicate
{
    explicit IdPredicate(int i)
        : id(i)
    {
    }

    bool operator()(const QWeakPointer<AttributeExtension> &extension)
    {
        AttributeExtension * ptr = extension.data();
        return (ptr ? id == ptr->id() : false);
    }

    int id;
};

}

AttributeExtensionRegistryPrivate::AttributeExtensionRegistryPrivate() :
    extensions(),
    connection(Maliit::createServerConnection(qgetenv("QT_IM_MODULE")))
{
    if (not connection) {
        qCritical() << "Could not establish a connection (" << qgetenv("QT_IM_MODULE") << ")to server.";
    }
}

AttributeExtensionRegistryPrivate::~AttributeExtensionRegistryPrivate()
{
}

AttributeExtension * AttributeExtensionRegistryPrivate::find(int id)
{
    AttributeExtension * result = 0;
    ExtensionList::iterator iterator = std::find_if(extensions.begin(),
                                                    extensions.end(),
                                                    IdPredicate(id));

    if (iterator != extensions.end()) {
        result = iterator->data();
    }

    return result;
}

AttributeExtensionRegistry::AttributeExtensionRegistry() :
    QObject(),
    d_ptr(new AttributeExtensionRegistryPrivate)
{
    Q_D(AttributeExtensionRegistry);

    if (d->connection) {
        connect(d->connection.data(),
                SIGNAL(extendedAttributeChanged(int,QString,QString,QString,QVariant)),
                this,
                SLOT(updateAttribute(int,QString,QString,QString,QVariant)));
	connect(d->connection.data(), SIGNAL(connected()),
		this,                 SLOT(registerExistingAttributeExtensions()));
    }
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
    const int id(extension->id());
    const QString filename(extension->fileName());

    d->extensions.push_back(QWeakPointer<AttributeExtension>(extension));
    if (d->connection) {
        d->connection->registerAttributeExtension(id, filename);
    }
    Q_EMIT extensionRegistered(id, filename);
}

void AttributeExtensionRegistry::addExtension(const QSharedPointer<AttributeExtension> &extension)
{
    Q_D(AttributeExtensionRegistry);
    const int id(extension->id());
    const QString filename(extension->fileName());

    d->extensions.push_back(QWeakPointer<AttributeExtension>(extension));
    if (d->connection) {
        d->connection->registerAttributeExtension(id, filename);
    }
    Q_EMIT extensionRegistered(id, filename);
}

void AttributeExtensionRegistry::removeExtension(AttributeExtension *extension)
{
    Q_D(AttributeExtensionRegistry);
    const int id(extension->id());

    d->extensions.removeAll(QWeakPointer<AttributeExtension>(extension));
    if (d->connection) {
        d->connection->unregisterAttributeExtension(id);
    }
    Q_EMIT extensionUnregistered(id);
}

void AttributeExtensionRegistry::extensionChanged(AttributeExtension *extension, const QString &key, const QVariant &value)
{
    Q_D(AttributeExtensionRegistry);
    const int id(extension->id());

    if (d->connection) {
        const QString &target = QString::fromLatin1("/") + key.section('/', 1, 1);
        const QString &target_item = key.section('/', 2, -2);
        const QString &attribute = key.section('/', -1, -1);

        d->connection->setExtendedAttribute(id,
                                            target,
                                            target_item,
                                            attribute,
                                            value);
    }
    Q_EMIT extensionChanged(id, key, value);
}

ExtensionList AttributeExtensionRegistry::extensions() const
{
    Q_D(const AttributeExtensionRegistry);

    return d->extensions;
}

void AttributeExtensionRegistry::updateAttribute(int id,
                                                 const QString &target,
                                                 const QString &targetItem,
                                                 const QString &attribute,
                                                 const QVariant &value)
{
    Q_D(AttributeExtensionRegistry);

    AttributeExtension *extension = d->find(id);

    if (!extension) {
        qWarning() << __PRETTY_FUNCTION__
                   << "extension" << id << "was not found";
    }

    extension->updateAttribute(AttributeExtension::key(target, targetItem, attribute),
                               value);
}

void AttributeExtensionRegistry::registerExistingAttributeExtensions()
{
    Q_D(AttributeExtensionRegistry);

    if (d->connection) {
        Q_FOREACH (const QWeakPointer<AttributeExtension> &extension_ref, d->extensions) {
            AttributeExtension *extension = extension_ref.data();
	    if (!extension)
	        continue;

	    d->connection->registerAttributeExtension(extension->id(), extension->fileName());

	    const AttributeExtension::ExtendedAttributeMap &attributes = extension->attributes();

	    for (AttributeExtension::ExtendedAttributeMap::const_iterator iter(attributes.begin()), attributes_end(attributes.end());
		 iter != attributes_end;
		 ++iter) {
	        extensionChanged(extension->id(), iter.key(), iter.value());
	    }
	}
    }
}

} // namespace Maliit
