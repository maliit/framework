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


#ifndef MTOOLBARITEMFILTER_H
#define MTOOLBARITEMFILTER_H

#include "mattributeextensionid.h"

#include <QString>
#include <QVariant>
#include <QMap>

class MToolbarItem;

/*! \internal
 * \ingroup maliitserver
 * \brief This class has the same properties as MToolbarItem
 * and allow us to be sure that any QVariant will be handled exactly in
 * the same way as if you will call setProperty() for MToolbarItem.
 *
 */
struct MToolbarItemFilter
{
public:
    //! Constructor.
    //! \param newId Identifier of attribute extension
    explicit MToolbarItemFilter(const MAttributeExtensionId &newId);

    //! Return value associated with given \a name.
    QVariant property(const QString &name) const;

    //! Associate \a value with given \a name.
    void setProperty(const QString &name, const QVariant &value);

    //! Return identifier of attribute extension
    const MAttributeExtensionId & extensionId() const;

private:

    QMap<QString, QVariant> properties;
    MAttributeExtensionId m_extensionId;
};

//! \internal_end

#endif // MTOOLBARITEMFILTER_H

