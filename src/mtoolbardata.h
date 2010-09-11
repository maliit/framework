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



#ifndef MTOOLBARDATA_H
#define MTOOLBARDATA_H

#include <QObject>
#include <QList>
#include <QMap>

#include "minputmethodnamespace.h"
#include "mtoolbaritem.h"

class QDomElement;
struct MTBParseParameters;
struct MTBParseStructure;
class MToolbarRowPrivate;
class MToolbarLayoutPrivate;
class MToolbarDataPrivate;

/*!
 * \brief MToolbarRow defines content of one row in toolbar
 */
class MToolbarRow {
public:
    //! Constructor.
    MToolbarRow();

    //! Destructor.
    virtual ~MToolbarRow();

    //! Append item to row
    void append(QSharedPointer<MToolbarItem> item);

    //! Return all items contained by this row
    QList<QSharedPointer<MToolbarItem> > items() const;

    //! Sort items according to their priorities and alignments
    void sort();

private:
    Q_DISABLE_COPY(MToolbarRow)
    Q_DECLARE_PRIVATE(MToolbarRow)

    MToolbarRowPrivate *const d_ptr;
};

/*!
 * \brief MToolbarLayout defines toolbar layout for specified orientation
 */
class MToolbarLayout {
public:
    //! Creates empty layout object for given \a orientation
    explicit MToolbarLayout(M::Orientation orientation = M::Landscape);

    //! Destructor.
    virtual ~MToolbarLayout();

    //! Append row to layout
    void append(QSharedPointer<MToolbarRow> row);

    //! Return all items contained by this row
    QList<QSharedPointer<const MToolbarRow> > rows() const;

    //! Return all items contained by this row
    QList<QSharedPointer<MToolbarRow> > rows();

    //! Return layout's orientation
    M::Orientation orientation() const;

private:
    Q_DISABLE_COPY(MToolbarLayout)
    Q_DECLARE_PRIVATE(MToolbarLayout)

    MToolbarLayoutPrivate *const d_ptr;
};

/*!
  \brief MToolbarData corresponds to a toolbar defined in a XML file
 */
class MToolbarData : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(MToolbarData)

public:
    /*!
    * \brief Constructor
    */
    MToolbarData();

    /*!
    * \brief Destructor
    */
    ~MToolbarData();

    /*!
     * \brief Load a custom toolbar's content from \a fileName xml file.
     * \a fileName is the xml file name (with ".xml" postfix). And \a fileName could have absolute path.
     * If no absolute path specified, then it will be taken from the default path
     * "/usr/share/meegotouch/imtoolbars/".
     * \param fileName Name of the xml file which contains the content of a custom toolbar.
     */
    bool loadNokiaToolbarXml(const QString &fileName);

    //! Return all items in this toolbar
    QList<QSharedPointer<MToolbarItem> > allItems() const;

    //! Returns pointer to layout for given \a orientation.
    QSharedPointer<const MToolbarLayout> layout(M::Orientation orientation) const;

    /*!
     * \brief Return true if toolbar is locked, so user can not
     * close it manually
     */
    bool locked() const;

    /*!
     * \brief Return true if all conditions are met:
     * 1) toolbar is provided by application,
     * 2) toolbar contains at least one custom item.
     */
    bool isCustom() const;

    /*!
     * \brief Return true if toolbar should be visible.
     */
    bool isVisible() const;

private:
    //! Returns pointer to item for given \a name
    QSharedPointer<MToolbarItem> item(const QString &name);

    //! Sort toolbar items according to their priorities and alignments
    void sort();

    //! Sort layout items according to their priorities and alignments
    void sort(QSharedPointer<MToolbarLayout> layout);

    /*!
     * \brief Return list of standard item names which should
     * not be inserted into this toolbar.
     */
    QStringList refusedNames() const;

    /*!
     * \brief Defines whether this toolbar is custom or standard.
     * \param custom Set this parameter to true if toolbar is custom.
     * \sa isCustom()
     */
    void setCustom(bool custom);

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

    //! Type of tag parser methods
    typedef void (MToolbarData::*TagParser)(const QDomElement &, MTBParseParameters &);

    /*!
     * \brief Helper method for parsing children of an element
     * \param element Element whose children are to be parsed
     * \param params Parsing state
     * \param parserList, a TBParseStructure array.
     * \param parserCount, the number of TBParseStructure in the array.
     */
    void parseChildren(const QDomElement &element, MTBParseParameters &params,
                       const MTBParseStructure *parserList, int parserCount = 1);
protected:
    Q_DECLARE_PRIVATE(MToolbarData)

    MToolbarDataPrivate *const d_ptr;
    friend class MToolbarManager;
    friend struct MTBParseStructure;
    friend class Ut_MToolbarData;
    friend class Ut_MToolbarManager;
};

#endif

