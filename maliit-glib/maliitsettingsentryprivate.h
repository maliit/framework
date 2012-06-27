/* This file is part of Maliit framework
 *
 * Copyright (C) 2012 Openismus GmbH
 *
 * Contact: maliit-discuss@lists.maliit.org
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

#ifndef MALIIT_GLIB_SETTINGS_ENTRY_PRIVATE_H
#define MALIIT_GLIB_SETTINGS_ENTRY_PRIVATE_H

#include "maliitattributeextension.h"
#include "maliitsettingsentry.h"

G_BEGIN_DECLS

MaliitSettingsEntry *
maliit_settings_entry_new_from_dbus_data (GValueArray *info,
                                          MaliitAttributeExtension *extension);

G_END_DECLS

#endif /* MALIIT_GLIB_SETTINGS_ENTRY_PRIVATE_H */
