/* This file is part of Maliit framework
 *
 * Copyright (C) 2012 Canonical Ltd
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

#ifndef MALIIT_GLIB_SETTING_DATA_H
#define MALIIT_GLIB_SETTING_DATA_H

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

typedef enum
{
    MALIIT_STRING_TYPE      = 1,
    MALIIT_INT_TYPE         = 2,
    MALIIT_BOOL_TYPE        = 3,
    MALIIT_STRING_LIST_TYPE = 4,
    MALIIT_INT_LIST_TYPE    = 5
} MaliitSettingsEntryType;

/**
 * MALIIT_SETTING_VALUE_DOMAIN:
 *
 * Name of setting entry attribute which holds the list of values that
 * can be assigned to the entry.
 */
#define MALIIT_SETTING_VALUE_DOMAIN "valueDomain"

/**
 * MALIIT_SETTING_VALUE_DOMAIN_DESCRIPTIONS:
 *
 * Name of setting entry attribute which holds the descriptions for
 * the values in MALIIT_SETTING_VALUE_DOMAIN().
 */
#define MALIIT_SETTING_VALUE_DOMAIN_DESCRIPTIONS "valueDomainDescriptions"

/**
 * MALIIT_SETTING_VALUE_RANGE_MIN:
 *
 * Name of setting entry attribute which holds the minimum valid value
 * (inclusive) for an integer property.
 */
#define MALIIT_SETTING_VALUE_RANGE_MIN "valueRangeMin"
/**
 * MALIIT_SETTING_VALUE_RANGE_MAX:
 *
 * Name of setting entry attribute which holds the maximum valid value
 * (inclusive) for an integer property.
 */
#define MALIIT_SETTING_VALUE_RANGE_MAX "valueRangeMax"

/**
 * MALIIT_SETTING_DEFAULT_VALUE:
 *
 * Name of setting entry attribute which holds the default value for a setting entry.
 */
#define MALIIT_SETTING_DEFAULT_VALUE "defaultValue"

#define MALIIT_TYPE_SETTINGS_ENTRY_TYPE maliit_settings_entry_type_get_type ()
GType maliit_settings_entry_type_get_type (void) G_GNUC_CONST;

gboolean
maliit_validate_setting_value(MaliitSettingsEntryType type,
                              GHashTable *attributes,
                              GVariant *value);

G_END_DECLS

#endif /* MALIIT_GLIB_SETTING_DATA_H */
