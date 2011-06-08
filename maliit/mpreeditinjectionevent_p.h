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

// Based on mpreeditinjectionevent_p.h from libmeegotouch

#ifndef MPREEDITINJECTIONEVENT_P_H
#define MPREEDITINJECTIONEVENT_P_H

class MPreeditInjectionEventPrivate
{
public:
    MPreeditInjectionEventPrivate(const QString &preedit, int eventCursorPos);
    virtual ~MPreeditInjectionEventPrivate();

    QString preedit;
    int eventCursorPosition;
    int replacementStart;
    int replacementLength;
};

#endif
