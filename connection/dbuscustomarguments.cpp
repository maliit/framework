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

#include "dbuscustomarguments.h"

#include  <maliit/settingdata.h>

#include <QDBusArgument>


QT_BEGIN_NAMESPACE
QDBusArgument &operator<<(QDBusArgument &arg, const Maliit::PreeditTextFormat &format)
{
    arg.beginStructure();
    arg << format.start
        << format.length
        << static_cast<int>(format.preeditFace);
    arg.endStructure();

    return arg;
}

const QDBusArgument &operator>>(const QDBusArgument &arg, Maliit::PreeditTextFormat &format)
{
    int preedit_face(0);

    arg.beginStructure();
    arg >> format.start
        >> format.length
        >> preedit_face;
    arg.endStructure();
    format.preeditFace = static_cast<Maliit::PreeditFace> (preedit_face);

    return arg;
}
QT_END_NAMESPACE

