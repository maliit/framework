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

#include "maliitpluginsettingsprivate.h"
#include "maliitsettingsentryprivate.h"

/**
 * SECTION:maliitsettingsentry
 * @short_description: settings entry
 * @title: MaliitSettingsEntry
 * @see_also: #MaliitSettingsEntry, #MaliitPluginSettings
 * @stability: Stable
 * @include: maliit/maliitsettingsentry.h
 *
 * The #MaliitSettingsEntry is a class holding single plugin
 * setting. It can be one of several available types
 * (#MaliitSettingsEntryType). It can also have some attributes like
 * value domain (MALIIT_SETTING_VALUE_DOMAIN()), default value
 * (MALIIT_SETTING_DEFAULT_VALUE) and value ranges
 * (MALIIT_SETTING_VALUE_RANGE_MIN() and
 * MALIIT_SETTING_VALUE_RANGE_MAX()).
 */

struct _MaliitSettingsEntryPrivate
{
    MaliitAttributeExtension *extension;
    gchar *description;
    gchar *extension_key;
    MaliitSettingsEntryType type;
    gboolean valid;
    GHashTable *attributes;

    guint extension_signal_id;
};

G_DEFINE_TYPE_WITH_CODE (MaliitSettingsEntry, maliit_settings_entry,
                         G_TYPE_OBJECT, G_ADD_PRIVATE(MaliitSettingsEntry))

enum
{
    PROP_0,

    PROP_EXTENSION,
    PROP_DESCRIPTION,
    PROP_EXTENSION_KEY,
    PROP_TYPE,
    PROP_VALID,
    PROP_VALUE,
    PROP_ATTRIBUTES
};

enum
{
    VALUE_CHANGED,

    LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

static void
maliit_settings_entry_finalize (GObject *object)
{
    MaliitSettingsEntry *entry = MALIIT_SETTINGS_ENTRY (object);
    MaliitSettingsEntryPrivate *priv = entry->priv;

    g_free (priv->description);
    g_free (priv->extension_key);

    G_OBJECT_CLASS (maliit_settings_entry_parent_class)->finalize (object);
}

static void
maliit_settings_entry_dispose (GObject *object)
{
    MaliitSettingsEntry *entry = MALIIT_SETTINGS_ENTRY (object);
    MaliitSettingsEntryPrivate *priv = entry->priv;

    if (priv->extension_signal_id) {
        if (priv->extension) {
            g_signal_handler_disconnect (priv->extension,
                                         priv->extension_signal_id);
        }
        priv->extension_signal_id = 0;
    }
    g_clear_object (&priv->extension);
    if (priv->attributes) {
        GHashTable *attributes = priv->attributes;

        priv->attributes = NULL;
        g_hash_table_unref (attributes);
    }

    G_OBJECT_CLASS (maliit_settings_entry_parent_class)->dispose (object);
}

static void
maliit_settings_entry_set_property (GObject *object,
                                    guint prop_id,
                                    const GValue *value,
                                    GParamSpec *pspec)
{
    MaliitSettingsEntry *entry = MALIIT_SETTINGS_ENTRY (object);
    MaliitSettingsEntryPrivate *priv = entry->priv;

    switch (prop_id) {
    case PROP_EXTENSION:
        if (priv->extension) {
            g_object_unref (priv->extension);
        }
        priv->extension = g_value_dup_object (value);
        break;
    case PROP_DESCRIPTION:
        g_free (priv->description);
        priv->description = g_value_dup_string (value);
        break;
    case PROP_EXTENSION_KEY:
        g_free (priv->extension_key);
        priv->extension_key = g_value_dup_string (value);
        break;
    case PROP_TYPE:
        priv->type = g_value_get_enum (value);
        break;
    case PROP_VALID:
        priv->valid = g_value_get_boolean (value);
        break;
    case PROP_VALUE:
        maliit_settings_entry_set_value (entry, g_value_get_variant (value));
        break;
    case PROP_ATTRIBUTES:
        if (priv->attributes) {
            g_hash_table_unref (priv->attributes);
        }
        priv->attributes = g_value_dup_boxed (value);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
        break;
    }
}

static void
maliit_settings_entry_get_property (GObject *object,
                                     guint prop_id,
                                     GValue *value,
                                     GParamSpec *pspec)
{
    MaliitSettingsEntry *entry = MALIIT_SETTINGS_ENTRY (object);
    MaliitSettingsEntryPrivate *priv = entry->priv;

    switch (prop_id) {
        /* PROP_EXTENSION is write only, construction only - omitted here */
    case PROP_DESCRIPTION:
        g_value_set_string (value, priv->description);
        break;
    case PROP_EXTENSION_KEY:
        g_value_set_string (value, priv->extension_key);
        break;
    case PROP_TYPE:
        g_value_set_enum (value, priv->type);
        break;
    case PROP_VALID:
        g_value_set_boolean (value, priv->valid);
        break;
    case PROP_VALUE:
        g_value_set_variant (value,
                             maliit_settings_entry_get_value (entry));
        break;
    case PROP_ATTRIBUTES:
        g_value_set_boxed (value, priv->attributes);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
        break;
    }
}

static void
value_changed(MaliitSettingsEntry *entry,
              const gchar *key,
              GVariant *value G_GNUC_UNUSED,
              gpointer user_data G_GNUC_UNUSED)
{
    if (!g_strcmp0 (key, entry->priv->extension_key)) {
        g_signal_emit(entry, signals[VALUE_CHANGED], 0);
    }
}

static void
maliit_settings_entry_constructed (GObject *object)
{
    MaliitSettingsEntry *entry = MALIIT_SETTINGS_ENTRY (object);
    MaliitSettingsEntryPrivate *priv = entry->priv;

    if (priv->extension) {
        priv->extension_signal_id = g_signal_connect_swapped (priv->extension,
                                                              "extended-attribute-changed",
                                                              G_CALLBACK (value_changed),
                                                              entry);
    }
}

static void
maliit_settings_entry_class_init (MaliitSettingsEntryClass *entry_class)
{
    GObjectClass *g_object_class = G_OBJECT_CLASS (entry_class);

    g_object_class->finalize = maliit_settings_entry_finalize;
    g_object_class->dispose = maliit_settings_entry_dispose;
    g_object_class->set_property = maliit_settings_entry_set_property;
    g_object_class->get_property = maliit_settings_entry_get_property;
    g_object_class->constructed = maliit_settings_entry_constructed;

    /**
     * MaliitSettingsEntry:extension:
     *
     * #MaliitAttributeExtension used by this entry.
     */
    g_object_class_install_property (g_object_class,
                                     PROP_EXTENSION,
                                     g_param_spec_object ("extension",
                                                          "Extension", /* TODO: mark as translatable? */
                                                          "Extension used by this entry", /* TODO: mark as translatable? */
                                                          MALIIT_TYPE_ATTRIBUTE_EXTENSION,
                                                          G_PARAM_WRITABLE |
                                                          G_PARAM_CONSTRUCT_ONLY |
                                                          G_PARAM_STATIC_NAME |
                                                          G_PARAM_STATIC_BLURB |
                                                          G_PARAM_STATIC_NICK));

    /**
     * MaliitSettingsEntry:description:
     *
     * Description of the entry.
     */
    g_object_class_install_property (g_object_class,
                                     PROP_DESCRIPTION,
                                     g_param_spec_string ("description",
                                                          "Description", /* TODO: mark as translatable? */
                                                          "Description of the entry", /* TODO: mark as translatable? */
                                                          "",
                                                          G_PARAM_READABLE |
                                                          G_PARAM_WRITABLE |
                                                          G_PARAM_CONSTRUCT_ONLY |
                                                          G_PARAM_STATIC_NAME |
                                                          G_PARAM_STATIC_BLURB |
                                                          G_PARAM_STATIC_NICK));

    /**
     * MaliitSettingsEntry:extension-key:
     *
     * Key of the entry.
     */
    g_object_class_install_property (g_object_class,
                                     PROP_EXTENSION_KEY,
                                     g_param_spec_string ("extension-key",
                                                          "Extension key", /* TODO: mark as translatable? */
                                                          "Key of the entry.", /* TODO: mark as translatable? */
                                                          "",
                                                          G_PARAM_READABLE |
                                                          G_PARAM_WRITABLE |
                                                          G_PARAM_CONSTRUCT_ONLY |
                                                          G_PARAM_STATIC_NAME |
                                                          G_PARAM_STATIC_BLURB |
                                                          G_PARAM_STATIC_NICK));

    /**
     * MaliitSettingsEntry:type:
     *
     * Type of the entry
     */
    g_object_class_install_property (g_object_class,
                                     PROP_TYPE,
                                     g_param_spec_enum ("type",
                                                        "Type", /* TODO: mark as translatable? */
                                                        "Type if the entry", /* TODO: mark as translatable? */
                                                        MALIIT_TYPE_SETTINGS_ENTRY_TYPE,
                                                        MALIIT_STRING_TYPE,
                                                        G_PARAM_READABLE |
                                                        G_PARAM_WRITABLE |
                                                        G_PARAM_CONSTRUCT_ONLY |
                                                        G_PARAM_STATIC_NAME |
                                                        G_PARAM_STATIC_BLURB |
                                                        G_PARAM_STATIC_NICK));

    /**
     * MaliitSettingsEntry:valid:
     *
     * Whether entry's value is valid.
     */
    g_object_class_install_property (g_object_class,
                                     PROP_VALID,
                                     g_param_spec_boolean ("valid",
                                                           "Valid", /* TODO: mark as translatable? */
                                                           "Whether entry's value is valid", /* TODO: mark as translatable? */
                                                           FALSE,
                                                           G_PARAM_READABLE |
                                                           G_PARAM_WRITABLE |
                                                           G_PARAM_CONSTRUCT_ONLY |
                                                           G_PARAM_STATIC_NAME |
                                                           G_PARAM_STATIC_BLURB |
                                                           G_PARAM_STATIC_NICK));

    /**
     * MaliitSettingsEntry:value:
     *
     * Value of the entry.
     */
    g_object_class_install_property (g_object_class,
                                     PROP_VALUE,
                                     g_param_spec_variant ("value",
                                                           "Value", /* TODO: mark as translatable? */
                                                           "Value of the entry", /* TODO: mark as translatable? */
                                                           G_VARIANT_TYPE_ANY,
                                                           g_variant_new_int32 (0),
                                                           G_PARAM_READABLE |
                                                           G_PARAM_WRITABLE |
                                                           G_PARAM_CONSTRUCT_ONLY |
                                                           G_PARAM_STATIC_NAME |
                                                           G_PARAM_STATIC_BLURB |
                                                           G_PARAM_STATIC_NICK));

    /**
     * MaliitSettingsEntry:attributes:
     *
     * Attributes of the entry.
     */
    g_object_class_install_property (g_object_class,
                                     PROP_ATTRIBUTES,
                                     g_param_spec_boxed ("attributes",
                                                         "Attributes", /* TODO: mark as translatable? */
                                                         "Attributes of the entry", /* TODO: mark as translatable? */
                                                         G_TYPE_HASH_TABLE,
                                                         G_PARAM_READABLE |
                                                         G_PARAM_WRITABLE |
                                                         G_PARAM_CONSTRUCT_ONLY |
                                                         G_PARAM_STATIC_NAME |
                                                         G_PARAM_STATIC_BLURB |
                                                         G_PARAM_STATIC_NICK));

    /**
     * MaliitSettingsEntry::value-changed:
     * @entry: The #MaliitSettingsEntry emitting the signal.
     *
     * Emitted when value of the entry was changed in the plugin.
     */
    signals[VALUE_CHANGED] =
        g_signal_new ("value-changed",
                      MALIIT_TYPE_SETTINGS_ENTRY,
                      G_SIGNAL_RUN_FIRST,
                      0,
                      NULL,
                      NULL,
                      g_cclosure_marshal_VOID__VOID,
                      G_TYPE_NONE,
                      0);
}

static void
maliit_settings_entry_init (MaliitSettingsEntry *entry)
{
    MaliitSettingsEntryPrivate *priv = maliit_settings_entry_get_instance_private (entry);

    priv->extension = NULL;
    priv->description = NULL;
    priv->extension_key = NULL;
    priv->type = MALIIT_STRING_TYPE;
    priv->valid = FALSE;
    priv->attributes = NULL;
    priv->extension_signal_id = 0;

    entry->priv = priv;
}

static GHashTable *
attributes_from_dbus_g_variant (GVariant *dbus_attributes)
{
    GHashTable *attributes = g_hash_table_new_full (g_str_hash,
                                                    g_str_equal,
                                                    g_free,
                                                    (GDestroyNotify)g_variant_unref);
    GVariantIter iter;
    gchar *key;
    GVariant *value;

    g_variant_iter_init (&iter, dbus_attributes);
    while (g_variant_iter_next (&iter, "{sv}", &key, &value)) {
        g_hash_table_replace (attributes, key, value);
    }

    return attributes;
}

/**
 * maliit_settings_entry_new_from_dbus_data: (skip)
 * @info: (transfer none): A #GVariant of DBus provenance containing entry information.
 * @extension: (transfer none): A #MaliitAttributeExtensions for #MaliitAttributeSettingsEntry instance.
 *
 * Creates new settings entry. This is used internally only by
 * #MaliitPluginSettings.
 *
 * Returns: (transfer full): The newly created #MaliitSettingsEntry.
 */
MaliitSettingsEntry *
maliit_settings_entry_new_from_dbus_data (GVariant                 *info,
                                          MaliitAttributeExtension *extension)
{
    const gchar *description;
    const gchar *extension_key;
    MaliitSettingsEntryType type;
    gboolean valid;
    GVariant *value;
    GVariant *attrs;
    GHashTable *attributes;
    MaliitSettingsEntry *entry;

    g_return_val_if_fail (MALIIT_IS_ATTRIBUTE_EXTENSION (extension), NULL);

    g_variant_get (info, "(&s&sib@v@a{sv})", &description, &extension_key, &type, &valid, &value, &attrs);

    attributes = attributes_from_dbus_g_variant (attrs);
    entry = MALIIT_SETTINGS_ENTRY (g_object_new (MALIIT_TYPE_SETTINGS_ENTRY,
                                                 "extension", extension,
                                                 "description", description,
                                                 "extension-key", extension_key,
                                                 "type", type,
                                                 "valid", valid,
                                                 "value", value,
                                                 "attributes", attributes,
                                                 NULL));

    g_hash_table_unref (attributes);
    g_variant_unref (attrs);
    g_variant_unref (value);
    return entry;
}

/**
 * maliit_settings_entry_get_description:
 * @entry: (transfer none): The #MaliitSettingsEntry.
 *
 * Gets description of the entry.
 *
 * Returns: (transfer none): A description. Returned value should not be modified nor freed.
 */
const gchar *
maliit_settings_entry_get_description (MaliitSettingsEntry *entry)
{
    g_return_val_if_fail (MALIIT_IS_SETTINGS_ENTRY (entry), NULL);

    return entry->priv->description;
}

/**
 * maliit_settings_entry_get_key:
 * @entry: (transfer none): The #MaliitSettingsEntry.
 *
 * Gets key of the entry.
 *
 * Returns: (transfer none): A key. Returned value should not be modified nor freed.
 */
const gchar *
maliit_settings_entry_get_key (MaliitSettingsEntry *entry)
{
    g_return_val_if_fail (MALIIT_IS_SETTINGS_ENTRY (entry), NULL);

    return entry->priv->extension_key;
}

/**
 * maliit_settings_entry_get_entry_type:
 * @entry: (transfer none): The #MaliitSettingsEntry.
 *
 * Gets type of the entry.
 *
 * Returns: A type.
 */
MaliitSettingsEntryType
maliit_settings_entry_get_entry_type (MaliitSettingsEntry *entry)
{
    g_return_val_if_fail (MALIIT_IS_SETTINGS_ENTRY (entry), MALIIT_STRING_TYPE);

    return entry->priv->type;
}

/**
 * maliit_settings_entry_is_current_value_valid:
 * @entry: (transfer none): The #MaliitSettingsEntry.
 *
 * Gets whether current value of the entry is valid.
 *
 * Returns: %TRUE if valid, otherwise %FALSE
 */
gboolean
maliit_settings_entry_is_current_value_valid (MaliitSettingsEntry *entry)
{
    g_return_val_if_fail (MALIIT_IS_SETTINGS_ENTRY (entry), FALSE);

    return entry->priv->valid;
}

/**
 * maliit_settings_entry_get_value:
 * @entry: (transfer none): The #MaliitSettingsEntry.
 *
 * Gets value of the entry. Check its validity with
 * maliit_settings_entry_is_current_value_valid() before using it.
 *
 * Returns: (transfer none): A value.
 */
GVariant *
maliit_settings_entry_get_value (MaliitSettingsEntry *entry)
{
    GHashTable *attributes;
    MaliitSettingsEntryPrivate *priv;

    g_return_val_if_fail (MALIIT_IS_SETTINGS_ENTRY (entry), NULL);

    priv = entry->priv;
    attributes = maliit_attribute_extension_get_attributes (priv->extension);

    return g_hash_table_lookup (attributes, priv->extension_key);
}

/**
 * maliit_settings_entry_set_value:
 * @entry: (transfer none): The #MaliitSettingsEntry.
 * @value: (transfer none): The #GVariant.
 *
 * Sets a new value of the entry. Before setting new value, validate
 * it with maliit_settings_entry_is_value_valid().
 */
void
maliit_settings_entry_set_value (MaliitSettingsEntry *entry,
                                 GVariant *value)
{
    MaliitSettingsEntryPrivate *priv;

    g_return_if_fail (MALIIT_IS_SETTINGS_ENTRY (entry));

    priv = entry->priv;
    maliit_attribute_extension_set_attribute (priv->extension,
                                              priv->extension_key,
                                              value);
}

/**
 * maliit_settings_entry_is_value_valid:
 * @entry: (transfer none): The #MaliitSettingsEntry.
 * @value: (transfer none): The #GVariant
 *
 * Checks whether the value is valid one for the entry.
 *
 * Returns: %TRUE if valid, otherwise %FALSE.
 */
gboolean
maliit_settings_entry_is_value_valid (MaliitSettingsEntry *entry,
                                      GVariant *value)
{
    MaliitSettingsEntryPrivate *priv;

    g_return_val_if_fail (MALIIT_IS_SETTINGS_ENTRY (entry), FALSE);

    priv = entry->priv;
    return maliit_validate_setting_value (priv->type, priv->attributes, value);
}

/**
 * maliit_settings_entry_get_attributes:
 * @entry: (transfer none): The #MaliitSettingsEntry.
 *
 * Gets attributes of the entry. The keys of the attributes are
 * MALIIT_SETTING_VALUE_DOMAIN(),
 * MALIIT_SETTING_VALUE_DOMAIN_DESCRIPTIONS(),
 * MALIIT_SETTING_VALUE_RANGE_MIN(), MALIIT_SETTING_VALUE_RANGE_MAX()
 * and MALIIT_SETTING_DEFAULT_VALUE(). Note that these keys don't have
 * to exist in attributes.
 *
 * Returns: (transfer none) (element-type utf8 GLib.Variant): Attributes. Returned value should not be modified nor freed.
 */
GHashTable *
maliit_settings_entry_get_attributes (MaliitSettingsEntry *entry)
{
    g_return_val_if_fail (MALIIT_IS_SETTINGS_ENTRY (entry), NULL);

    return entry->priv->attributes;
}
