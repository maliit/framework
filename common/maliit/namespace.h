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

#ifndef MALIIT_NAMESPACE_H
#define MALIIT_NAMESPACE_H

#include <QMetaType>
#include <QSharedPointer>

namespace Maliit {
    /*!
     * This enum contains values of the orientation angle of windows in the application.
     *
     * \sa Orientation
     */
    enum OrientationAngle {
        Angle0   =   0,
        Angle90  =  90,
        Angle180 = 180,
        Angle270 = 270
    };

    /*!
     * This enum contains possible values for orientation of windows in the application.
     *
     * \sa OrientationAngle
     */
    enum Orientation {
        Portrait, //!< equal to either M::Angle90 or M::Angle270 orientation angles
        Landscape //!< equal to either M::Angle0 or M::Angle180 orientation angles
    };

    //! Content type for text entries. Used at least with MTextEdit
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

    //! Type of toolbar widget
    enum ItemType {
        //! Undefined item type
        ItemUndefined,

        //! Item should be visualized as button
        ItemButton,

        //! Item should be visualized as label
        ItemLabel
    };

    //! Type of visible premiss for toolbar button
    enum VisibleType {
        //! Item's visibility will not be changed automatically
        VisibleUndefined,

        //! Item's visibility depends on text selection
        VisibleWhenSelectingText,

        //! Item is always visible
        VisibleAlways
    };

    //! Type of action
    enum ActionType {
        //! Do nothing
        ActionUndefined,

        //! Send key sequence like Ctrl+D
        ActionSendKeySequence,

        //! Send string
        ActionSendString,

        //! Send command (not implemented yet)
        ActionSendCommand,

        //! Copy selected text
        ActionCopy,

        //! Paste text from clipboard
        ActionPaste,

        //! Show some group of items
        ActionShowGroup,

        //! Hide some group of items
        ActionHideGroup,

        //! Close virtual keyboard
        ActionClose,

        //! Standard copy/paste button
        ActionCopyPaste
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
     * This enum defines direction of plugin switching
     */
    enum SwitchDirection {
        SwitchUndefined, //!< Special value for uninitialized variables
        SwitchForward, //!< Activate next plugin
        SwitchBackward //!< Activate previous plugin
    };

    enum PreeditFace {
        PreeditDefault,
        PreeditNoCandidates,
        PreeditKeyPress           //! Used for displaying the hwkbd key just pressed
    };

    enum HandlerState {
        OnScreen,
        Hardware,
        Accessory
    };

    /// \brief Key event request type for \a MInputContext::keyEvent().
    enum EventRequestType {
        EventRequestBoth,         //!< Both a Qt::KeyEvent and a signal
        EventRequestSignalOnly,   //!< Only a signal
        EventRequestEventOnly     //!< Only a Qt::KeyEvent
    };

     /*!
      * This enum contains possible values for all the modes that are shown in the
      * Input mode indicator.
      */
    enum InputModeIndicator {
        NoIndicator,                 //!< No indicator should be shown
        LatinLowerIndicator,         //!< Latin lower case mode
        LatinUpperIndicator,         //!< Latin upper case mode
        LatinLockedIndicator,        //!< Latin caps locked mode
        CyrillicLowerIndicator,      //!< Cyrillic lower case mode
        CyrillicUpperIndicator,      //!< Cyrillic upper case mode
        CyrillicLockedIndicator,     //!< Cyrillic caps locked mode
        ArabicIndicator,             //!< Arabic mode
        PinyinIndicator,             //!< Pinyin mode
        ZhuyinIndicator,             //!< Zhuyin mode
        CangjieIndicator,            //!< Cangjie mode
        NumAndSymLatchedIndicator,   //!< Number and Symbol latched mode
        NumAndSymLockedIndicator,    //!< Number and Symbol locked mode
        DeadKeyAcuteIndicator,       //!< Dead key acute mode
        DeadKeyCaronIndicator,       //!< Dead key caron mode
        DeadKeyCircumflexIndicator,  //!< Dead key circumflex mode
        DeadKeyDiaeresisIndicator,   //!< Dead key diaeresis mode
        DeadKeyGraveIndicator,       //!< Dead key grave mode
        DeadKeyTildeIndicator        //!< Dead key tilde mode
    };

    enum InputMethodMode {
        //! Normal mode allows to use preedit and error correction
        InputMethodModeNormal,

        //! Virtual keyboard sends QKeyEvent for every key press or release
        InputMethodModeDirect,

        //! Used with proxy widget
        InputMethodModeProxy
    };

    /*!
     * PreeditTextFormat defines the text format for part of the preedit string
     * specified by start and length.
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

    //! extensions for Qt::inputMethodQuery
    enum InputMethodQueryExtensions {
        VisualizationPriorityQuery = 10001, //!< Tells if input method widget wants to have high
        //!< priority for visualization. Input method should
        //!< honor this and stay out of widgets space.
        PreeditRectangleQuery,      //!< Retrieve bounding rectangle for current preedit text.
        ImCorrectionEnabledQuery,   //!< Explicit correction enabling for text entries.
        ImModeQuery,                //!< Retrieve mode: normal, direct or proxy.
        InputMethodAttributeExtensionIdQuery, //!< Attribute extension identifier for text entry.
        InputMethodAttributeExtensionQuery, //!< Attribute extension file name for text entry.
        WesternNumericInputEnforcedQuery //!< Overrides localized numeric input with western numeric input.
    };

    namespace InputMethodQuery
    {
        //! Name of property which tells whether correction is enabled.
        //! See Maliit::ImCorrectionEnabledQuery.
        const char* const correctionEnabledQuery = "maliit-correction-enabled";
        //! Name of property which holds ID of attribute extension.
        //! See Maliit::InputMethodAttributeExtensionIdQuery.
        const char* const attributeExtensionId = "maliit-attribute-extension-id";
        //! Name of property which holds attribute extension.
        //! See Maliit::InputMethodAttributeExtensionQuery.
        const char* const attributeExtension = "maliit-attribute-extension";
        //! Name of the property which overrides localized numeric input with western numeric input.
        //! See Maliit::WesternNumericInputEnforcedQuery.
        const char* const westernNumericInputEnforced = "maliit-western-numeric-input-enforced";
        //! Name of the property which controls translucent VKB mode.
        const char* const translucentInputMethod = "maliit-translucent-inputmethod";
        //! Name of the property which can suppress VKB even if focused.
        //! See Maliit::VisualizationPriorityQuery
        const char* const suppressInputMethod = "maliit-suppress-inputmethod";
    }
}

Q_DECLARE_METATYPE(Maliit::TextContentType)
Q_DECLARE_METATYPE(Maliit::Orientation)
Q_DECLARE_METATYPE(Maliit::OrientationAngle)
Q_DECLARE_METATYPE(Maliit::InputMethodMode)
Q_DECLARE_METATYPE(Maliit::InputMethodQueryExtensions)

#endif
