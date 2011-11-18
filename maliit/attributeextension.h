/* * This file is part of meego-im-framework *
 *
 * Copyright (C) 2010, 2011 Nokia Corporation and/or its subsidiary(-ies).
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

// Based on minputmethodstate.h from libmeegotouch

#ifndef MALIIT_ATTRIBUTEEXTENSION_H
#define MALIIT_ATTRIBUTEEXTENSION_H

#include <QMap>
#include <QObject>
#include <QScopedPointer>
#include <QString>
#include <QVariant>

namespace Maliit {

class AttributeExtensionPrivate;

/*! \ingroup libmaliit
 * \brief Register extensions such as key overrides.
 */
class AttributeExtension : public QObject
{
    Q_OBJECT

public:
    /*!
     * \brief Registers an attribute extension (toolbar contents, keyboard overrides) which is defined in \a fileName, and will be assigned to a text input widget (software input panel).
     * \param \a fileName is the file name of the attribute extension. The filename should be absolute. If the filename is relative, it
     *      will be looked up in the default directory provided by the framework.
     * \note if \a fileName is an empty string, then it means don't load any content, just get a new id.
     * \return the registered unique identifier of this attribute extension.
     */
    explicit AttributeExtension(const QString &fileName = QString());
    virtual ~AttributeExtension();

    // providing target name -> (item name -> (attribute name -> attribute value))
    typedef QMap<QString, QVariant> ExtendedAttributeMap;

    // returns all the registered extended attributes.
    ExtendedAttributeMap attributes() const;

    /*!
     * \brief Returns the attribute extension definition file name.
     * \return the file name of the attribute extension, or empty QString.
     */
    QString fileName() const;

    /*!
     * \brief Returns an unique identifier id for the attribute extension.
     * \return the unique identifier id.
     */
    int id() const;

public Q_SLOTS:
    /*!
     * \brief Set the \a attribute of the \a targetItem in the input method registered attribute extension \a target to \a value.
     * \param key a string specifying the target for the attribute.
     * \param value new value.
     *
     * \note
     *  Every key begins with a slash character ('/') and is of the form /target/item/attribute
     *  Target "keys" is used for virtual keyboard keys, "toolbar" for toolbar contents.
     *  Additional targets can be added in the future.
     */
    void setAttribute(const QString &key, const QVariant &value);

private:
    Q_DISABLE_COPY(AttributeExtension)

    const QScopedPointer<AttributeExtensionPrivate> d_ptr;

    Q_DECLARE_PRIVATE(AttributeExtension)
};

} // namespace Maliit

#endif // MALIIT_ATTRIBUTEEXTENSION_H
