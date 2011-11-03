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

#ifndef MIMUPDATERECEIVER_H
#define MIMUPDATERECEIVER_H

#include <QtCore>

class MImUpdateReceiverPrivate;
class MImUpdateEvent;

//! An example that turns data-polling from events into data-pushing via
//! properties and change notifications.
//! Used in combination with MImUpdateEvents: The event is propagated down to
//! the final component, where it is then processed by an event receiver. The
//! receiver therefore acts as an endpoint for events, allowing the actual
//! consumers of event data to listen to property changes. Using this approach,
//! consumers are completely decoupled from the event propagation itself.
//! Event receivers implemented in plugins are supposed to be more specialized
//! than the generic MImUpdateReceiver. They can be designed to only carry the
//! properties that is required for a given endpoint.
//! The life time of event receivers usually exceeds the life time of events,
//! which allows to keep signal connections alive.
class MImUpdateReceiver
    : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(MImUpdateReceiver)
    Q_DECLARE_PRIVATE(MImUpdateReceiver)

    Q_PROPERTY(bool westernNumericInputEnforced
               READ westernNumericInputEnforced
               NOTIFY westernNumericInputEnforcedChanged)
    Q_PROPERTY(bool preferNumbers
               READ preferNumbers
               NOTIFY preferNumbersChanged)
    Q_PROPERTY(bool translucentInputMethod
               READ translucentInputMethod
               NOTIFY translucentInputMethodChanged)

public:
    explicit MImUpdateReceiver(QObject *parent = 0);
    virtual ~MImUpdateReceiver();

    void process(MImUpdateEvent *ev);

    bool westernNumericInputEnforced() const;
    bool preferNumbers() const;
    bool translucentInputMethod() const;

Q_SIGNALS:
    void westernNumericInputEnforcedChanged(bool value);
    void preferNumbersChanged(bool value);
    void translucentInputMethodChanged(bool value);

private:
    MImUpdateReceiverPrivate *const d_ptr;
};

#endif // MIMUPDATERECEIVER_H
