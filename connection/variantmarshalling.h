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

#ifndef VARIANTMARSHALLING_H
#define VARIANTMARSHALLING_H

#include <glib.h>
#include <dbus/dbus-glib-lowlevel.h>
#include <dbus/dbus-glib.h>

class QVariant;
class QString;

/*!
 * \brief Convert data from GValue into QVariant.
 * \param[out] dest Variable to get result to.
 * \param[in] source Original value.
 * \param[out] error_message Contains error description if original value could not be decoded.
 */
bool decodeVariant(QVariant *dest, GValue *source, QString *error_message);


/*!
 * \brief Convert data from QVariant into GValue.
 * \param[out] dest Variable to get result to.
 * \param[in] source Original value.
 */
bool encodeVariant(GValue *dest, const QVariant &source);

#endif
