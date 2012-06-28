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

// Based on minputmethodstate.h from libmeegotouch

#ifndef MALIIT_ATTRIBUTEEXTENSIONREGISTRY_H
#define MALIIT_ATTRIBUTEEXTENSIONREGISTRY_H

#include <QList>
#include <QObject>
#include <QScopedPointer>
#include <QWeakPointer>
#include "attributeextension.h"

namespace Maliit {

class AttributeExtensionRegistryPrivate;

typedef QList<QWeakPointer<AttributeExtension> > ExtensionList;

//! \ingroup libmaliit
//! \internal
class AttributeExtensionRegistry : public QObject
{
    Q_OBJECT
public:
    //! \brief Get singleton instance
    //! \return singleton instance
    static AttributeExtensionRegistry *instance();

    void addExtension(AttributeExtension *extension);
    void addExtension(const QSharedPointer<AttributeExtension> &extension);

    void removeExtension(AttributeExtension *extension);

    void extensionChanged(AttributeExtension *extension, const QString &key, const QVariant &value);

    ExtensionList extensions() const;

Q_SIGNALS:
    //! Emitted when an input method attribute extension which is defined in \a fileName with an unique identifier \a id is registered.
    void extensionRegistered(int id, const QString &fileName);

    //! Emitted when an input method attribute extension with an unique \a id is unregistered.
    void extensionUnregistered(int id);

    //! Emitted when input method extended attribute is changed.
    void extensionChanged(int id, const QString &key, const QVariant &value);

public Q_SLOTS:
    /*!
     * \brief Update cached \a value of extended attribute corresponding to given parameters.
     */
    void updateAttribute(int id,
                         const QString &target,
                         const QString &targetItem,
                         const QString &attribute,
                         const QVariant &value);

private:
    AttributeExtensionRegistry();
    ~AttributeExtensionRegistry();
    Q_DISABLE_COPY(AttributeExtensionRegistry)

    const QScopedPointer<AttributeExtensionRegistryPrivate> d_ptr;

    Q_DECLARE_PRIVATE(AttributeExtensionRegistry)

    Q_SLOT void registerExistingAttributeExtensions();
};

} // namespace Maliit

#endif // MALIIT_ATTRIBUTEEXTENSIONREGISTRY_H
