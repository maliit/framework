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

#ifndef MIMSETTINGS_H
#define MIMSETTINGS_H

#ifndef MALIIT_FRAMEWORK_USE_INTERNAL_API
#error Internal API only.
#else

#include <QVariant>
#include <QStringList>
#include <QObject>
#include <QScopedPointer>


//! \internal

/*!
  \ingroup maliitserver
  \brief Implements a storage backend for MImSettings
*/
class MImSettingsBackend : public QObject
{
    Q_OBJECT

public:
    /*! Initialized a MImSettingsBackend.
     */
    explicit MImSettingsBackend(QObject *parent = 0);

    /*! Finalizes a MImSettingsBackend.
     */
    virtual ~MImSettingsBackend();

    /*! Returns the key of this item, as given to MImSettings constructor.

        \sa MImSettings::key()
     */
    virtual QString key() const = 0;

    /*! Returns the current value of this item, as a QVariant.  If
        there is no value for this item, return \a def instead.

        The implementation is:
        - return the value associated to the key (if present)
        - otherwise return the value listed in MImSettings::getDefaults()
          (if present)
        - otherwise return the passed-in default value

        \sa MImSettings::value()
     */
    virtual QVariant value(const QVariant &def) const = 0;

    /*! Set the value of this item to \a val.

        Must emit valueChanged() if the new value differs from the old one.

        \param val  The new value (is always a valid QVariant).

        \sa MImSettings::set()
    */
    virtual void set(const QVariant &val) = 0;

    /*! Unset this item.

        Must emit valueChanged() if it deletes the entry.

        This backend method is called both for MImSettings::unset()
        and MImSettings::set(QVariant()).

        \sa MImSettings::unset()
        \sa MImSettings::set()
     */
    virtual void unset() = 0;

    /*! Return a list of the directories below this item.  The
        returned strings are absolute key names like
        "/myapp/settings".

        A directory is a key that has children.

        \sa MImSettings::listDirs()
    */
    virtual QList<QString> listDirs() const = 0;

    /*! Return a list of entries below this item.  The returned
        strings are absolute key names like "/myapp/settings/first".

        A entry is a key that has a value.

        \sa MImSettings::listEntries()
    */
    virtual QList<QString> listEntries() const = 0;

Q_SIGNALS:
    /*! Emitted when the value is changed or unset.

        \sa MImSettingsBackend::valueChanged()
    */
    void valueChanged();
};


//! \internal

/*!
  \ingroup maliitserver
  \brief Factory for MImSettings backend implementations
*/
class MImSettingsBackendFactory
{
public:
    /*! Creates a backend instance for the specified key.
    */
    virtual MImSettingsBackend *create(const QString &key, QObject *parent) = 0;

    virtual ~MImSettingsBackendFactory() {}
};


//! \internal

/*!
  \ingroup maliitserver
  \brief MImSettings is a generic interface to access configuration values

  Creating a MImSettings instance gives you access to a single configuration
  key.  You can get and set its value, and connect to its
  valueChanged() signal to be notified about changes.

  The value of the key is returned to you as a QVariant, and you
  pass in a QVariant when setting the value.

  Before making use of MImSettings, you must call MImSettings::setPreferredSettingsType().

  \warning MImSettings is not reentrant.
*/

class MImSettings : public QObject
{
    Q_OBJECT

public:
    enum SettingsType {
        InvalidSettings,
        TemporarySettings,
        PersistentSettings
    };

public:
    /*! Initializes a MImSettings to access the configuratin key denoted by
        \a key.  Key names are formatted like Unix filesystem paths (es.
        like "/myapp/settings/first").

        \param key    The name of the key.
        \param parent Parent object
    */
    explicit MImSettings(const QString &key, QObject *parent = 0);

    /*! Finalizes a MImSettings.
     */
    virtual ~MImSettings();

    /*! Returns the key of this item, as given to the constructor.
     */
    QString key() const;

    /*! Returns the current value of this item, as a QVariant.
     */
    QVariant value() const;

    /*! Returns the current value of this item, as a QVariant.  If
     *  there is no value for this item, return \a def instead.
     */
    QVariant value(const QVariant &def) const;

    /*! Set the value of this item to \a val.  If string \a val fails
        for any reason, the current value is not changed and nothing happens.

        When the new value is different from the old value, the
        changedValue() signal is emitted on this MImSettings and on all
        other MImSettings instances for the same key.

        Depending on the backend, the signals might be emitted immediatly
        or the next time the main event loop runs.

        \param val  The new value.
    */
    void set(const QVariant &val);

    /*! Unset this item.  This is equivalent to

        \code
        item.set(QVariant(QVariant::Invalid));
        \endcode
     */
    void unset();

    /*! Return a list of the directories below this item.  The
        returned strings are absolute key names like
        "/myapp/settings".

        A directory is a key that has children.  The same key might
        also have a value, but that is confusing and best avoided.
    */
    QList<QString> listDirs() const;

    /*! Return a list of entries below this item.  The returned
        strings are absolute key names like "/myapp/settings/first".

        A entry is a key that has a value.  The same key might also
        have children, but that is confusing and is best avoided.
    */
    QList<QString> listEntries() const;

    /*! Set the factory used to create backend implementations.

        Should be called at most once at startup, and is meant to be used
        only for tests. Takes ownership of the passed instance.
    */
    static void setImplementationFactory(MImSettingsBackendFactory *factory);

    /*! Set the preferred settings type for backend implementations
     *
     * Should be called at most once at startup, before creating MImSetting instances.
     * This is not honored if using setImplementationFactory() manually.
     */
    static void setPreferredSettingsType(SettingsType setting);

    /*! Return the default values used for some keys
    */
    static QHash<QString, QVariant> defaults();

Q_SIGNALS:
    /*! Emitted when the value of this item has changed.
     */
    void valueChanged();

private:
    QScopedPointer<MImSettingsBackend> backend;
    static QScopedPointer<MImSettingsBackendFactory> factory;
    static SettingsType preferredSettingsType;
};

//! \internal_end

#endif // MALIIT_FRAMEWORK_USE_INTERNAL_API

#endif // MIMSETTINGS_H
