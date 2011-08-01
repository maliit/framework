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

#ifndef MIABSTRACTPLUGINFACTORY_H
#define MIABSTRACTPLUGINFACTORY_H

#include <QtCore>
#include <QtPlugin>

class MInputMethodPlugin;
class MImAbstractPluginFactoryPrivate;

/*!
 * \brief MImAbstractPluginFactory is an MInputMethodPlugin factory used for dynamic languages
 *
 * To create a factory, re-implement the virtual functions this functions will be used to create
 * a new instacen of MInputMethodPlugin based on file mime-type
 */
class MImAbstractPluginFactory
{
    Q_DISABLE_COPY(MImAbstractPluginFactory)
    Q_DECLARE_PRIVATE(MImAbstractPluginFactory)

private:
    const QScopedPointer<MImAbstractPluginFactoryPrivate> d_ptr;

public:
    explicit MImAbstractPluginFactory();
    virtual ~MImAbstractPluginFactory() = 0;

    //! \brief Implement this function to return the extension of which kind of file this factory can handle
    virtual QString fileExtension() const = 0;

    //! \brief Implement this function to create a new MInputMethodPlugin base on a file
    virtual MInputMethodPlugin* create(const QString &file) const = 0;
};


Q_DECLARE_INTERFACE(MImAbstractPluginFactory,
                    "org.maliit.maliit-plugins.MImAbstractPluginFactory/0.80")

#endif
