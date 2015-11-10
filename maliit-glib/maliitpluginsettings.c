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

#include "maliitpluginsettingsprivate.h"
#include "maliitsettingsentryprivate.h"

/**
 * SECTION:maliitpluginsettings
 * @short_description: plugin settings
 * @title: MaliitPluginSettings
 * @see_also: #MaliitSettingsEntry, #MaliitSettingsManager
 * @stability: Stable
 * @include: maliit/maliitpluginsettings.h
 *
 * The #MaliitPluginSettings is a class holding general plugin
 * information like name, description and configuration entries.
 */

struct _MaliitPluginSettingsPrivate
{
    gchar *description_language;
    gchar *plugin_name;
    gchar *plugin_description;
    GPtrArray *entries;
};

G_DEFINE_TYPE (MaliitPluginSettings, maliit_plugin_settings, G_TYPE_OBJECT)

enum
{
    PROP_0,

    PROP_DESCRIPTION_LANGUAGE,
    PROP_PLUGIN_NAME,
    PROP_PLUGIN_DESCRIPTION,
    PROP_CONFIGURATION_ENTRIES
};

static void
maliit_plugin_settings_finalize (GObject *object)
{
    MaliitPluginSettings *settings = MALIIT_PLUGIN_SETTINGS (object);
    MaliitPluginSettingsPrivate *priv = settings->priv;

    g_free (priv->description_language);
    g_free (priv->plugin_name);
    g_free (priv->plugin_description);

    G_OBJECT_CLASS (maliit_plugin_settings_parent_class)->finalize (object);
}

static void
maliit_plugin_settings_dispose (GObject *object)
{
    MaliitPluginSettings *settings = MALIIT_PLUGIN_SETTINGS (object);
    MaliitPluginSettingsPrivate *priv = settings->priv;

    if (priv->entries) {
        GPtrArray *entries = priv->entries;

        priv->entries = NULL;
        g_ptr_array_unref (entries);
    }

    G_OBJECT_CLASS (maliit_plugin_settings_parent_class)->dispose (object);
}

static void
maliit_plugin_settings_set_property (GObject *object,
                                     guint prop_id,
                                     const GValue *value,
                                     GParamSpec *pspec)
{
    MaliitPluginSettings *settings = MALIIT_PLUGIN_SETTINGS (object);
    MaliitPluginSettingsPrivate *priv = settings->priv;

    switch (prop_id) {
    case PROP_DESCRIPTION_LANGUAGE:
        g_free (priv->description_language);
        priv->description_language = g_value_dup_string (value);
        break;
    case PROP_PLUGIN_NAME:
        g_free (priv->plugin_name);
        priv->plugin_name = g_value_dup_string (value);
        break;
    case PROP_PLUGIN_DESCRIPTION:
        g_free (priv->plugin_description);
        priv->plugin_description = g_value_dup_string (value);
        break;
    case PROP_CONFIGURATION_ENTRIES:
        if (priv->entries) {
            g_ptr_array_unref (priv->entries);
        }
        priv->entries = g_value_dup_boxed (value);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
        break;
    }
}

static void
maliit_plugin_settings_get_property (GObject *object,
                                     guint prop_id,
                                     GValue *value,
                                     GParamSpec *pspec)
{
    MaliitPluginSettings *settings = MALIIT_PLUGIN_SETTINGS (object);
    MaliitPluginSettingsPrivate *priv = settings->priv;

    switch (prop_id) {
    case PROP_DESCRIPTION_LANGUAGE:
        g_value_set_string (value, priv->description_language);
        break;
    case PROP_PLUGIN_NAME:
        g_value_set_string (value, priv->plugin_name);
        break;
    case PROP_PLUGIN_DESCRIPTION:
        g_value_set_string (value, priv->plugin_description);
        break;
    case PROP_CONFIGURATION_ENTRIES:
        g_value_set_boxed (value, priv->entries);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
        break;
    }
}

static void
maliit_plugin_settings_class_init (MaliitPluginSettingsClass *settings_class)
{
    GObjectClass *g_object_class = G_OBJECT_CLASS (settings_class);

    g_object_class->finalize = maliit_plugin_settings_finalize;
    g_object_class->dispose = maliit_plugin_settings_dispose;
    g_object_class->set_property = maliit_plugin_settings_set_property;
    g_object_class->get_property = maliit_plugin_settings_get_property;

    /**
     * MaliitPluginSettings:description-language:
     *
     * Language of the plugin description.
     */
    g_object_class_install_property (g_object_class,
                                     PROP_DESCRIPTION_LANGUAGE,
                                     g_param_spec_string ("description-language",
                                                          "Description language", /* TODO: mark as translatable? */
                                                          "Language of the plugin description", /* TODO: mark as translatable? */
                                                          "en",
                                                          G_PARAM_READABLE |
                                                          G_PARAM_WRITABLE |
                                                          G_PARAM_CONSTRUCT_ONLY |
                                                          G_PARAM_STATIC_NAME |
                                                          G_PARAM_STATIC_BLURB |
                                                          G_PARAM_STATIC_NICK));

    /**
     * MaliitPluginSettings:plugin-name:
     *
     * Name of the plugin.
     */
    g_object_class_install_property (g_object_class,
                                     PROP_PLUGIN_NAME,
                                     g_param_spec_string ("plugin-name",
                                                          "Plugin name", /* TODO: mark as translatable? */
                                                          "Name of the plugin", /* TODO: mark as translatable? */
                                                          "unknown",
                                                          G_PARAM_READABLE |
                                                          G_PARAM_WRITABLE |
                                                          G_PARAM_CONSTRUCT_ONLY |
                                                          G_PARAM_STATIC_NAME |
                                                          G_PARAM_STATIC_BLURB |
                                                          G_PARAM_STATIC_NICK));

    /**
     * MaliitPluginSettings:plugin-description:
     *
     * Description of the plugin.
     */
    g_object_class_install_property (g_object_class,
                                     PROP_PLUGIN_DESCRIPTION,
                                     g_param_spec_string ("plugin-description",
                                                          "Plugin description", /* TODO: mark as translatable? */
                                                          "Description of the plugin", /* TODO: mark as translatable? */
                                                          "",
                                                          G_PARAM_READABLE |
                                                          G_PARAM_WRITABLE |
                                                          G_PARAM_CONSTRUCT_ONLY |
                                                          G_PARAM_STATIC_NAME |
                                                          G_PARAM_STATIC_BLURB |
                                                          G_PARAM_STATIC_NICK));

    /* TODO: this property appears as array of gpointers in gir file.
     * I have no clue how to fix it.
     */
    /**
     * MaliitPluginSettings:configuration-entries: (element-type Maliit.SettingsEntry)
     *
     * List of configuration entries of the plugin.
     */
    g_object_class_install_property (g_object_class,
                                     PROP_CONFIGURATION_ENTRIES,
                                     g_param_spec_boxed ("configuration-entries",
                                                         "Configuration entries", /* TODO: mark as translatable? */
                                                         "List of configuration entries of the plugin", /* TODO: mark as translatable? */
                                                         G_TYPE_PTR_ARRAY,
                                                         G_PARAM_READABLE |
                                                         G_PARAM_WRITABLE |
                                                         G_PARAM_CONSTRUCT_ONLY |
                                                         G_PARAM_STATIC_NAME |
                                                         G_PARAM_STATIC_BLURB |
                                                         G_PARAM_STATIC_NICK));

    g_type_class_add_private (settings_class, sizeof (MaliitPluginSettingsPrivate));
}

static void
maliit_plugin_settings_init (MaliitPluginSettings *settings)
{
    MaliitPluginSettingsPrivate *priv = G_TYPE_INSTANCE_GET_PRIVATE (settings,
                                                                     MALIIT_TYPE_PLUGIN_SETTINGS,
                                                                     MaliitPluginSettingsPrivate);

    priv->description_language = NULL;
    priv->plugin_name = NULL;
    priv->plugin_description = NULL;
    priv->entries = NULL;

    settings->priv = priv;
}

static GPtrArray *
configuration_entries_from_dbus_g_variant (GVariant                 *dbus_entries,
                                           MaliitAttributeExtension *extension)
{
    GPtrArray *configuration_entries = g_ptr_array_sized_new (g_variant_n_children (dbus_entries));
    guint iter;

    g_ptr_array_set_free_func (configuration_entries, g_object_unref);
    for (iter = 0; iter < g_variant_n_children (dbus_entries); ++iter) {
        GVariant *info = g_variant_get_child_value (dbus_entries, iter);

        g_ptr_array_add (configuration_entries,
                         maliit_settings_entry_new_from_dbus_data (info,
                                                                   extension));

        g_variant_unref (info);
    }
    return configuration_entries;
}

/**
 * maliit_plugin_settings_new_from_dbus_data: (skip)
 * @plugin_info: A #GVariant of DBus provenance containing plugin information.
 * @extension: A #MaliitAttributeExtensions for #MaliitAttributeSettingsEntry instances.
 *
 * Creates new settings settings. This is used internally only by
 * #MaliitSettingsManager.
 *
 * Returns: The newly created #MaliitPluginSettings.
 */
MaliitPluginSettings *
maliit_plugin_settings_new_from_dbus_data (GVariant                 *plugin_info,
                                           MaliitAttributeExtension *extension)
{
    const gchar *description_language;
    const gchar *plugin_name;
    const gchar *plugin_description;
    GVariant *plugin_entries;
    GPtrArray *configuration_entries;
    MaliitPluginSettings *settings;

    g_return_val_if_fail (MALIIT_IS_ATTRIBUTE_EXTENSION (extension), NULL);

    g_variant_get (plugin_info,
                   "(&s&s&si@a(ssibva{sv}))",
                   &description_language,
                   &plugin_name,
                   &plugin_description,
                   NULL,
                   &plugin_entries);

    configuration_entries = configuration_entries_from_dbus_g_variant (plugin_entries, extension);
    settings = MALIIT_PLUGIN_SETTINGS (g_object_new (MALIIT_TYPE_PLUGIN_SETTINGS,
                                                     "description-language", description_language,
                                                     "plugin-name", plugin_name,
                                                     "plugin-description", plugin_description,
                                                     "configuration-entries", configuration_entries,
                                                     NULL));

    g_ptr_array_unref (configuration_entries);
    g_variant_unref (plugin_entries);

    return settings;
}

/**
 * maliit_plugin_settings_get_description_language:
 * @settings: (transfer none): The #MaliitPluginSettings.
 *
 * Gets language of the plugin description.
 *
 * Returns: (transfer none): A language. Returned value should not be modified nor freed.
 */
const gchar *
maliit_plugin_settings_get_description_language (MaliitPluginSettings *settings)
{
    g_return_val_if_fail (MALIIT_IS_PLUGIN_SETTINGS (settings), NULL);

    return settings->priv->description_language;
}

/**
 * maliit_plugin_settings_get_plugin_name:
 * @settings: (transfer none): The #MaliitPluginSettings.
 *
 * Gets name of the plugin.
 *
 * Returns: (transfer none): A name. Returned value should not be modified nor freed.
 */
const gchar *
maliit_plugin_settings_get_plugin_name (MaliitPluginSettings *settings)
{
    g_return_val_if_fail (MALIIT_IS_PLUGIN_SETTINGS (settings), NULL);

    return settings->priv->plugin_name;
}

/**
 * maliit_plugin_settings_get_plugin_description:
 * @settings: (transfer none): The #MaliitPluginSettings.
 *
 * Gets description of the plugin.
 *
 * Returns: (transfer none): A description. Returned value should not be modified nor freed.
 */
const gchar *
maliit_plugin_settings_get_plugin_description (MaliitPluginSettings *settings)
{
    g_return_val_if_fail (MALIIT_IS_PLUGIN_SETTINGS (settings), NULL);

    return settings->priv->plugin_description;
}

/**
 * maliit_plugin_settings_get_configuration_entries:
 * @settings: (transfer none): The #MaliitPluginSettings.
 *
 * Gets configuration entries of the plugin
 *
 * Returns: (transfer none) (element-type Maliit.SettingsEntry): Configuration entries. Returned value should not be modified nor freed.
 */
GPtrArray *
maliit_plugin_settings_get_configuration_entries (MaliitPluginSettings *settings)
{
    g_return_val_if_fail (MALIIT_IS_PLUGIN_SETTINGS (settings), NULL);

    return settings->priv->entries;
}
