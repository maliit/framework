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


#ifndef MIMSCENE_H

#include <MScene>

/*!
 * \internal
 * \brief MImScene is custom scene class for input method server.
 *
 * This class implements workaround for NB#216904 and could be removed
 * when that bug will be fixed properly.
 */
class MImScene : public MScene
{
    Q_OBJECT

public:
    MImScene(QObject *parent = 0);

protected:
    //! \reimp
    bool event(QEvent *event);
    //! \reimp_end
};
//! \internal_end

#endif

