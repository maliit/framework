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

// Based on minputmethodstate_p.h from libmeegotouch


#ifndef MINPUTMETHODSTATE_P_H
#define MINPUTMETHODSTATE_P_H

#include <QObject>
#include <QMap>
#include <QVariant>

#include "minputmethodstate.h"
#include "minputmethodnamespace.h"

class AttributeExtensionInfo {
public:
    AttributeExtensionInfo(const QString &attributeExtensionFile)
        : fileName(attributeExtensionFile)
    {
    }

    QString fileName;
    MInputMethodState::ExtendedAttributeMap extendedAttributes;
};


class MInputMethodStatePrivate
{
    Q_DECLARE_PUBLIC(MInputMethodState)
public:
    MInputMethodStatePrivate();
    ~MInputMethodStatePrivate();

    void init();

    //! Current input method area
    QRect region;

    //! Current orientation angle of application's active window
    MInputMethod::OrientationAngle orientationAngle;

    //! Indicate during rotation changing.
    bool rotationInProgress;

    QMap<int, AttributeExtensionInfo*> attributeExtensions;

private:
    Q_DISABLE_COPY(MInputMethodStatePrivate)

    MInputMethodState *q_ptr;
};

#endif
