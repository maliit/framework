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

#include "maliitsettingsmanager.h"
#include "maliitpluginsettingsprivate.h"
#include "maliitattributeextensionprivate.h"
#include "maliitbus.h"

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
};

G_DEFINE_TYPE_WITH_CODE (MaliitSettingsManager, maliit_settings_manager,
                         G_TYPE_OBJECT, G_ADD_PRIVATE (MaliitSettingsManager))

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
    GError *error = NULL;
    MaliitSettingsManager *manager = MALIIT_SETTINGS_MANAGER (object);
    MaliitContext *context = maliit_get_context_sync (NULL, &error);

    if (context) {
        g_signal_handlers_disconnect_by_data (context, manager);
    } else {
        g_warning ("Unable to connect to context: %s", error->message);
        g_clear_error (&error);
    }

    if (manager->priv->settings_list_changed) {
        g_signal_handlers_disconnect_by_data (manager->priv->settings_list_changed, manager);
    }

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
     *
     * Deprecated: 0.99.0: This signal will no longer be emitted.
     */
    signals[DISCONNECTED] =
        g_signal_new ("disconnected",
                      MALIIT_TYPE_SETTINGS_MANAGER,
                      G_SIGNAL_RUN_FIRST,
                      0,
                      NULL,
                      NULL,
                      g_cclosure_marshal_VOID__VOID,
                      G_TYPE_NONE,
                      0);
}

static void
on_connection_established (MaliitSettingsManager *manager,
                          gpointer user_data G_GNUC_UNUSED)
{
    g_signal_emit(manager, signals[CONNECTED], 0);
}

static gboolean
on_plugins_loaded (MaliitSettingsManager *manager,
                   GDBusMethodInvocation *invocation G_GNUC_UNUSED,
                   GVariant *plugin_settings,
                   gpointer user_data G_GNUC_UNUSED)
{
    guint iter;
    GList *result;
    GHashTable *extensions;
    GVariant *plugin_info;

    if (!plugin_settings) {
        return FALSE;
    }

    result = NULL;
    extensions = g_hash_table_new (g_direct_hash,
                                   g_direct_equal);

    for (iter = 0; iter < g_variant_n_children (plugin_settings); ++iter) {
        const gchar *plugin_name;
        int extension_id;

        g_variant_get_child (plugin_settings, iter, "(&s&s&si@a(ssibva{sv}))", NULL, &plugin_name, NULL, &extension_id, NULL);

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

            plugin_info = g_variant_get_child_value (plugin_settings, iter);

            result = g_list_prepend (result,
                                     maliit_plugin_settings_new_from_dbus_data (plugin_info,
                                                                                extension));

            g_variant_unref (plugin_info);
        }
    }

    result = g_list_reverse (result);
    g_signal_emit (manager,
                   signals[PLUGIN_SETTINGS_RECEIVED],
                   0,
                   result);

    g_list_free_full (result,
                      g_object_unref);

    return FALSE;
}

static void
connection_established (GObject      *source_object G_GNUC_UNUSED,
                        GAsyncResult *res,
                        gpointer      user_data)
{
    GError *error = NULL;
    MaliitServer *server = maliit_get_server_finish (res, &error);

    if (server) {
        on_connection_established (user_data, server);
    } else {
        g_warning ("Unable to connect to server: %s", error->message);
        g_clear_error (&error);
    }
}

static void
maliit_settings_manager_init (MaliitSettingsManager *manager)
{
    MaliitSettingsManagerPrivate *priv = maliit_settings_manager_get_instance_private (manager);
    MaliitContext *context;
    GError *error = NULL;

    priv->settings_list_changed = NULL;
    priv->attribute_changed_signal_id = 0;
    manager->priv = priv;

    maliit_get_server (NULL, connection_established, manager);

    context = maliit_get_context_sync (NULL, &error);

    if (context) {
        g_signal_connect_swapped (context,
                                  "handle-plugin-settings-loaded",
                                  G_CALLBACK (on_plugins_loaded),
                                  manager);
    } else {
        g_warning ("Unable to connect to context: %s", error->message);
        g_clear_error (&error);
    }
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
 * The settings will be returned async via the MaliitServerManager::plugin-settings-received signal
 */
void
maliit_settings_manager_load_plugin_settings (MaliitSettingsManager *manager)
{
    MaliitServer *server;
    GError *error = NULL;

    g_return_if_fail (MALIIT_IS_SETTINGS_MANAGER (manager));

    server = maliit_get_server_sync (NULL, &error);

    if (!server) {
        g_warning ("Unable to connect to server: %s", error->message);
        g_clear_error (&error);
    }
}

/**
 * maliit_settings_manager_set_preferred_description_locale:
 * @locale_name: (transfer none): The new preferred locale.
 *
 * Sets the preferred locale for human-readable descriptions. The setting is
 * valid for all instances of #MaliitSettingsManager in the process.
 * Note that the server may not always be able to return info in the requested locale.
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
