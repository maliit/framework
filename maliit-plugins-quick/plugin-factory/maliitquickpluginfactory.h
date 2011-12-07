/* * This file is part of meego-im-framework *
 *
 * Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 *
 * Contact: Kimmo Surakka <kimmo.surakka@nokia.com>
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

#ifndef MALIITQUICKPLUGINFACTORY_H
#define MALIITQUICKPLUGINFACTORY_H

#include "mimabstractpluginfactory.h"

#include <QtCore>

class MaliitQuickPluginFactory
    : public QObject, public MImAbstractPluginFactory
{
    Q_OBJECT
    Q_INTERFACES(MImAbstractPluginFactory)
    Q_DISABLE_COPY(MaliitQuickPluginFactory)

public:
    explicit MaliitQuickPluginFactory(QObject *parent = 0);

    //! \reimp
    virtual ~MaliitQuickPluginFactory();
    virtual QString fileExtension() const;
    virtual Maliit::Server::InputMethodPlugin * create(const QString &file) const;
    //! \reimp_end
};

#endif // MALIITQUICKPLUGINFACTORY_H
