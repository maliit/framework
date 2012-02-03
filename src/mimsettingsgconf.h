/* * This file is part of meego-im-framework *
 *
 * Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef MIMSETTINGSGCONF_H
#define MIMSETTINGSGCONF_H

#include "mimsettings.h"


//! \internal

/*!
  \ingroup maliitserver
  \brief MImSettingsGConfBackend stores configuration values using GConf

  The conversion between Qvariant and GConf values are performed
  according to the following rules:

  - A QVariant of type QVariant::Invalid denotes an unset GConf key.

  - QVariant::Int, QVariant::Double, QVariant::Bool are converted to
    and from the obvious equivalents.

  - QVariant::String is converted to/from a GConf string and always
    uses the UTF-8 encoding.  No other encoding is supported.

  - QVariant::StringList is converted to a list of UTF-8 strings.

  - QVariant::List (which denotes a QList<QVariant>) is converted
    to/from a GConf list.  All elements of such a list must have the
    same type, and that type must be one of QVariant::Int,
    QVariant::Double, QVariant::Bool, or QVariant::String.  (A list of
    strings is returned as a QVariant::StringList, however, when you
    get it back.)

  - Any other QVariant or GConf value is essentially ignored.

  \warning MImSettingsGConfBackend is as thread-safe as GConf
*/

struct MImSettingsGConfBackendPrivate;

class MImSettingsGConfBackend : public MImSettingsBackend
{
    Q_OBJECT

public:
    MImSettingsGConfBackend(const QString &key, QObject *parent);
    virtual ~MImSettingsGConfBackend();

    virtual QString key() const;
    virtual QVariant value(const QVariant &def) const;
    virtual void set(const QVariant &val);
    virtual void unset();
    virtual QList<QString> listDirs() const;
    virtual QList<QString> listEntries() const;

private:
    QScopedPointer<MImSettingsGConfBackendPrivate> d_ptr;

    Q_DISABLE_COPY(MImSettingsGConfBackend)
    Q_DECLARE_PRIVATE(MImSettingsGConfBackend)

    void update_value(bool emit_signal);
};

#endif // MIMSETTINGSGCONF_H
