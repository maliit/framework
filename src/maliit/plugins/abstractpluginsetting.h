/* This file is part of Maliit framework
 *
 * Copyright (C) 2012 Mattia Barbon <mattia@develer.com>
 *
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the licence, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef MALIIT_PLUGIN_SETTINGS_H
#define MALIIT_PLUGIN_SETTINGS_H

#include <QObject>

namespace Maliit {
namespace Plugins {

/*! \ingroup maliitserver
 * \brief A generic interface to access plugin configuration values
 */
class AbstractPluginSetting : public QObject
{
    Q_OBJECT

public:
    //! Returns the key of this item, as given to MAbstractInputMethodHost::registerPluginSetting()
    virtual QString key() const = 0;

    //! Returns the current value of this item, as a QVariant.
    virtual QVariant value() const = 0;

    /*! Returns the current value of this item, as a QVariant.  If
     *  there is no value for this item, return \a def instead.
     */
    virtual QVariant value(const QVariant &def) const = 0;

    /*! Set the value of this item to \a val.  If string \a val fails
        for any reason, the current value is not changed and nothing happens.
    */
    virtual void set(const QVariant &val) = 0;

    /*! Unset this item.  This is equivalent to

        \code
        item.set(QVariant(QVariant::Invalid));
        \endcode
     */
    virtual void unset() = 0;

    //! Emitted when the value of this item has changed.
    Q_SIGNAL void valueChanged();
};

}
}

#endif
