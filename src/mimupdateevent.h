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

#ifndef MIMUPDATEEVENT_H
#define MIMUPDATEEVENT_H

#include "mimextensionevent.h"

#include <QtCore>

class MImUpdateEventPrivate;

class MImUpdateEvent
    : public MImExtensionEvent
{
public:
    //! C'tor
    //! \param update the map containing all properties.
    //! \param propertiesChanged a string list of changed properties.
    explicit MImUpdateEvent(const QMap<QString, QVariant> &update,
                            const QStringList &propertiesChanged);

    //! C'tor
    //! \param update the map containing all properties.
    //! \param propertiesChanged a string list of changed properties.
    //! \param lastHints the last input method hints, as compared to the new
    //!        ones in the updates map. Necessary to detect whether a flag
    //!        flipped between update event.
    explicit MImUpdateEvent(const QMap<QString, QVariant> &update,
                            const QStringList &propertiesChanged,
                            const Qt::InputMethodHints &lastHints);

    //! Returns invalid QVariant if key is invalid.
    QVariant value(const QString &key) const;

    //! Returns list of keys that have changed, compared to last update event.
    QStringList propertiesChanged() const;

    //! Returns the focus widget's input method hints.
    //! \param changed whether this value changed with this event.
    Qt::InputMethodHints hints(bool *changed = 0) const;

    //! Returns whether western numeric input should be shown, overridding
    //! language-specific numeric inputs.
    //! False by default.
    //! \param changed whether this value changed with this event.
    bool westernNumericInputEnforced(bool *changed = 0) const;

    //! Returns whether input field has Qt::ImhPreferNumbers hint set.
    //! False by default.
    //! \param changed whether this value changed with this event.
    bool preferNumbers(bool *changed = 0) const;

private:
    Q_DISABLE_COPY(MImUpdateEvent)
    Q_DECLARE_PRIVATE(MImUpdateEvent)
};

#endif // MIMUPDATEEVENT_H
