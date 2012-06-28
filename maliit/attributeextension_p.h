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

// Based on minputmethodstate_p.h from libmeegotouch

#ifndef ATTRIBUTEEXTENSION_P_H
#define ATTRIBUTEEXTENSION_P_H

#include "attributeextension.h"
#include "attributeextensionregistry.h"

namespace Maliit {

class AttributeExtensionPrivate {
public:
    AttributeExtensionPrivate(const QString &fileName);
    AttributeExtensionPrivate(int id);
    ~AttributeExtensionPrivate();

    static int createId();

    //! id
    int id;

    //! fileName
    QString fileName;

    //! values
    AttributeExtension::ExtendedAttributeMap values;

    //! weak reference to registry
    QWeakPointer<AttributeExtensionRegistry> registry;
};

}

#endif // ATTRIBUTEEXTENSION_P_H
