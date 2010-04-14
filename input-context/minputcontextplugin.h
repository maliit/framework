/* * This file is part of m-im-framework *
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

#ifndef MINPUTCONTEXTPLUGIN_H
#define MINPUTCONTEXTPLUGIN_H

#include <QInputContextPlugin>

class MInputContextPlugin: public QInputContextPlugin
{
    Q_OBJECT

public:
    MInputContextPlugin(QObject *parent = 0);
    virtual ~MInputContextPlugin();

    //! \reimp
    QInputContext *create(const QString &key);
    QString description(const QString &key);
    QString displayName(const QString &key);
    QStringList keys() const;
    QStringList languages(const QString &key);
    //! \reimp_end
};

#endif
