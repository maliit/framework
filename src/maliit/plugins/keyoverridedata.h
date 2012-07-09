/* * This file is part of Maliit framework *
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef MKEYOVERRIDEDATA_H
#define MKEYOVERRIDEDATA_H

#include <QObject>
#include <QList>
#include <QMap>
#include <QSharedPointer>

#include <maliit/plugins/keyoverride.h>


/*! \ingroup maliitserver
 * \brief Corresponds to the key overrides.
 */
class MKeyOverrideData : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(MKeyOverrideData)

public:
    /*!
    * \brief Constructor
    */
    MKeyOverrideData();

    /*!
    * \brief Destructor
    */
    ~MKeyOverrideData();

    /*
     * \brief Return all key overrides in this key override data.
     * The returned list is sorted by key Id.
     */
    QList<QSharedPointer<MKeyOverride> > keyOverrides() const;

    //! Returns true if a new key override is created.
    bool createKeyOverride(const QString &keyId);

    //! Returns pointer to the key override for given \a keyId
    QSharedPointer<MKeyOverride> keyOverride(const QString &keyId) const;

protected:

    typedef QMap<QString, QSharedPointer<MKeyOverride> > KeyOverrides;
    KeyOverrides mKeyOverrides;

    friend class Ut_MKeyOverrideData;
};

#endif

