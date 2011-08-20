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

#ifndef INPUTMETHOD_P_H
#define INPUTMETHOD_P_H

#include "inputmethod.h"

namespace Maliit {

class InputMethodPrivate
{
public:
    InputMethodPrivate();
    ~InputMethodPrivate();

    //! Current input method area
    QRect area;

    //! Current orientation angle of application's active window
    OrientationAngle orientationAngle;

    //! Indicate during rotation changing.
    bool rotationInProgress;

    //! Current language of active input method
    QString language;

private:
    Q_DISABLE_COPY(InputMethodPrivate)
};

}

#endif // INPUTMETHOD_P_H
