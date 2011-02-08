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


#include "mattributeextension.h"
#include "mattributeextension_p.h"
#include "mkeyoverridedata.h"
#include "mtoolbardata.h"

#include <QDebug>

namespace
{
}

MAttributeExtensionPrivate::MAttributeExtensionPrivate()
{
}

MAttributeExtension::MAttributeExtension(const MAttributeExtensionId &id, const QString &fileName)
    : d_ptr(new MAttributeExtensionPrivate())
{
    Q_D(MAttributeExtension);
    d->id = id;
    d->toolbarData = createToolbar(fileName);
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

QSharedPointer<MToolbarData> MAttributeExtension::createToolbar(const QString &name)
{
    // load a toolbar
    QSharedPointer<MToolbarData> toolbar(new MToolbarData);
    const bool loaded = toolbar->loadToolbarXml(name);

    if (!loaded) {
        qWarning() << "ToolbarsManager: toolbar load error: "
                   << name;
        toolbar.clear();
    }

    return toolbar;
}

QSharedPointer<MToolbarData> MAttributeExtension::toolbarData() const
{
    Q_D(const MAttributeExtension);
    return d->toolbarData;
}

QSharedPointer<MKeyOverrideData> MAttributeExtension::keyOverrideData() const
{
    Q_D(const MAttributeExtension);
    return d->keyOverrideData;
}
