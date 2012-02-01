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
    friend struct MImSettingsGConfBackendPrivate;
    struct MImSettingsGConfBackendPrivate *priv;

    void update_value(bool emit_signal);
};

#endif // MIMSETTINGSGCONF_H
