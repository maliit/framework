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

// Based on mpreeditinjectionevent_p.h from libmeegotouch

#ifndef MALIIT_PREEDITINJECTIONEVENT_P_H
#define MALIIT_PREEDITINJECTIONEVENT_P_H

namespace Maliit {

class PreeditInjectionEventPrivate
{
public:
    PreeditInjectionEventPrivate(const QString &preedit, int eventCursorPos);
    virtual ~PreeditInjectionEventPrivate();

    QString preedit;
    int eventCursorPosition;
    int replacementStart;
    int replacementLength;
};

}

#endif
