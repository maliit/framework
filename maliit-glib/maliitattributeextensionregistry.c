/* This file is part of Maliit framework
 *
 * Copyright (C) 2012 One Laptop per Child Association
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

#include "maliitattributeextensionregistry.h"
#include "maliitbus.h"

struct _MaliitAttributeExtensionRegistryPrivate
{
    GHashTable *extensions;
};

static MaliitAttributeExtensionRegistry *global_singleton;

G_DEFINE_TYPE_WITH_CODE (MaliitAttributeExtensionRegistry,
                         maliit_attribute_extension_registry,
                         G_TYPE_OBJECT,
                         G_ADD_PRIVATE (MaliitAttributeExtensionRegistry))

static void
maliit_attribute_extension_registry_finalize (GObject *object)
{
    global_singleton = NULL;

    G_OBJECT_CLASS (maliit_attribute_extension_registry_parent_class)->finalize (object);
}

static void
extension_notify (gpointer data,
                  GObject *where_the_object_was)
{
    MaliitAttributeExtensionRegistry *registry = MALIIT_ATTRIBUTE_EXTENSION_REGISTRY (data);
    MaliitAttributeExtensionRegistryPrivate *priv = registry->priv;
    GHashTableIter iter;
    MaliitAttributeExtension *extension;

    g_hash_table_iter_init (&iter, priv->extensions);
    while (g_hash_table_iter_next (&iter, NULL, (gpointer *)&extension)) {
        if ((gpointer)extension == (gpointer)where_the_object_was) {
            g_hash_table_iter_steal (&iter);
            break;
        }
    }
}

static void
maliit_attribute_extension_registry_dispose (GObject *object)
{
    MaliitAttributeExtensionRegistry *registry = MALIIT_ATTRIBUTE_EXTENSION_REGISTRY (object);
    MaliitAttributeExtensionRegistryPrivate *priv = registry->priv;

    if (priv->extensions) {
        GHashTable *extensions = priv->extensions;

        priv->extensions = NULL;
        g_hash_table_unref (extensions);
    }

    G_OBJECT_CLASS (maliit_attribute_extension_registry_parent_class)->dispose (object);
}

static GObject*
maliit_attribute_extension_registry_constructor (GType type,
                                                 guint n_params,
                                                 GObjectConstructParam *params)
{
    GObject *object;

    if (global_singleton) {
        object = g_object_ref (G_OBJECT (global_singleton));
    } else {
        object = G_OBJECT_CLASS (maliit_attribute_extension_registry_parent_class)->constructor (type,
                                                                                                 n_params,
                                                                                                 params);
        /* We are doing an additional reference here, so object will not
         * be destroyed when last owner removes its reference. This is a
         * leak, but for now it ensures that singleton have a lifetime of
         * application.  This needs to be fixed, when object lifetimes are
         * fixed in gtk-input-context. */
        global_singleton = MALIIT_ATTRIBUTE_EXTENSION_REGISTRY (g_object_ref (object));
    }

    return object;
}

static void
maliit_attribute_extension_registry_class_init (MaliitAttributeExtensionRegistryClass *registry_class)
{
    GObjectClass *g_object_class = G_OBJECT_CLASS (registry_class);

    g_object_class->finalize = maliit_attribute_extension_registry_finalize;
    g_object_class->dispose = maliit_attribute_extension_registry_dispose;
    g_object_class->constructor = maliit_attribute_extension_registry_constructor;
}

static void
extension_weak_unref (MaliitAttributeExtension *extension,
                      MaliitAttributeExtensionRegistry *registry)
{
    g_object_weak_unref (G_OBJECT (extension),
                         extension_notify,
                         registry);
}

static void
extension_weak_unref_global (gpointer data)
{
    MaliitAttributeExtension *extension = MALIIT_ATTRIBUTE_EXTENSION (data);

    extension_weak_unref (extension,
                          global_singleton);
}

static void
register_all_extensions (MaliitServer *server, gpointer user_data)
{
    MaliitAttributeExtensionRegistry *registry = user_data;
    GList *extensions = maliit_attribute_extension_registry_get_extensions (registry);
    GList *iter;
    GError *error = NULL;

    for (iter = extensions; iter; iter = iter->next) {
        MaliitAttributeExtension *extension = MALIIT_ATTRIBUTE_EXTENSION (iter->data);
    }

    g_list_free (extensions);
}

static void
connection_established (GObject      *source_object G_GNUC_UNUSED,
                        GAsyncResult *res,
                        gpointer      user_data)
{
    GError *error = NULL;
    MaliitServer *server = maliit_get_server_finish (res, &error);

    if (server) {
        register_all_extensions (server, user_data);
    } else {
        g_warning ("Unable to connect to server: %s", error->message);
        g_clear_error (&error);
    }
}

static void
maliit_attribute_extension_registry_init (MaliitAttributeExtensionRegistry *registry)
{
    MaliitAttributeExtensionRegistryPrivate *priv = maliit_attribute_extension_registry_get_instance_private (registry);

    priv->extensions = g_hash_table_new_full (g_direct_hash,
                                              g_direct_equal,
                                              NULL,
                                              extension_weak_unref_global);

    registry->priv = priv;

    maliit_get_server (NULL, connection_established, registry);
}

MaliitAttributeExtensionRegistry *
maliit_attribute_extension_registry_get_instance (void)
{
    return MALIIT_ATTRIBUTE_EXTENSION_REGISTRY (g_object_new (MALIIT_TYPE_ATTRIBUTE_EXTENSION_REGISTRY,
                                                              NULL));
}

void
maliit_attribute_extension_registry_add_extension (MaliitAttributeExtensionRegistry *registry,
                                                   MaliitAttributeExtension *extension)
{
    MaliitServer *server;
    GHashTable *extensions;
    gint id;
    GError *error = NULL;

    g_return_if_fail (MALIIT_IS_ATTRIBUTE_EXTENSION_REGISTRY (registry));
    g_return_if_fail (MALIIT_IS_ATTRIBUTE_EXTENSION (extension));

    extensions = registry->priv->extensions;
    id = maliit_attribute_extension_get_id (extension);

    if (!g_hash_table_lookup_extended (extensions, GINT_TO_POINTER (id), NULL, NULL)) {
        g_object_weak_ref (G_OBJECT (extension),
                           extension_notify,
                           registry);

        g_hash_table_insert (extensions,
                             GINT_TO_POINTER (id),
                             extension);

        server = maliit_get_server_sync (NULL, &error);

        if (!server) {
            g_warning ("Unable to connect to server: %s", error->message);
            g_clear_error (&error);
        }
    }
}

void
maliit_attribute_extension_registry_remove_extension (MaliitAttributeExtensionRegistry *registry,
                                                      MaliitAttributeExtension *extension)
{
    MaliitServer *server;
    GHashTable *extensions;
    gint id;
    GError *error = NULL;

    g_return_if_fail (MALIIT_IS_ATTRIBUTE_EXTENSION_REGISTRY (registry));
    g_return_if_fail (MALIIT_IS_ATTRIBUTE_EXTENSION (extension));

    extensions = registry->priv->extensions;
    id = maliit_attribute_extension_get_id (extension);

    if (g_hash_table_lookup_extended (extensions, GINT_TO_POINTER (id), NULL, NULL)) {
        g_hash_table_remove (extensions,
                             GINT_TO_POINTER (id));

        server = maliit_get_server_sync (NULL, &error);

        if (!server) {
            g_warning ("Unable to connect to server: %s", error->message);
            g_clear_error (&error);
        }
    }
}

/* For glib < 2.30 */
#ifndef G_VALUE_INIT
#define G_VALUE_INIT { 0, { { 0 } } }
#endif

void
maliit_attribute_extension_registry_extension_changed (MaliitAttributeExtensionRegistry *registry,
                                                       MaliitAttributeExtension *extension,
                                                       const gchar *key,
                                                       GVariant *value)
{
    MaliitServer *server;
    gchar **parts;
    GError *error = NULL;

    g_return_if_fail (MALIIT_IS_ATTRIBUTE_EXTENSION_REGISTRY (registry));
    g_return_if_fail (MALIIT_IS_ATTRIBUTE_EXTENSION (extension));
    g_return_if_fail (key != NULL);
    g_return_if_fail (value != NULL);

    parts = g_strsplit (key + 1, "/", 3);

    if (!parts)
        return;

    if (g_strv_length (parts) == 3) {
        gchar *target = g_strdup_printf ("/%s", parts[0]);

        server = maliit_get_server_sync (NULL, &error);

        if (!server) {
            g_warning ("Unable to connect to server: %s", error->message);
            g_clear_error (&error);
        }

        g_free (target);
    } else {
        g_warning("Key `%s' is not valid. It needs to be `/target/item/key'", key);
    }
    g_strfreev (parts);
}

static void
fill_list_with_extensions (gpointer key G_GNUC_UNUSED,
                           gpointer value,
                           gpointer user_data)
{
    MaliitAttributeExtension *extension = MALIIT_ATTRIBUTE_EXTENSION (value);
    GList **list = (GList **)user_data;

    *list = g_list_prepend (*list, extension);
}

GList *
maliit_attribute_extension_registry_get_extensions (MaliitAttributeExtensionRegistry *registry)
{
    GList *list;

    g_return_val_if_fail (MALIIT_IS_ATTRIBUTE_EXTENSION_REGISTRY (registry), NULL);

    list = NULL;
    g_hash_table_foreach (registry->priv->extensions,
                          fill_list_with_extensions,
                          &list);

    return list;
}

void
maliit_attribute_extension_registry_update_attribute (MaliitAttributeExtensionRegistry *registry,
                                                      gint id,
                                                      const gchar *target,
                                                      const gchar *target_item,
                                                      const gchar *attribute,
                                                      GVariant *value)
{
    MaliitAttributeExtension *extension;

    g_return_if_fail (MALIIT_IS_ATTRIBUTE_EXTENSION_REGISTRY (registry));
    g_return_if_fail (id >= 0);
    g_return_if_fail (target != NULL);
    g_return_if_fail (target_item != NULL);
    g_return_if_fail (attribute != NULL);
    g_return_if_fail (value != NULL);

    if (g_hash_table_lookup_extended (registry->priv->extensions,
                                      GINT_TO_POINTER (id),
                                      NULL,
                                      (gpointer *)&extension)) {
        gchar *key = g_strdup_printf ("%s/%s/%s", target, target_item, attribute);

        maliit_attribute_extension_update_attribute (extension,
                                                     key,
                                                     value);
        g_free (key);
    } else {
        g_warning ("Extension %d was not found.", id);
    }
}
