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



#ifndef MTOOLBARDATA_P_H
#define MTOOLBARDATA_P_H

#include <QSharedPointer>
#include <QMap>
#include <QStringList>
#include <QString>

#include <mtoolbaritem.h>
#include "mkeyoverride.h"

class MToolbarData;

class MToolbarDataPrivate
{
public:
    Q_DECLARE_PUBLIC(MToolbarData)

    //! Construct new instance.
    explicit MToolbarDataPrivate(MToolbarData *owner);

    /*!
     * \brief Return item state associated with the given \a name and \a type.
     *
     * If state with specified name does not exist, then it will be added.
     */
    QSharedPointer<MToolbarItem> getOrCreateItemByName(const QString &name,
                                                       MInputMethod::ItemType type);

    /*!
     * \brief Translate alignmentString to Qt::Alignment.
     */
    static Qt::Alignment alignment(const QString &alignmentString);

    /*!
    * \brief Translate orientationString to M::Orientation.
    */
    static M::Orientation orientation(const QString &orientationString);

    /*!
     * \brief Translate visibleTypeString to ToolbarWidget::VisibleType.
     */
    static MInputMethod::VisibleType visibleType(const QString &visibleTypeString);

    typedef void (MToolbarItem::*SetString)(const QString &);
    /*!
     * \brief Read value from given \a attributeName of \a element and call given
     * \a setter for currentItem in \a params
     *
     * \a setter is not called if specified attrubute does not exist.
     */
    void parseAttribute(SetString setter, const QDomElement &element,
                        const QString &attributeName, const MTBParseParameters &params);

    typedef void (MToolbarItem::*SetVisibleType)(MInputMethod::VisibleType);
    //! This overloaded function is added for convinience
    void parseAttribute(SetVisibleType setter, const QDomElement &element,
                        const QString &attributeName, const MTBParseParameters &params);

    typedef void (MToolbarItem::*SetBool)(bool);
    //! This overloaded function is added for convinience
    void parseAttribute(SetBool setter, const QDomElement &element,
                        const QString &attributeName, const MTBParseParameters &params);

    typedef void (MToolbarItem::*SetAlignment)(Qt::Alignment);
    //! This overloaded function is added for convinience
    void parseAttribute(SetAlignment setter, const QDomElement &element,
                        const QString &attributeName, const MTBParseParameters &params);

    typedef void (MToolbarItem::*SetInt)(int);
    //! This overloaded function is added for convinience
    void parseAttribute(SetInt setter, const QDomElement &element,
                        const QString &attributeName, const MTBParseParameters &params);

    //! Parse XML tag for input method.
    void parseTagInputMethod(const QDomElement &element, MTBParseParameters &params);

    //! Parse XML tag for toolbar.
    void parseTagToolbar(const QDomElement &element, MTBParseParameters &params);

    //! Parse XML tag for layout.
    void parseTagLayout(const QDomElement &element, MTBParseParameters &params);

    //! Parse XML tag for row.
    void parseTagRow(const QDomElement &element, MTBParseParameters &params);

    //! Parse XML tag for button.
    void parseTagButton(const QDomElement &element, MTBParseParameters &params);

    //! Parse XML tag for label.
    void parseTagLabel(const QDomElement &element, MTBParseParameters &params);

    //! Parse XML tag for action.
    void parseTagActions(const QDomElement &element, MTBParseParameters &params);

    //! Parse XML tag for SendKeySequence.
    void parseTagSendKeySequence(const QDomElement &element, MTBParseParameters &params);

    //! Parse XML tag for SendString
    void parseTagSendString(const QDomElement &element, MTBParseParameters &params);

    //! Parse XML tag for SendCommand
    void parseTagSendCommand(const QDomElement &element, MTBParseParameters &params);

    //! Parse XML tag for Copy
    void parseTagCopy(const QDomElement &element, MTBParseParameters &params);

    //! Parse XML tag for Paste
    void parseTagPaste(const QDomElement &element, MTBParseParameters &params);

    //! Parse XML tag for CopyPaste
    void parseTagCopyPaste(const QDomElement &element, MTBParseParameters &params);

    //! Parse XML tag for Close
    void parseTagClose(const QDomElement &element, MTBParseParameters &params);

    //! Parse XML tag for ShowGroup
    void parseTagShowGroup(const QDomElement &element, MTBParseParameters &params);

    //! Parse XML tag for HideGroup
    void parseTagHideGroup(const QDomElement &element, MTBParseParameters &params);

    //! Parse XML tag for Items
    void parseTagItems(const QDomElement &element, MTBParseParameters &params);

    //! Parse XML tag for Item
    void parseTagItem(const QDomElement &element, MTBParseParameters &params);

    //! Type of tag parser methods
    typedef void (MToolbarDataPrivate::*TagParser)(const QDomElement &, MTBParseParameters &);

    /*!
     * \brief Helper method for parsing children of an element
     * \param element Element whose children are to be parsed
     * \param params Parsing state
     * \param parserList, a TBParseStructure array.
     * \param parserCount, the number of TBParseStructure in the array.
     */
    void parseChildren(const QDomElement &element, MTBParseParameters &params,
                       const MTBParseStructure *parserList, int parserCount = 1);

    /*!
     * \brief Helper method for parsing DOM element
     * \param element Element to be parsed
     * \param params Parsing state
     * \param parserList, a TBParseStructure array.
     * \param parserCount, the number of TBParseStructure in the array.
     */
    void parseDomElement(const QDomElement &element, MTBParseParameters &params,
                         const MTBParseStructure *parserList, int parserCount = 1);
public:
    MToolbarData *q_ptr;

    //! Layout for landscape orientation
    QSharedPointer<MToolbarLayout> layoutLandscape;

    //! Layout for portrait orientation
    QSharedPointer<MToolbarLayout> layoutPortrait;

    //! Name of file containing this toolbar
    QString toolbarFileName;

    typedef QMap<QString, QSharedPointer<MToolbarItem> > Items;
    Items items;

    typedef QMap<QString, QSharedPointer<MKeyOverride> > KeyOverrides;
    KeyOverrides keyOverrides;

    bool visible;

    QStringList refusedNames;
};

#endif

