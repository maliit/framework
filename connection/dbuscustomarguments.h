/* * This file is part of Maliit framework *
 *
 * Copyright (C) 2012 Mattia Barbon <mattia@develer.com>
 *
 * Contact: maliit-discuss@lists.maliit.org
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 */

#ifndef DBUSCUSTOMARGUMENTS_H
#define DBUSCUSTOMARGUMENTS_H

class MImPluginSettingsEntry;
class MImPluginSettingsInfo;

#include <maliit/namespace.h>

#include <QtCore/qnamespace.h>

QT_BEGIN_NAMESPACE
class QDBusArgument;
class QVariant;
// MImPluginSettingsEntry marshalling
QDBusArgument &operator<<(QDBusArgument &argument, const MImPluginSettingsEntry &entry);
const QDBusArgument &operator>>(const QDBusArgument &argument, MImPluginSettingsEntry &entry);

// MImPluginSettingsInfo marshalling
QDBusArgument &operator<<(QDBusArgument &argument, const MImPluginSettingsInfo &info);
const QDBusArgument &operator>>(const QDBusArgument &argument, MImPluginSettingsInfo &info);

QDBusArgument &operator<<(QDBusArgument &arg, const Maliit::PreeditTextFormat &format);
const QDBusArgument &operator>>(const QDBusArgument &arg, Maliit::PreeditTextFormat &format);
QT_END_NAMESPACE

#endif // DBUSCUSTOMARGUMENTS_H
