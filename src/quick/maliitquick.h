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

#ifndef MALIIT_KEYBOARD_QUICK_H
#define MALIIT_KEYBOARD_QUICK_H

#include <QObject>
#include <maliit/namespace.h>

//! \brief MaliitQuick exposes Maliit enums to QML-based input methods
class MaliitQuick: public QObject
{
    Q_OBJECT
    Q_ENUMS(SwitchDirection)
    Q_ENUMS(ContentType)
    Q_ENUMS(KeyEvent)
    Q_ENUMS(PreeditFace)

public:
    //! This enum defines direction of plugin switching
    enum SwitchDirection {
        SwitchUndefined = Maliit::SwitchUndefined, //!< Special value for uninitialized variables
        SwitchForward = Maliit::SwitchForward, //!< Activate next plugin
        SwitchBackward = Maliit::SwitchBackward //!< Activate previous plugin
    };

    enum ContentType {
        FreeTextContentType = Maliit::FreeTextContentType,
        NumberContentType = Maliit::NumberContentType,
        PhoneNumberContentType = Maliit::PhoneNumberContentType,
        EmailContentType = Maliit::EmailContentType,
        UrlContentType = Maliit::UrlContentType,
        CustomContentType = Maliit::CustomContentType
    };

    enum KeyEvent {
        KeyPress,
        KeyRelease,
        KeyClick
    };

    enum PreeditFace {
        PreeditDefault = Maliit::PreeditDefault,
        PreeditNoCandidates = Maliit::PreeditNoCandidates,
        PreeditKeyPress = Maliit::PreeditKeyPress
    };
};

#endif // MALIIT_KEYBOARD_QUICK_H
