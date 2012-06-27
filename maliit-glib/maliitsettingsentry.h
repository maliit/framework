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

#ifndef MALIIT_GLIB_SETTINGS_ENTRY_H
#define MALIIT_GLIB_SETTINGS_ENTRY_H

#include <glib.h>
#include <glib-object.h>

#include "maliitsettingdata.h"

G_BEGIN_DECLS

#define MALIIT_TYPE_SETTINGS_ENTRY           (maliit_settings_entry_get_type())
#define MALIIT_SETTINGS_ENTRY(obj)           (G_TYPE_CHECK_INSTANCE_CAST(obj, MALIIT_TYPE_SETTINGS_ENTRY, MaliitSettingsEntry))
#define MALIIT_SETTINGS_ENTRY_CLASS(cls)     (G_TYPE_CHECK_CLASS_CAST(cls, MALIIT_TYPE_SETTINGS_ENTRY, MaliitSettingsEntryClass))
#define MALIIT_IS_SETTINGS_ENTRY(obj)        (G_TYPE_CHECK_INSTANCE_TYPE(obj, MALIIT_TYPE_SETTINGS_ENTRY))
#define MALIIT_IS_SETTINGS_ENTRY_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE(obj, MALIIT_TYPE_SETTINGS_ENTRY))
#define MALIIT_SETTINGS_ENTRY_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), MALIIT_TYPE_SETTINGS_ENTRY, MaliitSettingsEntryClass))

typedef struct _MaliitSettingsEntry MaliitSettingsEntry;
typedef struct _MaliitSettingsEntryClass MaliitSettingsEntryClass;
typedef struct _MaliitSettingsEntryPrivate MaliitSettingsEntryPrivate;

struct _MaliitSettingsEntry
{
    GObject parent;

    /*< private >*/
    MaliitSettingsEntryPrivate *priv;
};

/**
 * MaliitSettingsEntryClass:
 * @parent_class: The parent class.
 */
struct _MaliitSettingsEntryClass
{
    GObjectClass parent_class;
};

GType
maliit_settings_entry_get_type (void) G_GNUC_CONST;

const gchar *
maliit_settings_entry_get_description (MaliitSettingsEntry *entry);

const gchar *
maliit_settings_entry_get_key (MaliitSettingsEntry *entry);

MaliitSettingsEntryType
maliit_settings_entry_get_entry_type (MaliitSettingsEntry *entry);

gboolean
maliit_settings_entry_is_current_value_valid (MaliitSettingsEntry *entry);

GVariant *
maliit_settings_entry_get_value (MaliitSettingsEntry *entry);

void
maliit_settings_entry_set_value (MaliitSettingsEntry *entry,
                                 GVariant *value);

gboolean
maliit_settings_entry_is_value_valid (MaliitSettingsEntry *entry,
                                      GVariant *value);

GHashTable *
maliit_settings_entry_get_attributes (MaliitSettingsEntry *entry);

G_END_DECLS

#endif /* MALIIT_GLIB_SETTINGS_ENTRY_H */
