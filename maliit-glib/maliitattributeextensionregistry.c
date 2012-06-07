/* This file is part of Maliit framework
 *
 * Copyright (C) 2012 One Laptop per Child Association
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

#include "maliitattributeextensionregistry.h"
#include "maliitmarshallers.h"
#include "meego-im-connector.h"

struct _MaliitAttributeExtensionRegistryPrivate
{
    GHashTable *extensions;
};

static MaliitAttributeExtensionRegistry *global_singleton;

G_DEFINE_TYPE (MaliitAttributeExtensionRegistry, maliit_attribute_extension_registry, G_TYPE_OBJECT)

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

    g_type_class_add_private (registry_class, sizeof (MaliitAttributeExtensionRegistryPrivate));
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
register_all_extensions (MeegoIMProxy *proxy, gpointer user_data)
{
    MaliitAttributeExtensionRegistry *registry = user_data;
    GList *extensions = maliit_attribute_extension_registry_get_extensions (registry);
    GList *iter;

    for (iter = extensions; iter; iter = iter->next) {
        MaliitAttributeExtension *extension = MALIIT_ATTRIBUTE_EXTENSION (iter->data);

        if (!meego_im_proxy_register_extension (proxy,
                                                maliit_attribute_extension_get_id (extension),
                                                maliit_attribute_extension_get_filename (extension))) {
            g_warning ("could not register an extension in mass registerer - no proxy");
        } else {
            GHashTable *attributes = maliit_attribute_extension_get_attributes (extension);
            GHashTableIter attributes_iter;
            gpointer key;
            gpointer value;

            g_hash_table_iter_init (&attributes_iter, attributes);

            while (g_hash_table_iter_next (&attributes_iter, &key, &value)) {
                maliit_attribute_extension_registry_extension_changed(registry, extension, key, value);
            }
        }
    }

    g_list_free (extensions);
}

static void
maliit_attribute_extension_registry_init (MaliitAttributeExtensionRegistry *registry)
{
    MeegoImConnector *connector = meego_im_connector_get_singleton();

    MaliitAttributeExtensionRegistryPrivate *priv = G_TYPE_INSTANCE_GET_PRIVATE (registry,
                                                                                 MALIIT_TYPE_ATTRIBUTE_EXTENSION_REGISTRY,
                                                                                 MaliitAttributeExtensionRegistryPrivate);

    priv->extensions = g_hash_table_new_full (g_direct_hash,
                                              g_direct_equal,
                                              NULL,
                                              extension_weak_unref_global);
    registry->priv = priv;

    g_signal_connect(connector->proxy, "connection-established",
                     G_CALLBACK(register_all_extensions), registry);
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
    GHashTable *extensions;
    gint id;

    g_return_if_fail (MALIIT_IS_ATTRIBUTE_EXTENSION_REGISTRY (registry));
    g_return_if_fail (MALIIT_IS_ATTRIBUTE_EXTENSION (extension));

    extensions = registry->priv->extensions;
    id = maliit_attribute_extension_get_id (extension);

    if (!g_hash_table_lookup_extended (extensions, GINT_TO_POINTER (id), NULL, NULL)) {
        MeegoImConnector *connector = meego_im_connector_get_singleton();

        g_object_weak_ref (G_OBJECT (extension),
                           extension_notify,
                           registry);

        g_hash_table_insert (extensions,
                             GINT_TO_POINTER (id),
                             extension);
        meego_im_proxy_register_extension(connector->proxy,
                                          id,
                                          maliit_attribute_extension_get_filename (extension));
    }
}

void
maliit_attribute_extension_registry_remove_extension (MaliitAttributeExtensionRegistry *registry,
                                                      MaliitAttributeExtension *extension)
{
    GHashTable *extensions;
    gint id;

    g_return_if_fail (MALIIT_IS_ATTRIBUTE_EXTENSION_REGISTRY (registry));
    g_return_if_fail (MALIIT_IS_ATTRIBUTE_EXTENSION (extension));

    extensions = registry->priv->extensions;
    id = maliit_attribute_extension_get_id (extension);

    if (g_hash_table_lookup_extended (extensions, GINT_TO_POINTER (id), NULL, NULL)) {
        MeegoImConnector *connector = meego_im_connector_get_singleton();

        g_hash_table_remove (extensions,
                             GINT_TO_POINTER (id));
        meego_im_proxy_unregister_extension(connector->proxy,
                                            id);
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
    gchar **parts;

    g_return_if_fail (MALIIT_IS_ATTRIBUTE_EXTENSION_REGISTRY (registry));
    g_return_if_fail (MALIIT_IS_ATTRIBUTE_EXTENSION (extension));
    g_return_if_fail (key != NULL);
    g_return_if_fail (value != NULL);

    parts = g_strsplit (key + 1, "/", -1);

    if (!parts)
        return;

    if (g_strv_length (parts) == 3) {
        GValue g_value = G_VALUE_INIT;

        dbus_g_value_parse_g_variant (value, &g_value);
        if (G_VALUE_TYPE (&g_value) != G_TYPE_INVALID) {
            MeegoImConnector *connector = meego_im_connector_get_singleton();
            gchar *target = g_strdup_printf ("/%s", parts[0]);

            meego_im_proxy_set_extended_attribute (connector->proxy,
                                                   maliit_attribute_extension_get_id(extension),
                                                   target,
                                                   parts[1],
                                                   parts[2],
                                                   &g_value);
            g_free (target);
            g_value_unset (&g_value);
        } else {
            g_warning ("Could not convert variant into value");
        }
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
