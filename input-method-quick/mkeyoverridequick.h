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

#ifndef MALIIT_QUICK_KEY_OVERRIDE_H
#define MALIIT_QUICK_KEY_OVERRIDE_H

#include <mkeyoverride.h>

class MKeyOverrideQuickPrivate;

//! MKeyOverrideQuick is a subclass of MKeyOverride providing default values.
class MKeyOverrideQuick
    : public MKeyOverride
{
    Q_OBJECT

    Q_PROPERTY(QString defaultLabel  READ defaultLabel    WRITE setDefaultLabel NOTIFY defaultLabelChanged)
    Q_PROPERTY(QString defaultIcon   READ defaultIcon    WRITE setDefaultIcon NOTIFY defaultIconChanged)
    Q_PROPERTY(bool defaultHighlighted   READ defaultHighlighted WRITE setDefaultHighlighted NOTIFY defaultHighlightedChanged)
    Q_PROPERTY(bool defaultEnabled   READ defaultEnabled WRITE setDefaultEnabled NOTIFY defaultEnabledChanged)

public:
    //! Constructor.
    MKeyOverrideQuick(const QString &keyId);

    //! Copy constructor.
    MKeyOverrideQuick(const MKeyOverrideQuick &other);

    //! Destructor.
    virtual ~MKeyOverrideQuick();

    //! Assignment operator.
    const MKeyOverrideQuick & operator=(const MKeyOverrideQuick &other);

    //! Returns default text from the key
    QString defaultLabel() const;

    //! Returns default icon name
    QString defaultIcon() const;

    //! Return true if the key is by default highlighted; otherwise return false.
    bool defaultHighlighted() const;

    //! Return true if the key is by default enabled; otherwise return false.
    bool defaultEnabled() const;

public Q_SLOTS:
    //! Set default text for the key.
    void setDefaultLabel(const QString &label);

    //! Set default icon name.
    void setDefaultIcon(const QString &icon);

    //! Set default highlighted state for item.
    void setDefaultHighlighted(bool highlighted);

    //! Set default enabled state for item.
    void setDefaultEnabled(bool enabled);

    //! Set all values to default ones.
    void useDefaults();

Q_SIGNALS:
    //! Emitted when default label is changed.
    void defaultLabelChanged(const QString &label);

    //! Emitted when default icon is changed.
    void defaultIconChanged(const QString &icon);

    //! Emitted when default highlighted is changed.
    void defaultHighlightedChanged(bool highlighted);

    //! Emitted when default enabled is changed.
    void defaultEnabledChanged(bool enabled);

private:
    Q_DECLARE_PRIVATE(MKeyOverrideQuick)

    MKeyOverrideQuickPrivate *const d_ptr;
};

#endif // MALIIT_QUICK_KEY_OVERRIDE_H
