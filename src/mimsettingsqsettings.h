/* * This file is part of Maliit framework *
 *
 * Copyright (C) 2012 Mattia Barbon <mattia@develer.com>
 * Copyright (C) 2012 Canonical Ltd
 *
 * Contact: maliit-discuss@lists.maliit.org
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 */

#ifndef MIMSETTINGSQSETTINGS_H
#define MIMSETTINGSQSETTINGS_H

#include "mimsettings.h"

#include <QScopedPointer>
#include <QSettings>
#include <QTemporaryFile>

//! \internal

struct MImSettingsQSettingsBackendPrivate;

class MImSettingsQSettingsBackend : public MImSettingsBackend
{
    Q_OBJECT

public:
    explicit MImSettingsQSettingsBackend(QSettings *settingsInstance, const QString &key, QObject *parent = 0);
    virtual ~MImSettingsQSettingsBackend();

    virtual QString key() const;
    virtual QVariant value(const QVariant &def) const;
    virtual void set(const QVariant &val);
    virtual void unset();
    virtual QList<QString> listDirs() const;
    virtual QList<QString> listEntries() const;

private:
    QScopedPointer<MImSettingsQSettingsBackendPrivate> d_ptr;

    Q_DISABLE_COPY(MImSettingsQSettingsBackend)
    Q_DECLARE_PRIVATE(MImSettingsQSettingsBackend)
};


//! \internal

class MImSettingsQSettingsBackendFactory : public MImSettingsBackendFactory
{
public:
    explicit MImSettingsQSettingsBackendFactory();
    explicit MImSettingsQSettingsBackendFactory(const QString &organization,
                                                const QString &application);
    virtual ~MImSettingsQSettingsBackendFactory();
    virtual MImSettingsBackend *create(const QString &key, QObject *parent);

private:
    QSettings mSettings;
};

class MImSettingsQSettingsTemporaryBackendFactory : public MImSettingsBackendFactory
{
public:
    explicit MImSettingsQSettingsTemporaryBackendFactory();
    virtual ~MImSettingsQSettingsTemporaryBackendFactory();
    virtual MImSettingsBackend *create(const QString &key, QObject *parent);

private:
    QTemporaryFile mTempFile;
    QScopedPointer<QSettings> mSettings;
};

#endif // MIMSETTINGSQSETTINGS_H
