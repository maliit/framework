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


#ifndef MATTRIBUTEEXTENSION_P_H
#define MATTRIBUTEEXTENSION_P_H

#include <QSharedPointer>
#include "mattributeextensionid.h"
class MKeyOverrideData;

class MAttributeExtensionPrivate {
public:
    Q_DECLARE_PUBLIC(MAttributeExtension)
    MAttributeExtensionPrivate();

private:
    MAttributeExtensionId id;
    QSharedPointer<MKeyOverrideData> keyOverrideData;

    MAttributeExtension *q_ptr;
};

#endif

