/* * This file is part of maliit-framework *
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

#ifndef MIMSUBVIEWOVERRIDE_H
#define MIMSUBVIEWOVERRIDE_H

#include <QObject>
#include <QWeakPointer>

class MImOnScreenPlugins;

//! \internal
//! \ingroup maliitserver
//! \brief A handle class for the enable-all-subviews override which disables the override upon destruction.
class MImSubViewOverride
    : public QObject
{
    Q_OBJECT

public:
    explicit MImSubViewOverride(MImOnScreenPlugins *plugins,
                                QObject *parent = 0);
    virtual ~MImSubViewOverride();

private:
    const QWeakPointer<MImOnScreenPlugins> mPlugins;
};

#endif // MIMSUBVIEWOVERRIDE_H
