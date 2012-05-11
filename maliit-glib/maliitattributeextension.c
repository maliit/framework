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

#include "maliitattributeextension.h"
#include "maliitattributeextensionprivate.h"
#include "maliitattributeextensionregistry.h"
#include "maliitmarshallers.h"

struct _MaliitAttributeExtensionPrivate
{
    int id;
    gchar *filename;
    GHashTable *attributes;
    MaliitAttributeExtensionRegistry *registry;
};

G_DEFINE_TYPE (MaliitAttributeExtension, maliit_attribute_extension, G_TYPE_OBJECT)

enum
{
    EXTENDED_ATTRIBUTE_CHANGED,

    LAST_SIGNAL
};

enum
{
    PROP_0,

    PROP_ID,
    PROP_FILENAME,
    PROP_ATTRIBUTES
};

static guint signals[LAST_SIGNAL] = { 0 };

static void
maliit_attribute_extension_finalize (GObject *object)
{
    MaliitAttributeExtension *extension = MALIIT_ATTRIBUTE_EXTENSION (object);
    MaliitAttributeExtensionPrivate *priv = extension->priv;

    g_free (priv->filename);

    G_OBJECT_CLASS (maliit_attribute_extension_parent_class)->finalize (object);
}

static void
maliit_attribute_extension_dispose (GObject *object)
{
    MaliitAttributeExtension *extension = MALIIT_ATTRIBUTE_EXTENSION (object);
    MaliitAttributeExtensionPrivate *priv = extension->priv;

    if (priv->registry) {
        MaliitAttributeExtensionRegistry *registry = priv->registry;

        priv->registry = NULL;
        maliit_attribute_extension_registry_remove_extension (registry,
                                                              extension);
        g_object_unref (registry);
    }

    if (priv->attributes) {
        GHashTable *attributes = priv->attributes;

        priv->attributes = NULL;
        g_hash_table_unref (attributes);
    }

    G_OBJECT_CLASS (maliit_attribute_extension_parent_class)->dispose (object);
}

static void
maliit_attribute_extension_set_property (GObject *object,
                                         guint prop_id,
                                         const GValue *value,
                                         GParamSpec *pspec)
{
    MaliitAttributeExtension *extension = MALIIT_ATTRIBUTE_EXTENSION (object);

    switch (prop_id) {
    case PROP_FILENAME:
        g_free (extension->priv->filename);
        extension->priv->filename = g_value_dup_string (value);
        break;
        /* PROP_ID and PROP_ATTRIBUTES are read only. */
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
        break;
    }
}

static void
maliit_attribute_extension_get_property (GObject *object,
                                         guint prop_id,
                                         GValue *value,
                                         GParamSpec *pspec)
{
    MaliitAttributeExtension *extension = MALIIT_ATTRIBUTE_EXTENSION (object);
    MaliitAttributeExtensionPrivate *priv = extension->priv;

    switch (prop_id) {
    case PROP_ID:
        g_value_set_int (value, priv->id);
        break;
    case PROP_FILENAME:
        g_value_set_string (value, priv->filename);
        break;
    case PROP_ATTRIBUTES:
        g_value_set_boxed (value, priv->attributes);
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
        break;
    }
}

static void
maliit_attribute_extension_constructed (GObject *object)
{
    MaliitAttributeExtension *extension = MALIIT_ATTRIBUTE_EXTENSION (object);
    MaliitAttributeExtensionPrivate *priv = extension->priv;

    maliit_attribute_extension_registry_add_extension (priv->registry,
                                                       extension);

    G_OBJECT_CLASS (maliit_attribute_extension_parent_class)->constructed (object);
}

static void
maliit_attribute_extension_class_init (MaliitAttributeExtensionClass *extension_class)
{
    GObjectClass *g_object_class = G_OBJECT_CLASS (extension_class);

    g_object_class->finalize = maliit_attribute_extension_finalize;
    g_object_class->dispose = maliit_attribute_extension_dispose;
    g_object_class->set_property = maliit_attribute_extension_set_property;
    g_object_class->get_property = maliit_attribute_extension_get_property;
    g_object_class->constructed = maliit_attribute_extension_constructed;

    /**
     * MaliitAttributeExtension:id:
     *
     * ID of the extension.
     */
    g_object_class_install_property (g_object_class,
                                     PROP_ID,
                                     g_param_spec_int ("id",
                                                       "ID", /* TODO: mark as translatable? */
                                                       "ID of the extension", /* TODO: mark as translatable? */
                                                       0,
                                                       G_MAXINT,
                                                       0,
                                                       G_PARAM_READABLE |
                                                       G_PARAM_STATIC_NAME |
                                                       G_PARAM_STATIC_BLURB |
                                                       G_PARAM_STATIC_NICK));

    /**
     * MaliitAttributeExtension:id:
     *
     * Path to file where definitions of overrides are defined.
     */
    g_object_class_install_property (g_object_class,
                                     PROP_FILENAME,
                                     g_param_spec_string ("filename",
                                                          "Filename", /* TODO: mark as translatable? */
                                                          "Filename of the extension", /* TODO: mark as translatable? */
                                                          NULL,
                                                          G_PARAM_READABLE |
                                                          G_PARAM_WRITABLE |
                                                          G_PARAM_CONSTRUCT_ONLY |
                                                          G_PARAM_STATIC_NAME |
                                                          G_PARAM_STATIC_BLURB |
                                                          G_PARAM_STATIC_NICK));

    g_object_class_install_property (g_object_class,
                                     PROP_ATTRIBUTES,
                                     g_param_spec_boxed ("attributes",
                                                         "Attributes", /* TODO: mark as translatable? */
                                                         "Attributes overrides", /* TODO: mark as translatable? */
                                                         G_TYPE_HASH_TABLE,
                                                         G_PARAM_READABLE |
                                                         G_PARAM_STATIC_NAME |
                                                         G_PARAM_STATIC_BLURB |
                                                         G_PARAM_STATIC_NICK));

    /**
     * MaliitAttributeExtension::extended-attribute-changed:
     * @extension: The #MaliitAttributeExtension emitting the signal.
     * @key: A string specifying the target for the attribute.
     * @value: A new value.
     *
     * Informs application that input method server has changed the
     * extended attribute.
     */
    signals[EXTENDED_ATTRIBUTE_CHANGED] =
        g_signal_new ("extended-attribute-changed",
                      MALIIT_TYPE_ATTRIBUTE_EXTENSION,
                      G_SIGNAL_RUN_FIRST,
                      0,
                      NULL,
                      NULL,
                      maliit_marshal_VOID__STRING_VARIANT,
                      G_TYPE_NONE,
                      2,
                      G_TYPE_STRING,
                      G_TYPE_VARIANT);

    g_type_class_add_private (extension_class, sizeof (MaliitAttributeExtensionPrivate));
}

static void
maliit_attribute_extension_init (MaliitAttributeExtension *extension)
{
    static int id_counter = 0;
    MaliitAttributeExtensionPrivate *priv = G_TYPE_INSTANCE_GET_PRIVATE (extension,
                                                                         MALIIT_TYPE_ATTRIBUTE_EXTENSION,
                                                                         MaliitAttributeExtensionPrivate);

    priv->id = id_counter++;
    priv->filename = NULL;
    priv->attributes = g_hash_table_new_full (g_str_hash,
                                              g_str_equal,
                                              g_free,
                                              (GDestroyNotify) g_variant_unref);
    priv->registry = maliit_attribute_extension_registry_get_instance ();

    extension->priv = priv;
}

/**
 * maliit_attribute_extension_new:
 *
 * Creates new attribute extension, which is not associated with any file.
 *
 * Returns: (transfer full): The newly created
 * #MaliitAttributeExtension.
 */
MaliitAttributeExtension *
maliit_attribute_extension_new (void)
{
    return MALIIT_ATTRIBUTE_EXTENSION (g_object_new (MALIIT_TYPE_ATTRIBUTE_EXTENSION,
                                                     NULL));
}

/**
 * maliit_attribute_extension_new_with_filename:
 * @filename: (transfer none) (type filename): Filename where overrides are stored.
 *
 * Creates new attribute extension, which is associated with file
 * given as @filename.
 *
 * Returns: (transfer full): The newly created
 * #MaliitAttributeExtension.
 */
MaliitAttributeExtension *
maliit_attribute_extension_new_with_filename (const gchar *filename)
{
    return MALIIT_ATTRIBUTE_EXTENSION (g_object_new (MALIIT_TYPE_ATTRIBUTE_EXTENSION,
                                                     "filename", filename,
                                                     NULL));
}

/**
 * maliit_attribute_extension_get_attributes:
 * @extension: (transfer none): The #MaliitAttributeExtension which attributes you want to get.
 *
 * Gets all attributes of this extension that were set previously with
 * maliit_attribute_extension_set_attribute().
 *
 * Returns: (transfer none) (element-type utf8 GLib.Variant): The #GHashTable
 * containing strings as keys and #GVariant<!-- -->s as values. Should not be
 * freed nor modified.
 */
GHashTable *
maliit_attribute_extension_get_attributes (MaliitAttributeExtension *extension)
{
    g_return_val_if_fail (MALIIT_IS_ATTRIBUTE_EXTENSION (extension), NULL);

    return extension->priv->attributes;
}

/**
 * maliit_attribute_extension_get_filename:
 * @extension: (transfer none): The #MaliitAttributeExtension which filename you want to get.
 *
 * Gets filename of this extension that were set previously with
 * maliit_attribute_extension_new_with_filename().
 *
 * Returns: (transfer none) (type filename): The string being a
 * filename of this extension or %NULL. Returned string should not be
 * freed nor modified.
 */
const gchar *
maliit_attribute_extension_get_filename (MaliitAttributeExtension *extension)
{
    g_return_val_if_fail (MALIIT_IS_ATTRIBUTE_EXTENSION (extension), NULL);

    return extension->priv->filename;
}

/**
 * maliit_attribute_extension_get_id:
 * @extension: (transfer none): The #MaliitAttributeExtension which ID you want to get.
 *
 * Gets ID of this extension.
 *
 * Returns: The ID of this extension.
 */
int
maliit_attribute_extension_get_id (MaliitAttributeExtension *extension)
{
    g_return_val_if_fail (MALIIT_IS_ATTRIBUTE_EXTENSION (extension), -1);

    return extension->priv->id;
}

/**
 * maliit_attribute_extension_update_attribute:
 * @extension: (transfer none): The #MaliitAttributeExtension which attribute you want to update.
 * @key: (transfer none): Attribute name to update.
 * @value: (transfer none): Attribute value to update.
 *
 * Updates the @extension's attribute described by @key with
 * @value. This function always emits a
 * #MaliitAttributeExtension::extended-attribute-changed signal.
 */
void
maliit_attribute_extension_update_attribute (MaliitAttributeExtension *extension,
                                             const gchar *key,
                                             GVariant *value)
{
    g_return_if_fail (MALIIT_IS_ATTRIBUTE_EXTENSION (extension));
    g_return_if_fail (key != NULL);
    g_return_if_fail (value != NULL);

    g_hash_table_replace (extension->priv->attributes,
                          g_strdup (key),
                          g_variant_ref (value));

    g_signal_emit (extension,
                   signals[EXTENDED_ATTRIBUTE_CHANGED],
                   0,
                   key,
                   value);
}

/**
 * maliit_attribute_extension_set_attribute:
 * @extension: (transfer none): The #MaliitAttributeExtension which attribute you want to set.
 * @key: (transfer none): Attribute name to update.
 * @value: (transfer none): Attribute value to update.
 *
 * Sets an attribute in @extension described by @key to value in @value.
 */
void maliit_attribute_extension_set_attribute (MaliitAttributeExtension *extension,
                                               const gchar *key,
                                               GVariant *value)
{
    MaliitAttributeExtensionPrivate *priv;
    GHashTable *attributes;
    GVariant *orig_value;

    g_return_if_fail (MALIIT_IS_ATTRIBUTE_EXTENSION (extension));
    g_return_if_fail (key != NULL);
    g_return_if_fail (value != NULL);

    priv = extension->priv;
    attributes = priv->attributes;

    if (!g_hash_table_lookup_extended (attributes, key, NULL, (gpointer *)&orig_value) ||
        !g_variant_equal (orig_value, value)) {

        g_hash_table_replace (attributes,
                              g_strdup (key),
                              g_variant_ref (value));

        maliit_attribute_extension_registry_extension_changed (priv->registry,
                                                               extension,
                                                               key,
                                                               value);
    }
}

/**
 * maliit_attribute_extension_attach_to_object:
 * @extension: (transfer none): The #MaliitAttributeExtension which you want to be attached.
 * @object: (transfer none): The #GObject to which @extension will be attached.
 *
 * Attaches @extension to @object, so input context can retrieve it
 * from @object. Note that attaching extensions to non-input
 * #GObject<!-- -->s does not have much sense.
 */
void
maliit_attribute_extension_attach_to_object (MaliitAttributeExtension *extension,
                                             GObject *object)
{
    g_return_if_fail (MALIIT_IS_ATTRIBUTE_EXTENSION (extension));
    g_return_if_fail (G_IS_OBJECT (object));

    g_object_set_qdata_full (object, MALIIT_ATTRIBUTE_EXTENSION_DATA_QUARK,
                             extension, g_object_unref);
}
