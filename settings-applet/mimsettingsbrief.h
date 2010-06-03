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

#ifndef MIMSETTINGSBRIEF_H
#define MIMSETTINGSBRIEF_H

#include <DcpBrief>

class MImSettingsBrief: public DcpBrief
{
    Q_OBJECT
public:
    MImSettingsBrief();
    //! reimp
    virtual QString titleText() const;
    //! reimp_end
};


#endif
