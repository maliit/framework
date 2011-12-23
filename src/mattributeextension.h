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



#ifndef MATTRIBUTEEXTENSION_H
#define MATTRIBUTEEXTENSION_H

#include <QObject>
#include <QList>
#include <QWeakPointer>

#include "minputmethodnamespace.h"


class MAttributeExtensionId;
class MAttributeExtensionPrivate;
class MToolbarData;
class MKeyOverrideData;
class MSubViewWatcher;

/*! \ingroup pluginapi
 * \brief Attribute extension.
 */
class MAttributeExtension : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(MAttributeExtension)

public:
    /*!
    * \brief Constructor
    */
    MAttributeExtension(const MAttributeExtensionId &id,  const QString &fileName);

    /*!
    * \brief Destructor
    */
    ~MAttributeExtension();

    MAttributeExtensionId id() const;

    //! Return the pointer to toolbar data.
    QSharedPointer<MToolbarData> toolbarData() const;

    //! Return the pointer to key override data.
    QSharedPointer<MKeyOverrideData> keyOverrideData() const;

    //! Remember object watching on layout changes
    void addSubViewWatcher(MSubViewWatcher *watcher);

    //! Destroy object watching on layout changes
    void destroySubViewWatcher();

    //! Return object watching on layout changes
    MSubViewWatcher * subViewWatcher() const;

private:
    /*!
     * \brief Returns new toolbar which is loaded from specified file \a name.
     */
    QSharedPointer<MToolbarData> createToolbar(const QString &name);

protected:
    Q_DECLARE_PRIVATE(MAttributeExtension)
    MAttributeExtensionPrivate *const d_ptr;

    friend class MAttributeExtensionManager;
    friend class Ut_MAttributeExtension;
    friend class Ut_MAttributeExtensionManager;
};

#endif

