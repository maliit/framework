/* * This file is part of meego-im-framework *
 *
 * Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef MIMUPDATEEVENT_H
#define MIMUPDATEEVENT_H

#include "mimextensionevent.h"

#include <QtCore>

class MImUpdateEventPrivate;

class MImUpdateEvent
    : public MImExtensionEvent
{
public:
    explicit MImUpdateEvent(const QMap<QString, QVariant> &update,
                            const QStringList &propertiesChanged);

    //! Returns invalid QVariant if key is invalid.
    QVariant value(const QString &key) const;

    //! Returns list of keys that have changed, compared to last update event.
    QStringList propertiesChanged() const;

protected:
    MImUpdateEventPrivate * const d_ptr;

private:
    Q_DISABLE_COPY(MImUpdateEvent)
    Q_DECLARE_PRIVATE(MImUpdateEvent)
};

#endif // MIMUPDATEEVENT_H