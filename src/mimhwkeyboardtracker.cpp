/* * This file is part of Maliit framework *
 *
 * Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 *
 * Copyright (C) 2012 One Laptop per Child Association
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

#include <QSocketNotifier>

#ifdef __cplusplus
extern "C" {
#include <libudev.h>
}
#else
#include <libudev.h>
#endif
#include <linux/input.h>

#include "mimhwkeyboardtracker.h"
#include "mimhwkeyboardtracker_p.h"

/* bit array ops */
#define BITS2BYTES(x) ((((x) - 1) / 8) + 1)
#define TEST_BIT(bit, array) (array[(bit) / 8] & (1 << (bit) % 8))

namespace {
    const char * const keyboardPresent("/maemo/InternalKeyboard/Present");
    const char * const keyboardOpen("/maemo/InternalKeyboard/Open");
}

MImHwKeyboardTrackerPrivate::MImHwKeyboardTrackerPrivate(MImHwKeyboardTracker *q_ptr) :
#ifdef HAVE_CONTEXTSUBSCRIBER
    keyboardOpenProperty(),
#elif defined(Q_WS_MAEMO_5)
    keyboardOpenConf("/system/osso/af/slide-open"),
#else
    evdevTabletModePending(-1),
    evdevTabletMode(0),
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
#elif defined(Q_WS_MAEMO_5)
    present = true;
    QObject::connect(&keyboardOpenConf, SIGNAL(valueChanged()),
                     q_ptr, SIGNAL(stateChanged()));
#else
    Q_UNUSED(q_ptr);
    QObject::connect(this, SIGNAL(stateChanged()),
                     q_ptr, SIGNAL(stateChanged()));

    detectEvdev();
#endif
}

void MImHwKeyboardTrackerPrivate::detectEvdev()
{
    // Use udev to enumerate all input devices, using evdev on each device to
    // find the first device offering a SW_TABLET_MODE switch. If found, this
    // switch is used to determine keyboard presence.

    struct udev_list_entry *device;
    struct udev_list_entry *devices;

    struct udev *udev = udev_new();
    if (!udev)
        return;

    struct udev_enumerate *enumerate = udev_enumerate_new(udev);
    if (!enumerate) {
        udev_unref(udev);
        return;
    }

    udev_enumerate_add_match_subsystem(enumerate, "input");
    udev_enumerate_add_match_property(enumerate, "ID_INPUT", "1");
    udev_enumerate_scan_devices(enumerate);
    devices = udev_enumerate_get_list_entry(enumerate);

    udev_list_entry_foreach(device, devices) {
        const char *syspath = udev_list_entry_get_name(device);
        struct udev_device *udev_device =
            udev_device_new_from_syspath(udev, syspath);
        const char *device = udev_device_get_devnode(udev_device);

        if (device)
            tryEvdevDevice(device);

        udev_device_unref(udev_device);
        if (present)
            break;
    }
    udev_enumerate_unref(enumerate);
    udev_unref(udev);
}

void MImHwKeyboardTrackerPrivate::evdevEvent()
{
    // Parse the evdev event and look for SW_TABLET_MODE status.

    struct input_event ev;

    qint64 len = evdevFile->read((char *) &ev, sizeof(ev));
    if (len != sizeof(ev))
        return;

    // We wait for a SYN before "committing" the new state, just in case.
    if (ev.type == EV_SW && ev.code == SW_TABLET_MODE) {
        evdevTabletModePending = ev.value;
    } else if (ev.type == EV_SYN && ev.code == SYN_REPORT
            && evdevTabletModePending != -1) {
        evdevTabletMode = evdevTabletModePending;
        evdevTabletModePending = -1;
        Q_EMIT stateChanged();
    }

}

void MImHwKeyboardTrackerPrivate::tryEvdevDevice(const char *device)
{
    QFile *qfile = new QFile(this);
    unsigned char evbits[BITS2BYTES(EV_MAX)];
    int fd;

    qfile->setFileName(device);
    if (!qfile->open(QIODevice::ReadOnly | QIODevice::Unbuffered)) {
        delete qfile;
        return;
    }

    fd = qfile->handle();
    if (fd == -1) {
        delete qfile;
        return;
    }

    if (ioctl(fd, EVIOCGBIT(0, EV_MAX), evbits) < 0) {
        delete qfile;
        return;
    }

    // Check that this input device has switches
    if (!TEST_BIT(EV_SW, evbits)) {
        delete qfile;
        return;
    }

    unsigned char swbit[BITS2BYTES(EV_MAX)];
    if (ioctl(fd, EVIOCGBIT(EV_SW, SW_CNT), swbit) < 0) {
        delete qfile;
        return;
    }

    // Check that there is a tablet mode switch here
    if (!TEST_BIT(SW_TABLET_MODE, swbit)) {
        delete qfile;
        return;
    }

    // Found an appropriate device - start monitoring it
    QSocketNotifier *sn = new QSocketNotifier(fd, QSocketNotifier::Read, qfile);
    sn->setEnabled(true);
    QObject::connect(sn, SIGNAL(activated(int)), this, SLOT(evdevEvent()));

    evdevFile = qfile;
    present = true;

    // Initialise initial tablet mode state
    unsigned long state[BITS2BYTES(SW_MAX)];
    if (ioctl(fd, EVIOCGSW(SW_MAX), state) < 0)
        return;

    evdevTabletMode = TEST_BIT(SW_TABLET_MODE, state);
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
#elif defined(Q_WS_MAEMO_5)
    return d->keyboardOpenConf.value().toBool();
#else
    // If we found a talet mode switch, we report that the hardware keyboard
    // is available when the system is not in tablet mode (switch closed),
    // and is not available otherwise (switch open).
    if (d->evdevFile)
        return !d->evdevTabletMode;

    return false;
#endif
}
