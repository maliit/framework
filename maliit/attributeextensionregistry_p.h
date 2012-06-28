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

#ifndef ATTRIBUTEEXTENSIONREGISTRY_P_H
#define ATTRIBUTEEXTENSIONREGISTRY_P_H

#include "attributeextensionregistry.h"
#include "mimserverconnection.h"

namespace Maliit {

class AttributeExtensionRegistryPrivate {
public:
    AttributeExtensionRegistryPrivate();
    ~AttributeExtensionRegistryPrivate();

    ExtensionList extensions;
    QSharedPointer<MImServerConnection> connection;

    AttributeExtension * find(int id);
};

}

#endif // ATTRIBUTEEXTENSIONREGISTRY_P_H
