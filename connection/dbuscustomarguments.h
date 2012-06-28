/* * This file is part of maliit-framework *
 *
 * Copyright (C) 2012 Mattia Barbon <mattia@develer.com>
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

#ifndef DBUSCUSTOMARGUMENTS_H
#define DBUSCUSTOMARGUMENTS_H

class MImPluginSettingsEntry;
class MImPluginSettingsInfo;
class QDBusArgument;
class QVariant;


// MImPluginSettingsEntry marshalling
QDBusArgument &operator<<(QDBusArgument &argument, const MImPluginSettingsEntry &entry);
const QDBusArgument &operator>>(const QDBusArgument &argument, MImPluginSettingsEntry &entry);

// MImPluginSettingsInfo marshalling
QDBusArgument &operator<<(QDBusArgument &argument, const MImPluginSettingsInfo &info);
const QDBusArgument &operator>>(const QDBusArgument &argument, MImPluginSettingsInfo &info);

#endif // DBUSCUSTOMARGUMENTS_H
