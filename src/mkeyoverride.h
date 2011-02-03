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



#ifndef MKEYOVERRIDE_H
#define MKEYOVERRIDE_H

#include <QObject>
#include <QList>
#include <QString>
#include <QStringList>

#include "minputmethodnamespace.h"
#include <MNamespace>

class MKeyOverridePrivate;

/*!
 * \brief MKeyOverride is used to store key attribute overrides for virtual keyboard.
 */
class MKeyOverride : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString label  READ label    WRITE setLabel)
    Q_PROPERTY(QString icon   READ icon    WRITE setIcon)
    Q_PROPERTY(bool highlighted   READ highlighted WRITE setHighlighted)
    Q_PROPERTY(bool enabled   READ enabled WRITE setEnabled)

public:
    /*!
    * \brief Constructor
    */
    MKeyOverride(const QString &keyId);

    /*!
     * \brief Copy constructor
     */
    MKeyOverride(const MKeyOverride&);

    /*!
    * \brief Destructor
    */
    virtual ~MKeyOverride();

    /*
     * \brief Assignment operator
     */
    const MKeyOverride &operator=(const MKeyOverride &other);

    /*!
     * \brief Returns the key id.
     */
    QString keyId() const;

    //! Returns text from the key
    QString label() const;

    //! Returns icon name
    QString icon() const;

    //! Return true if the key is highlighted; otherwise return false.
    bool highlighted() const;

    //! Return true if the key is enabled; otherwise return false.
    bool enabled() const;

public slots:
    //! Sets text for the key
    void setLabel(const QString &label);

    //! Sets icon name
    void setIcon(const QString &icon);

    /*!
     * \brief Set highlighted state for item
     *
     */
    void setHighlighted(bool highlighted);

    /*!
     * \brief  If \a enabled is true, the key is enabled; otherwise, it is disabled.
     *
     */
    void setEnabled(bool enabled);

signals:
    /*!
     * \brief Emitted when some property is changed
     * \param propertyName Specifies name of changed property
     */
    void propertyChanged(const QString &propertyName);

private:
    Q_DECLARE_PRIVATE(MKeyOverride)

    MKeyOverridePrivate *const d_ptr;

    friend class Ut_MKeyOverride;
};


#endif
