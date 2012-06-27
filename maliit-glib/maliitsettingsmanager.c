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

#include "maliitsettingsmanager.h"
#include "maliitpluginsettingsprivate.h"
#include "maliitattributeextensionprivate.h"
#include "meego-im-connector.h"

/**
 * SECTION:maliitsettingsmanager
 * @short_description: settings manager
 * @title: MaliitSettingsManager
 * @see_also: #MaliitSettingsEntry, #MaliitPluginSettings
 * @stability: Stable
 * @include: maliit/maliitsettingsmanager.h
 *
 * The #MaliitSettingsManager handles requesting and receiving plugin
 * settings from maliit-server and notifying when those settings has
 * changed.
 */

static gchar* preferred_description_locale = 0;

struct _MaliitSettingsManagerPrivate
{
    MaliitAttributeExtension *settings_list_changed;
    guint attribute_changed_signal_id;
    MeegoImConnector *connector;
};

G_DEFINE_TYPE (MaliitSettingsManager, maliit_settings_manager, G_TYPE_OBJECT)

enum
{
    PLUGIN_SETTINGS_RECEIVED,
    CONNECTED,
    DISCONNECTED,

    LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

static void
maliit_settings_manager_finalize (GObject *object)
{
    G_OBJECT_CLASS (maliit_settings_manager_parent_class)->finalize (object);
}

static void
maliit_settings_manager_dispose (GObject *object)
{
    MaliitSettingsManager *manager = MALIIT_SETTINGS_MANAGER (object);

    g_clear_object (&manager->priv->settings_list_changed);

    G_OBJECT_CLASS (maliit_settings_manager_parent_class)->dispose (object);
}

static void
maliit_settings_manager_class_init (MaliitSettingsManagerClass *manager_class)
{
    GObjectClass *g_object_class = G_OBJECT_CLASS (manager_class);

    g_object_class->finalize = maliit_settings_manager_finalize;
    g_object_class->dispose = maliit_settings_manager_dispose;

    /**
     * MaliitSettingsManager::plugin-settings-received:
     * @manager: The #MaliitSettingsManager emitting the signal.
     * @settings: (type GLib.List) (element-type Maliit.PluginSettings): Gotten settings.
     *
     * Emitted after call to
     * maliit_settings_manager_load_plugin_settings() and when the
     * plugin list changes on the server.
     */
    signals[PLUGIN_SETTINGS_RECEIVED] =
        g_signal_new ("plugin-settings-received",
                      MALIIT_TYPE_SETTINGS_MANAGER,
                      G_SIGNAL_RUN_FIRST,
                      0,
                      NULL,
                      NULL,
                      g_cclosure_marshal_VOID__POINTER,
                      G_TYPE_NONE,
                      1,
                      G_TYPE_POINTER);

    /**
     * MaliitSettingsManager::connected:
     * @manager: The #MaliitSettingsManager emitting the signal.
     *
     * Emitted when connection to maliit-server is established.
     */
    signals[CONNECTED] =
        g_signal_new ("connected",
                      MALIIT_TYPE_SETTINGS_MANAGER,
                      G_SIGNAL_RUN_FIRST,
                      0,
                      NULL,
                      NULL,
                      g_cclosure_marshal_VOID__VOID,
                      G_TYPE_NONE,
                      0);

    /**
     * MaliitSettingsManager::disconnected:
     * @manager: The #MaliitSettingsManager emitting the signal.
     *
     * Emitted when connection to maliit-server is broken.
     */
    signals[CONNECTED] =
        g_signal_new ("disconnected",
                      MALIIT_TYPE_SETTINGS_MANAGER,
                      G_SIGNAL_RUN_FIRST,
                      0,
                      NULL,
                      NULL,
                      g_cclosure_marshal_VOID__VOID,
                      G_TYPE_NONE,
                      0);

    g_type_class_add_private (manager_class, sizeof (MaliitSettingsManagerPrivate));
}

static void
on_connection_established (MaliitSettingsManager *manager,
                          gpointer user_data G_GNUC_UNUSED)
{
    g_signal_emit(manager, signals[CONNECTED], 0);
}

static void
on_connection_dropped (MaliitSettingsManager *manager,
                       gpointer user_data G_GNUC_UNUSED)
{
    g_signal_emit(manager, signals[DISCONNECTED], 0);
}

static gboolean
dbus_plugin_info_is_valid (GValueArray *plugin_info)
{
    static GType expected_types[] = {
        G_TYPE_STRING, /* description language */
        G_TYPE_STRING, /* plugin name */
        G_TYPE_STRING, /* plugin description */
        G_TYPE_INT, /* extension id */
        G_TYPE_INVALID /* settings entries, set in the first run */
    };

    guint iter;

    if (expected_types[4] == G_TYPE_INVALID) {
        GType attributes_gtype = dbus_g_type_get_map("GHashTable",
                                                     G_TYPE_STRING,
                                                     G_TYPE_VALUE);
        GType entry_gtype = dbus_g_type_get_struct("GValueArray",
                                                   G_TYPE_STRING,
                                                   G_TYPE_STRING,
                                                   G_TYPE_INT,
                                                   G_TYPE_BOOLEAN,
                                                   G_TYPE_VALUE,
                                                   attributes_gtype,
                                                   G_TYPE_INVALID);

        expected_types[4] = dbus_g_type_get_collection("GPtrArray",
                                                       entry_gtype);
    }

    if (!plugin_info) {
        return FALSE;
    }

    if (plugin_info->n_values != G_N_ELEMENTS (expected_types)) {
        return FALSE;
    }

    for (iter = 0; iter < G_N_ELEMENTS (expected_types); ++iter) {
        GValue *value = g_value_array_get_nth (plugin_info, iter);

        if (!value) {
            return FALSE;
        }
        if (!G_VALUE_HOLDS (value, expected_types[iter])) {
            return FALSE;
        }
    }

    return TRUE;
}

static void
on_plugins_loaded (MaliitSettingsManager *manager,
                   GPtrArray *plugin_settings,
                   gpointer user_data G_GNUC_UNUSED)
{
    guint iter;
    GList *result;
    GHashTable *extensions;

    if (!plugin_settings) {
        return;
    }

    result = NULL;
    extensions = g_hash_table_new (g_direct_hash,
                                   g_direct_equal);

    for (iter = 0; iter < plugin_settings->len; ++iter) {
        GValueArray *plugin_info = g_ptr_array_index (plugin_settings, iter);
        const gchar *plugin_name;
        int extension_id;


        if (!dbus_plugin_info_is_valid (plugin_info)) {
            g_warning ("Received invalid plugin informations");
            continue;
        }

        plugin_name = g_value_get_string (g_value_array_get_nth (plugin_info, 1));
        extension_id = g_value_get_int(g_value_array_get_nth (plugin_info, 3));

        if (!g_strcmp0(plugin_name, "@settings")) {
            MaliitSettingsManagerPrivate *priv = manager->priv;

            if (priv->attribute_changed_signal_id) {
                if (priv->settings_list_changed) {
                    g_signal_handler_disconnect (priv->settings_list_changed,
                                                 priv->attribute_changed_signal_id);
                    g_clear_object (&priv->settings_list_changed);
                }
                priv->attribute_changed_signal_id = 0;
            }

            priv->settings_list_changed = maliit_attribute_extension_new_with_id (extension_id);
            priv->attribute_changed_signal_id = g_signal_connect_swapped (priv->settings_list_changed,
                                                                          "extended-attribute-changed",
                                                                          G_CALLBACK (maliit_settings_manager_load_plugin_settings),
                                                                          manager);
        } else {
            MaliitAttributeExtension *extension = g_hash_table_lookup (extensions,
                                                                       GINT_TO_POINTER (extension_id));

            if (!extension) {
                extension = maliit_attribute_extension_new_with_id (extension_id);
                g_hash_table_insert (extensions, GINT_TO_POINTER (extension_id), extension);
            }

            result = g_list_prepend (result,
                                     maliit_plugin_settings_new_from_dbus_data (plugin_info,
                                                                                extension));
        }
    }

    result = g_list_reverse (result);
    g_signal_emit (manager,
                   signals[PLUGIN_SETTINGS_RECEIVED],
                   0,
                   result);

    g_list_free_full (result,
                      g_object_unref);
}

static void
maliit_settings_manager_init (MaliitSettingsManager *manager)
{
    MeegoIMProxy *proxy;
    MeegoIMContextDbusObj *context_dbus;
    MaliitSettingsManagerPrivate *priv = G_TYPE_INSTANCE_GET_PRIVATE (manager,
                                                                      MALIIT_TYPE_SETTINGS_MANAGER,
                                                                      MaliitSettingsManagerPrivate);

    priv->settings_list_changed = NULL;
    priv->attribute_changed_signal_id = 0;
    priv->connector = meego_im_connector_get_singleton ();
    proxy = priv->connector->proxy;
    context_dbus = priv->connector->dbusobj;

    g_signal_connect_swapped (proxy,
                              "connection-established",
                              G_CALLBACK (on_connection_established),
                              manager);
    g_signal_connect_swapped (proxy,
                              "connection-dropped",
                              G_CALLBACK(on_connection_dropped),
                              manager);
    g_signal_connect_swapped (context_dbus,
                              "plugin-settings-loaded",
                              G_CALLBACK(on_plugins_loaded),
                              manager);

    manager->priv = priv;
}

/**
 * maliit_settings_manager_new:
 *
 * Creates new settings manager.
 *
 * Returns: (transfer full): The newly created
 * #MaliitSettingsManager.
 */
MaliitSettingsManager *
maliit_settings_manager_new (void)
{
    return MALIIT_SETTINGS_MANAGER (g_object_new (MALIIT_TYPE_SETTINGS_MANAGER,
                                                  NULL));
}

/**
 * maliit_settings_manager_load_plugin_settings:
 * @manager: (transfer none): The #MaliitSettingsManager.
 *
 * Request the list of settings from maliit-server.
 */
void
maliit_settings_manager_load_plugin_settings (MaliitSettingsManager *manager)
{
    g_return_if_fail (MALIIT_IS_SETTINGS_MANAGER (manager));

    meego_im_proxy_load_plugin_settings (manager->priv->connector->proxy,
                                         maliit_settings_manager_get_preferred_description_locale ());
}

/**
 * maliit_settings_manager_set_preferred_description_locale:
 * @locale_name: (transfer none): The new preferred locale.
 *
 * Sets the preferred locale for human-readable description.
 */
void
maliit_settings_manager_set_preferred_description_locale (const gchar *locale_name)
{
    g_return_if_fail (locale_name != NULL);

    if (preferred_description_locale) {
        g_free (preferred_description_locale);
    }
    preferred_description_locale = g_strdup (locale_name);
}

/**
 * maliit_settings_manager_get_preferred_description_locale:
 *
 * Gets the preferred locale for human-readable description.
 *
 * Returns: (transfer none): The string being a current preferred
 * locale. Returned string should not be freed nor modified.
 */
const gchar *
maliit_settings_manager_get_preferred_description_locale (void)
{
    if (!preferred_description_locale) {
        preferred_description_locale = g_strdup("en");
    }

    return preferred_description_locale;
}
