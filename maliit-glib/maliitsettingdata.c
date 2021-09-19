/* This file is part of Maliit framework
 *
 * Copyright (C) 2012 Canonical Ltd
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

#include "maliitsettingdata.h"

/**
 * SECTION:maliitsettingdata
 * @short_description: miscellaneous setting functions and types
 * @title: Maliit settings misc
 * @see_also: #MaliitSettingsEntry, #MaliitPluginSettings
 * @stability: Stable
 * @include: maliit/maliitsettingdata.h
 *
 * Here #MaliitSettingsEntryType, attribute keys and
 * maliit_validate_setting_value() are specified.
 */


/**
 * MaliitSettingsEntryType:
 * @MALIIT_STRING_TYPE: Entry is a string.
 * @MALIIT_INT_TYPE: Entry is an integer.
 * @MALIIT_BOOL_TYPE: Entry is a boolean.
 * @MALIIT_STRING_LIST_TYPE: Entry is a list of strings.
 * @MALIIT_INT_LIST_TYPE: Entry is a list of integers.
 *
 * This enum describes type of settings entry.
 */
static const GEnumValue maliit_settings_entry_type_values[] = {
    { MALIIT_STRING_TYPE, "MALIIT_STRING_TYPE", "string-type" },
    { MALIIT_INT_TYPE, "MALIIT_INT_TYPE", "int-type" },
    { MALIIT_BOOL_TYPE, "MALIIT_BOOL_TYPE", "bool-type" },
    { MALIIT_STRING_LIST_TYPE, "MALIIT_STRING_LIST_TYPE", "string-list-type" },
    { MALIIT_INT_LIST_TYPE, "MALIIT_INT_LIST_TYPE", "int-list-type" },
    { 0, NULL, NULL }
};

GType
maliit_settings_entry_type_get_type (void)
{
    static GType type = G_TYPE_INVALID;

    if (type == G_TYPE_INVALID) {
        type = g_enum_register_static ("MaliitSettingsEntryType",
                                       maliit_settings_entry_type_values);
    }

    return type;
}

static gboolean
check_single_value_domain(GVariant *value,
                          GVariant *domain)
{
  gsize array_len = g_variant_n_children(domain);
  gboolean equal = FALSE;
  gsize iter;

  for (iter = 0; iter < array_len && !equal; ++iter) {
    GVariant *child = g_variant_get_child_value(domain, iter);

    equal = g_variant_equal(value, child);
    g_variant_unref(child);
  }
  return equal;
}

static gboolean
check_variant_value_domain(GVariant *value,
                           GVariant *domain)
{
  if (!domain) {
    return TRUE;
  }

  if (!g_variant_is_of_type(domain, G_VARIANT_TYPE_ARRAY)) {
    return FALSE;
  }

  return check_single_value_domain(value, domain);
}

static gboolean
check_variant_array_domain(GVariant *values, GVariant *domain)
{
  gsize iter;
  gsize array_len;
  gboolean correct;

  if (!domain) {
    return TRUE;
  }

  if (!g_variant_is_of_type(domain, G_VARIANT_TYPE_ARRAY)) {
    return FALSE;
  }

  array_len = g_variant_n_children(values);
  correct = TRUE;
  for (iter = 0; iter < array_len && correct; ++iter) {
    GVariant *child = g_variant_get_child_value(values, iter);

    correct = check_single_value_domain(child, domain);
    g_variant_unref (child);
  }
  return correct;
}

static gboolean
check_single_value_range(GVariant *value,
                         GVariant *range_min,
                         GVariant *range_max)
{
  if (range_min) {
    if (!g_variant_is_of_type(range_min, G_VARIANT_TYPE_INT32) ||
        g_variant_get_int32(range_min) > g_variant_get_int32(value)) {
      return FALSE;
    }
  }

  if (range_max) {
    if (!g_variant_is_of_type(range_max, G_VARIANT_TYPE_INT32) ||
        g_variant_get_int32(range_max) < g_variant_get_int32(value)) {
      return FALSE;
    }
  }

  return TRUE;
}

static gboolean
check_variant_value_range(GVariant *value,
                          GVariant *range_min,
                          GVariant *range_max)
{
  if (!range_min && !range_max) {
    return TRUE;
  }

  return check_single_value_range(value, range_min, range_max);
}

static gboolean
check_variant_array_range(GVariant *values,
                          GVariant *range_min,
                          GVariant *range_max)
{
  gsize iter;
  gsize values_len;
  gboolean correct;

  if (!range_min && !range_max) {
    return TRUE;
  }

  correct = TRUE;
  values_len = g_variant_n_children(values);
  for (iter = 0; iter < values_len && correct; ++iter) {
    GVariant *child = g_variant_get_child_value(values, iter);

    correct = check_single_value_range(child, range_min, range_max);
    g_variant_unref (child);
  }

  return correct;
}

static gboolean
check_variant_array_is_int_array(GVariant *values)
{
  gsize array_len = g_variant_n_children(values);
  gsize iter;
  gboolean correct = TRUE;

  for (iter = 0; iter < array_len && correct; ++iter) {
    GVariant *child = g_variant_get_child_value(values, iter);

    correct = g_variant_is_of_type(child, G_VARIANT_TYPE_INT32);
    g_variant_unref(child);
  }
  return correct;
}

/**
 * maliit_validate_setting_value:
 * @type: a #MaliitSettingsEntryType to validate against.
 * @attributes: (transfer none) (element-type utf8 GLib.Variant): Attributes to validate.
 * @value: (transfer none): A #GVariant to validate.
 *
 * Validate the value for a plugin setting entry.
 *
 * Returns: %TRUE if @value and @attributes are valid for given @type.
 */
gboolean
maliit_validate_setting_value(MaliitSettingsEntryType  type,
                              GHashTable             *attributes,
                              GVariant               *value)
{
    GVariant *domain = g_hash_table_lookup(attributes,
                                           MALIIT_SETTING_VALUE_DOMAIN);
    GVariant *range_min = g_hash_table_lookup(attributes,
                                              MALIIT_SETTING_VALUE_RANGE_MIN);
    GVariant *range_max = g_hash_table_lookup(attributes,
                                              MALIIT_SETTING_VALUE_RANGE_MAX);

    switch (type)
    {
    case MALIIT_STRING_TYPE:
        return (g_variant_is_of_type(value, G_VARIANT_TYPE_STRING) &&
                check_variant_value_domain(value, domain));

    case MALIIT_INT_TYPE:
      return (g_variant_is_of_type(value, G_VARIANT_TYPE_INT32) &&
              check_variant_value_domain(value, domain) &&
              check_variant_value_range(value, range_min, range_max));

    case MALIIT_BOOL_TYPE:
      return g_variant_is_of_type(value, G_VARIANT_TYPE_BOOLEAN);

    case MALIIT_STRING_LIST_TYPE:
      return (g_variant_is_of_type(value, G_VARIANT_TYPE_STRING_ARRAY) &&
              check_variant_array_domain(value, domain));

    case MALIIT_INT_LIST_TYPE:
      return (g_variant_is_of_type(value, G_VARIANT_TYPE_ARRAY) &&
              check_variant_array_is_int_array(value) &&
              check_variant_array_domain(value, domain) &&
              check_variant_array_range(value, range_min, range_max));

    default:
      return FALSE;
    }
}
