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

#ifndef MIMDUMMYINPUTCONTEXT_H
#define MIMDUMMYINPUTCONTEXT_H

#include <QInputContext>

/*!
 * \internal
 *
 * \brief MIMDummyInputContext is a dummy QInputContext doing nothing
 */
class MIMDummyInputContext : public QInputContext
{
    Q_OBJECT
public:
    //! \reimp
    explicit MIMDummyInputContext(QObject *parent = 0);

    virtual QString identifierName();
    virtual bool isComposing() const;
    virtual QString language();
    virtual void reset();
    //! \endreimp
};
//! \endinternal

#endif // MIMDUMMYINPUTCONTEXT_H
