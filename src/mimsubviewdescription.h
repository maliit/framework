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

#ifndef MIMSUBVIEWDESCRIPTION_H
#define MIMSUBVIEWDESCRIPTION_H

#include <QString>

class MImSubViewDescriptionPrivate;

class MImSubViewDescription
{
public:
    //! Copy constructor.
    MImSubViewDescription(const MImSubViewDescription &other);

    //! Destructor.
    virtual ~MImSubViewDescription();

    //! Assignment operator
    void operator=(const MImSubViewDescription &other);

    //! Returns true if \a left and \a right describe the same subview.
    friend bool operator==(const MImSubViewDescription &left, const MImSubViewDescription &right);

    //! Return plugin id.
    QString pluginId() const;

    //! Return subview id
    QString id() const;

    //! Return subview title
    QString title() const;

private:
    //! Constructor
    //! \param plugin Reference to loaded plugin.
    explicit MImSubViewDescription(const QString &pluginId,
                                   const QString &subViewId,
                                   const QString &subViewTitle);

    Q_DECLARE_PRIVATE(MImSubViewDescription)

    MImSubViewDescriptionPrivate * const d_ptr;

    friend class MIMPluginManagerPrivate;
};

#endif // MIMSUBVIEWDESCRIPTION_H
