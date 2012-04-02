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

#include <QObject>
#include <QSharedPointer>

#include <mkeyoverride.h>

class MKeyOverrideQuickPrivate;

//! MKeyOverrideQuick stores some attributes of a key for QtQuick virtual keyboard.
class MKeyOverrideQuick : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(MKeyOverrideQuick)

    // actual properties
    Q_PROPERTY(QString label    READ label       WRITE overrideLabel       NOTIFY labelChanged)
    Q_PROPERTY(QString icon     READ icon        WRITE overrideIcon        NOTIFY iconChanged)
    Q_PROPERTY(bool highlighted READ highlighted WRITE overrideHighlighted NOTIFY highlightedChanged)
    Q_PROPERTY(bool enabled     READ enabled     WRITE overrideEnabled     NOTIFY enabledChanged)

    // default properties
    Q_PROPERTY(QString defaultLabel    READ defaultLabel       WRITE setDefaultLabel       NOTIFY defaultLabelChanged)
    Q_PROPERTY(QString defaultIcon     READ defaultIcon        WRITE setDefaultIcon        NOTIFY defaultIconChanged)
    Q_PROPERTY(bool defaultHighlighted READ defaultHighlighted WRITE setDefaultHighlighted NOTIFY defaultHighlightedChanged)
    Q_PROPERTY(bool defaultEnabled     READ defaultEnabled     WRITE setDefaultEnabled     NOTIFY defaultEnabledChanged)

public:
    //! Constructor.
    MKeyOverrideQuick();

    //! Destructor.
    virtual ~MKeyOverrideQuick();

    //! Returns text from the key
    QString label() const;

    //! Returns icon name
    QString icon() const;

    //! Return true if the key is highlighted; otherwise return false.
    bool highlighted() const;

    //! Return true if the key is enabled; otherwise return false.
    bool enabled() const;

    //! Returns default text from the key
    QString defaultLabel() const;

    //! Returns default icon name
    QString defaultIcon() const;

    //! Return true if the key is by default highlighted; otherwise return false.
    bool defaultHighlighted() const;

    //! Return true if the key is by default enabled; otherwise return false.
    bool defaultEnabled() const;

public Q_SLOTS:
    //! Applies overrides given in \a keyOverride.
    void applyOverride(const QSharedPointer<MKeyOverride>& keyOverride,
                       const MKeyOverride::KeyOverrideAttributes changedAttributes);

    //! Override actual label.
    void overrideLabel(const QString &label);

    //! Override actual icon.
    void overrideIcon(const QString &icon);

    //! Override actual highlighted.
    void overrideHighlighted(bool highlighted);

    //! Override actual enabled.
    void overrideEnabled(bool enabled);

    //! Set default text for the key.
    void setDefaultLabel(const QString &label);

    //! Set default icon name.
    void setDefaultIcon(const QString &icon);

    //! Set default highlighted state for item.
    void setDefaultHighlighted(bool highlighted);

    //! Set default enabled state for item.
    void setDefaultEnabled(bool enabled);

    //! Set actual label to use a default one.
    void useDefaultLabel();

    //! Set actual icon to use a default one.
    void useDefaultIcon();

    //! Set actual highlighted to use a default one.
    void useDefaultHighlighted();

    //! Set actual enabled to use a default one.
    void useDefaultEnabled();

Q_SIGNALS:
    //! Emitted when actual label is changed.
    void labelChanged(const QString &label);

    //! Emitted when actual icon is changed.
    void iconChanged(const QString &icon);

    //! Emitted when actual highlighted is changed.
    void highlightedChanged(bool highlighted);

    //! Emitted when actual enabled is changed.
    void enabledChanged(bool enabled);

    //! Emitted when default label is changed.
    void defaultLabelChanged(const QString &label);

    //! Emitted when default icon is changed.
    void defaultIconChanged(const QString &icon);

    //! Emitted when default highlighted is changed.
    void defaultHighlightedChanged(bool highlighted);

    //! Emitted when default enabled is changed.
    void defaultEnabledChanged(bool enabled);

private:
    //! Sets actual label and marks it as either overriden or default.
    void setLabel(const QString &label, bool overriden);

    //! Sets actual icon and marks it as either overriden or default.
    void setIcon(const QString &icon, bool overriden);

    //! Sets actual highlighted and marks it as either overriden or default.
    void setHighlighted(bool highlighted, bool overriden);

    //! Sets actual enabled and marks it as either overriden or default.
    void setEnabled(bool enabled, bool overriden);

    Q_DECLARE_PRIVATE(MKeyOverrideQuick)

    MKeyOverrideQuickPrivate *d_ptr;
};

#endif // MALIIT_QUICK_KEY_OVERRIDE_H
