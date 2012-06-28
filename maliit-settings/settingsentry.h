/* * This file is part of Maliit framework *
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


/*!
 * \brief Configuration entry for an input method plugin
 */
class SettingsEntry : public QObject
{
    Q_OBJECT

public:
    virtual ~SettingsEntry();

    /*!
     * \brief Returns the current value of this item, as a QVariant.
     */
    QVariant value() const;

    /*!
     * \brief Returns the current value of this item, as a QVariant.  If
     * there is no value for this item, return \a def instead.
     */
    QVariant value(const QVariant &def) const;

    //! Sets a new value for this configuration entry
    void set(const QVariant &val);

    //! Checks whether \val is a valid value for this configuration entry
    bool isValid(const QVariant &val);

    QString key() const;

    //! Human-readable description for this configuration entry
    QString description() const;

    //! Value type for this configuration entry
    SettingEntryType type() const;

    /*!
     * \brief Metadata for this configuration entry; keys can be:
     *
     * - valueDomain: list of allowed values for the entry
     * - valueDomainDescriptions: list of descriptions for the values in valueDomain
     * - valueRangeMin: minimum integer value (inclusive) for the entry
     * - valueRangeMax: maximum integer value (inclusive) for the entry
     *
     * \sa Maliit::SettingEntryAttributes
     */
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
