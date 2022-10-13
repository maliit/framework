/* * This file is part of Maliit framework *
 *
 * Copyright (C) 2012 Mattia Barbon <mattia@develer.com>
 *
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 */

#ifndef DBUSCUSTOMARGUMENTS_H
#define DBUSCUSTOMARGUMENTS_H

#include <maliit/namespace.h>

#include <QtCore/qnamespace.h>

QT_BEGIN_NAMESPACE
class QDBusArgument;
class QVariant;

QDBusArgument &operator<<(QDBusArgument &arg, const Maliit::PreeditTextFormat &format);
const QDBusArgument &operator>>(const QDBusArgument &arg, Maliit::PreeditTextFormat &format);
QT_END_NAMESPACE

#endif // DBUSCUSTOMARGUMENTS_H
