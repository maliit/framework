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

#ifndef MALIIT_NAMESPACE_H
#define MALIIT_NAMESPACE_H

#include <QMetaType>
#include <QSharedPointer>

//! \ingroup common
namespace Maliit {
    /*!
     * \brief Position of the window on the screen.
     */
    enum Position {
        PositionOverlay,
        PositionCenterBottom,
        PositionLeftBottom,
        PositionRightBottom,
    };

    /*!
     * \brief Content type for text entries.
     *
     * Content type of the text in the text edit widget, which can be used by
     * input method plugins to offer more specific input methods, such as a
     * numeric keypad for a number content type. Plugins may also adjust their
     * word prediction and error correction accordingly.
     */
    enum TextContentType {
        //! all characters allowed
        FreeTextContentType,

        //! only integer numbers allowed
        NumberContentType,

        //! allows numbers and certain other characters used in phone numbers
        PhoneNumberContentType,

        //! allows only characters permitted in email address
        EmailContentType,

        //! allows only character permitted in URL address
        UrlContentType,

        //! allows content with user defined format
        CustomContentType
    };

    /*!
     * \brief State of Copy/Paste button.
     */
    enum CopyPasteState {
        //! Copy/Paste button is hidden
        InputMethodNoCopyPaste,

        //! Copy button is accessible
        InputMethodCopy,

        //! Paste button is accessible
        InputMethodPaste
    };

    /*!
     * \brief Direction of plugin switching
     */
    enum SwitchDirection {
        SwitchUndefined, //!< Special value for uninitialized variables
        SwitchForward, //!< Activate next plugin
        SwitchBackward //!< Activate previous plugin
    };

    enum PreeditFace {
        PreeditDefault,
        PreeditNoCandidates,
        PreeditKeyPress,      //!< Used for displaying the hwkbd key just pressed
        PreeditUnconvertible, //!< Inactive preedit region, not clickable
        PreeditActive         //!< Preedit region with active suggestions
    };

    enum HandlerState {
        OnScreen,
        Hardware,
        Accessory
    };

    //! \brief Key event request type for \a MInputContext::keyEvent().
    enum EventRequestType {
        EventRequestBoth,         //!< Both a Qt::KeyEvent and a signal
        EventRequestSignalOnly,   //!< Only a signal
        EventRequestEventOnly     //!< Only a Qt::KeyEvent
    };

    /*!
     * \brief The text format for part of the preedit string, specified by
     * start and length.
     *
     * \sa PreeditFace.
     */
    struct PreeditTextFormat {
        int start;
        int length;
        PreeditFace preeditFace;

        PreeditTextFormat()
            : start(0), length(0), preeditFace(PreeditDefault)
        {};

        PreeditTextFormat(int s, int l, const PreeditFace &face)
            : start(s), length(l), preeditFace(face)
        {};
    };

    namespace InputMethodQuery
    {
        //! Name of property which tells whether correction is enabled.
        //! \sa Maliit::ImCorrectionEnabledQuery.
        const char* const correctionEnabledQuery = "maliit-correction-enabled";
        //! Name of property which holds ID of attribute extension.
        //! \sa Maliit::InputMethodAttributeExtensionIdQuery.
        const char* const attributeExtensionId = "maliit-attribute-extension-id";
        //! Name of property which holds attribute extension.
        //! \sa Maliit::InputMethodAttributeExtensionQuery.
        const char* const attributeExtension = "maliit-attribute-extension";
        //! Name of the property which overrides localized numeric input with western numeric input.
        //! \sa Maliit::WesternNumericInputEnforcedQuery.
        const char* const westernNumericInputEnforced = "maliit-western-numeric-input-enforced";
        //! Name of the property which controls translucent VKB mode.
        const char* const translucentInputMethod = "maliit-translucent-input-method";
        //! Name of the property which can suppress VKB even if focused.
        //! \sa Maliit::VisualizationPriorityQuery
        const char* const suppressInputMethod = "maliit-suppress-input-method";
    }

    enum SettingEntryType
    {
        StringType     = 1,
        IntType        = 2,
        BoolType       = 3,
        StringListType = 4,
        IntListType    = 5
    };

    namespace SettingEntryAttributes
    {
        //! Name of setting entry attribute which holds the list of values that can be assigned to the entry.
        //! \sa Maliit::SettingsEntry
        const char* const valueDomain = "valueDomain";
        //! Name of setting entry attribute which holds the descriptions for the values in valueDomain.
        //! \sa Maliit::SettingsEntry
        const char* const valueDomainDescriptions = "valueDomainDescriptions";
        //! Name of setting entry attribute which holds the minimum valid value (inclusive) for an integer property.
        //! \sa Maliit::SettingsEntry
        const char* const valueRangeMin = "valueRangeMin";
        //! Name of setting entry attribute which holds the maximum valid value (inclusive) for an integer property.
        //! \sa Maliit::SettingsEntry
        const char* const valueRangeMax = "valueRangeMax";
        //! Name of setting entry attribute which holds the default value for a setting entry.
        //! \sa Maliit::SettingsEntry
        const char* const defaultValue = "defaultValue";
    }
}

Q_DECLARE_METATYPE(Maliit::TextContentType)
Q_DECLARE_METATYPE(Maliit::PreeditTextFormat)
Q_DECLARE_METATYPE(QList<Maliit::PreeditTextFormat>)

#endif
