/***************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (directui@nokia.com)
**
** This file is part of libmeegotouch.
**
** If you have questions regarding the use of this file, please contact
** Nokia at directui@nokia.com.
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation
** and appearing in the file LICENSE.LGPL included in the packaging
** of this file.
**
****************************************************************************/

#ifndef MIMSETTINGS_H
#define MIMSETTINGS_H

#ifndef MALIIT_FRAMEWORK_USE_INTERNAL_API
#error Internal API only.
#else

#include <QVariant>
#include <QStringList>
#include <QObject>
#include <QScopedPointer>

class MImSettingsBackend : public QObject
{
    Q_OBJECT

public:
    explicit MImSettingsBackend(QObject *parent = 0);
    virtual ~MImSettingsBackend();

    virtual QString key() const = 0;
    virtual QVariant value(const QVariant &def) const = 0;
    virtual void set(const QVariant &val) = 0;
    virtual void unset() = 0;
    virtual QList<QString> listDirs() const = 0;
    virtual QList<QString> listEntries() const = 0;

Q_SIGNALS:
    void valueChanged();
};

//! \internal

/*!
  \ingroup maliitserver
  \brief MImSettings is a simple C++ wrapper for GConf.

  Creating a MImSettings instance gives you access to a single GConf
  key.  You can get and set its value, and connect to its
  valueChanged() signal to be notified about changes.

  The value of a GConf key is returned to you as a QVariant, and you
  pass in a QVariant when setting the value.  MGConfItem converts
  between a QVariant and GConf values as needed, and according to the
  following rules:

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

  \warning MGConfItem is as thread-safe as GConf.

*/

class MImSettings : public QObject
{
    Q_OBJECT

public:
    /*! Initializes a MGConfItem to access the GConf key denoted by
        \a key.  Key names should follow the normal GConf conventions
        like "/myapp/settings/first".

        \param key    The name of the key.
        \param parent Parent object
    */
    explicit MImSettings(const QString &key, QObject *parent = 0);

    /*! Finalizes a MGConfItem.
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

    /*! Set the value of this item to \a val.  If \a val can not be
        represented in GConf or GConf refuses to accept it for other
        reasons, the current value is not changed and nothing happens.

        When the new value is different from the old value, the
        changedValue() signal is emitted on this MGConfItem as part
        of calling set(), but other MGConfItem:s for the same key do
        only receive a notification once the main loop runs.

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

    static QHash<QString, QVariant> defaults();

Q_SIGNALS:
    /*! Emitted when the value of this item has changed.
     */
    void valueChanged();

private:
    QScopedPointer<MImSettingsBackend> backend;
};

//! \internal_end

#endif // MALIIT_FRAMEWORK_USE_INTERNAL_API

#endif // MIMSETTINGS_H
