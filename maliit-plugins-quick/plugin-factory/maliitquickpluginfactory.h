/* * This file is part of Maliit framework *
 *
 * Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef MALIITQUICKPLUGINFACTORY_H
#define MALIITQUICKPLUGINFACTORY_H

#include <maliit/plugins/abstractpluginfactory.h>

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
    virtual Maliit::Plugins::InputMethodPlugin * create(const QString &file) const;
    //! \reimp_end
};

#endif // MALIITQUICKPLUGINFACTORY_H
