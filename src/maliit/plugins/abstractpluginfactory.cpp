/* * This file is part of Maliit framework *
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <maliit/plugins/abstractpluginfactory.h>
#include "abstractplatform.h"

class MImAbstractPluginFactoryPrivate
{
public:
    QSharedPointer<Maliit::AbstractPlatform> m_platform;
};

MImAbstractPluginFactory::MImAbstractPluginFactory()
    : d_ptr(new MImAbstractPluginFactoryPrivate)
{}

MImAbstractPluginFactory::~MImAbstractPluginFactory()
{}

void MImAbstractPluginFactory::setPlatform(const QSharedPointer<Maliit::AbstractPlatform> &platform)
{
    Q_D(MImAbstractPluginFactory);

    d->m_platform = platform;
}

QSharedPointer<Maliit::AbstractPlatform> MImAbstractPluginFactory::getPlatform() const
{
    Q_D(const MImAbstractPluginFactory);

    return d->m_platform;
}
