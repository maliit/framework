/* * This file is part of Maliit framework *
 *
 * Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#include "mimdummyinputcontext.h"

MIMDummyInputContext::MIMDummyInputContext(QObject *parent) :
    QInputContext(parent)
{
}

QString MIMDummyInputContext::identifierName()
{
    return QString("MIMDummyInputContext");
}

bool MIMDummyInputContext::isComposing() const
{
    return false;
}

QString MIMDummyInputContext::language()
{
    return QString();
}

void MIMDummyInputContext::reset()
{
}
