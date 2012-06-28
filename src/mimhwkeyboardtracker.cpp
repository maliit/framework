/* * This file is part of Maliit framework *
 *
 * Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

// This file is based on mkeyboardstatetracker.cpp from libmeegotouch

#include "mimhwkeyboardtracker.h"
#include "mimhwkeyboardtracker_p.h"

namespace {
    const char * const keyboardPresent("/maemo/InternalKeyboard/Present");
    const char * const keyboardOpen("/maemo/InternalKeyboard/Open");
}

MImHwKeyboardTrackerPrivate::MImHwKeyboardTrackerPrivate(MImHwKeyboardTracker *q_ptr) :
#ifdef HAVE_CONTEXTSUBSCRIBER
    keyboardOpenProperty(),
#else
# ifdef Q_WS_MAEMO_5
    keyboardOpenConf("/system/osso/af/slide-open"),
# endif
#endif
    present(false)
{
#ifdef HAVE_CONTEXTSUBSCRIBER
    ContextProperty keyboardPresentProperty(keyboardPresent);
    keyboardOpenProperty.reset(new ContextProperty(keyboardOpen));
    keyboardPresentProperty.waitForSubscription(true);
    keyboardOpenProperty->waitForSubscription(true);
    present = keyboardPresentProperty.value().toBool();
    if (present) {
        QObject::connect(keyboardOpenProperty.data(), SIGNAL(valueChanged()),
                         q_ptr, SIGNAL(stateChanged()));
    } else {
        keyboardOpenProperty.reset();
    }
#else
# ifdef Q_WS_MAEMO_5
    present = true;
    QObject::connect(&keyboardOpenConf, SIGNAL(valueChanged()),
                     q_ptr, SIGNAL(stateChanged()));
# else
    Q_UNUSED(q_ptr);
# endif
#endif
}

MImHwKeyboardTrackerPrivate::~MImHwKeyboardTrackerPrivate()
{
}

MImHwKeyboardTracker::MImHwKeyboardTracker()
    : QObject(),
      d_ptr(new MImHwKeyboardTrackerPrivate(this))
{
}

MImHwKeyboardTracker::~MImHwKeyboardTracker()
{
}

bool MImHwKeyboardTracker::isPresent() const
{
    Q_D(const MImHwKeyboardTracker);

    return d->present;
}

bool MImHwKeyboardTracker::isOpen() const
{
    Q_D(const MImHwKeyboardTracker);

    if (!d->present) {
        return false;
    }

#ifdef HAVE_CONTEXTSUBSCRIBER
    return d->keyboardOpenProperty->value().toBool();
#else
# ifdef Q_WS_MAEMO_5
    return d->keyboardOpenConf.value().toBool();
# else
    return false;
# endif
#endif
}
