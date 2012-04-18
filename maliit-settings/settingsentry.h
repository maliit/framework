/* * This file is part of meego-im-framework *
 *
 * Copyright (C) 2012 Mattia Barbon <mattia@develer.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 */

#ifndef MALIIT_SETTINGSENTRY_H
#define MALIIT_SETTINGSENTRY_H

#include <QScopedPointer>
#include <QSharedPointer>
#include <QVariant>
#include <maliit/namespace.h>

class MImPluginSettingsEntry;

namespace Maliit {

class SettingsEntryPrivate;
class AttributeExtension;

class SettingsEntry : public QObject
{
    Q_OBJECT

public:
    virtual ~SettingsEntry();

    QVariant value() const;

    QVariant value(const QVariant &def) const;

    void set(const QVariant &val);
    bool isValid(const QVariant &val);


    QString key() const;
    QString description() const;
    SettingEntryType type() const;
    QVariantMap attributes() const;

Q_SIGNALS:
    void valueChanged();

private:
    SettingsEntry(QSharedPointer<AttributeExtension> extension, const MImPluginSettingsEntry &info);

    Q_SLOT void valueChanged(const QString &key);

    QScopedPointer<SettingsEntryPrivate> d_ptr;

    Q_DISABLE_COPY(SettingsEntry)
    Q_DECLARE_PRIVATE(SettingsEntry)

    friend class PluginSettings;
};

}

#endif // MALIIT_SETTINGSENTRY_H
