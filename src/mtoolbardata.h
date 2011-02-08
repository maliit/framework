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

#include "minputmethodnamespace.h"
#include <MNamespace>

class MToolbarLayout;
class MToolbarItem;

class QDomElement;
struct MTBParseParameters;
struct MTBParseStructure;
class MToolbarDataPrivate;

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
     * "/usr/share/meegoimframework/imtoolbars/".
     * \param fileName Name of the xml file which contains the content of a custom toolbar.
     */
    bool loadToolbarXml(const QString &fileName);

    //! Return all items in this toolbar
    QList<QSharedPointer<MToolbarItem> > items() const;

    //! Returns pointer to layout for given \a orientation.
    QSharedPointer<const MToolbarLayout> layout(M::Orientation orientation) const;

    /*!
     * \brief Return true if toolbar should be visible.
     */
    bool isVisible() const;

private:
    //! Append given \a item to the given layout.
    //! \param toolbarLayout should point to lanscape or portrait layout of this toolbar.
    //! \return Return false if item could not be added.
    //!
    //! \sa layout(M::Orientation)
    bool append(const QSharedPointer<MToolbarLayout> &toolbarLayout,
                const QSharedPointer<MToolbarItem> &item);

    //! Returns pointer to item for given \a name
    QSharedPointer<MToolbarItem> item(const QString &name) const;

    /*!
     * \brief Return list of standard item names which should
     * not be inserted into this toolbar.
     */
    QStringList refusedNames() const;

protected:
    Q_DECLARE_PRIVATE(MToolbarData)

    MToolbarDataPrivate *const d_ptr;
    friend class MAttributeExtensionManager;
    friend struct MTBParseStructure;
    friend class Ut_MToolbarData;
    friend class Ut_MAttributeExtensionManager;
};

#endif

