/* This file is part of Maliit framework
 *
 * Copyright (C) 2012 Openismus GmbH
 *
 * Contact: project@maliit.org
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

struct _MaliitAttributeExtensionRegistryPrivate
{
    GHashTable *extensions;
};

static MaliitAttributeExtensionRegistry *global_singleton;

G_DEFINE_TYPE (MaliitAttributeExtensionRegistry, maliit_attribute_extension_registry, G_TYPE_OBJECT)

enum
{
    EXTENSION_REGISTERED,
    EXTENSION_UNREGISTERED,
    EXTENSION_CHANGED,

    LAST_SIGNAL
};

guint signals[LAST_SIGNAL] = { 0 };

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

    signals[EXTENSION_REGISTERED] = g_signal_new ("extension-registered",
                                                  MALIIT_TYPE_ATTRIBUTE_EXTENSION_REGISTRY,
                                                  G_SIGNAL_RUN_FIRST,
                                                  0,
                                                  NULL,
                                                  NULL,
                                                  maliit_marshal_VOID__INT_STRING,
                                                  G_TYPE_NONE,
                                                  2,
                                                  G_TYPE_INT,
                                                  G_TYPE_STRING);

    signals[EXTENSION_UNREGISTERED] = g_signal_new ("extension-unregistered",
                                                    MALIIT_TYPE_ATTRIBUTE_EXTENSION_REGISTRY,
                                                    G_SIGNAL_RUN_FIRST,
                                                    0,
                                                    NULL,
                                                    NULL,
                                                    g_cclosure_marshal_VOID__INT,
                                                    G_TYPE_NONE,
                                                    1,
                                                    G_TYPE_INT);

    signals[EXTENSION_CHANGED] = g_signal_new ("extension-changed",
                                               MALIIT_TYPE_ATTRIBUTE_EXTENSION_REGISTRY,
                                               G_SIGNAL_RUN_FIRST,
                                               0,
                                               NULL,
                                               NULL,
                                               maliit_marshal_VOID__INT_STRING_VARIANT,
                                               G_TYPE_NONE,
                                               3,
                                               G_TYPE_INT,
                                               G_TYPE_STRING,
                                               G_TYPE_VARIANT);

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
maliit_attribute_extension_registry_init (MaliitAttributeExtensionRegistry *registry)
{
    MaliitAttributeExtensionRegistryPrivate *priv = G_TYPE_INSTANCE_GET_PRIVATE (registry,
                                                                                 MALIIT_TYPE_ATTRIBUTE_EXTENSION_REGISTRY,
                                                                                 MaliitAttributeExtensionRegistryPrivate);

    priv->extensions = g_hash_table_new_full (g_direct_hash,
                                              g_direct_equal,
                                              NULL,
                                              extension_weak_unref_global);
    registry->priv = priv;
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
        g_object_weak_ref (G_OBJECT (extension),
                           extension_notify,
                           registry);

        g_hash_table_insert (extensions,
                             GINT_TO_POINTER (id),
                             extension);
        g_signal_emit (registry,
                       signals[EXTENSION_REGISTERED],
                       0,
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
        g_hash_table_remove (extensions,
                             GINT_TO_POINTER (id));
        g_signal_emit (registry,
                       signals[EXTENSION_UNREGISTERED],
                       0,
                       id);
    }
}

void
maliit_attribute_extension_registry_extension_changed (MaliitAttributeExtensionRegistry *registry,
                                                       MaliitAttributeExtension *extension,
                                                       const gchar *key,
                                                       GVariant *value)
{
    g_return_if_fail (MALIIT_IS_ATTRIBUTE_EXTENSION_REGISTRY (registry));
    g_return_if_fail (MALIIT_IS_ATTRIBUTE_EXTENSION (extension));
    g_return_if_fail (key != NULL);
    g_return_if_fail (value != NULL);

    g_signal_emit (registry,
                   signals[EXTENSION_CHANGED],
                   0,
                   maliit_attribute_extension_get_id (extension),
                   key,
                   value);
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
