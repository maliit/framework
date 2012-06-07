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

#ifndef MALIIT_GLIB_ATTRIBUTE_EXTENSION_H
#define MALIIT_GLIB_ATTRIBUTE_EXTENSION_H

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define MALIIT_TYPE_ATTRIBUTE_EXTENSION           (maliit_attribute_extension_get_type())
#define MALIIT_ATTRIBUTE_EXTENSION(obj)           (G_TYPE_CHECK_INSTANCE_CAST(obj, MALIIT_TYPE_ATTRIBUTE_EXTENSION, MaliitAttributeExtension))
#define MALIIT_ATTRIBUTE_EXTENSION_CLASS(cls)     (G_TYPE_CHECK_CLASS_CAST(cls, MALIIT_TYPE_ATTRIBUTE_EXTENSION, MaliitAttributeExtensionClass))
#define MALIIT_IS_ATTRIBUTE_EXTENSION(obj)        (G_TYPE_CHECK_INSTANCE_TYPE(obj, MALIIT_TYPE_ATTRIBUTE_EXTENSION))
#define MALIIT_IS_ATTRIBUTE_EXTENSION_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE(obj, MALIIT_TYPE_ATTRIBUTE_EXTENSION))
#define MALIIT_ATTRIBUTE_EXTENSION_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), MALIIT_TYPE_ATTRIBUTE_EXTENSION, MaliitAttributeExtensionClass))

#define MALIIT_ATTRIBUTE_EXTENSION_DATA "maliit-attribute-extension"

#define MALIIT_ATTRIBUTE_EXTENSION_DATA_QUARK (g_quark_from_string (MALIIT_ATTRIBUTE_EXTENSION_DATA))

typedef struct _MaliitAttributeExtension MaliitAttributeExtension;
typedef struct _MaliitAttributeExtensionClass MaliitAttributeExtensionClass;
typedef struct _MaliitAttributeExtensionPrivate MaliitAttributeExtensionPrivate;

struct _MaliitAttributeExtension
{
    GObject parent;

    MaliitAttributeExtensionPrivate *priv;
};

/**
  * MaliitAttributeExtensionClass:
  * @parent_class: The parent class.
  **/
struct _MaliitAttributeExtensionClass
{
    GObjectClass parent_class;
};

GType
maliit_attribute_extension_get_type (void) G_GNUC_CONST;

MaliitAttributeExtension *
maliit_attribute_extension_new (void);

MaliitAttributeExtension *
maliit_attribute_extension_new_with_filename (const gchar* filename);

GHashTable *
maliit_attribute_extension_get_attributes (MaliitAttributeExtension *extension);

const gchar *
maliit_attribute_extension_get_filename (MaliitAttributeExtension *extension);

int
maliit_attribute_extension_get_id (MaliitAttributeExtension *extension);

void
maliit_attribute_extension_update_attribute (MaliitAttributeExtension *extension,
                                             const gchar *key,
                                             GVariant *value);

void
maliit_attribute_extension_set_attribute (MaliitAttributeExtension *extension,
                                          const gchar *key,
                                          GVariant *value);

void
maliit_attribute_extension_attach_to_object (MaliitAttributeExtension *extension,
                                             GObject *object);

G_END_DECLS

#endif /* MALIIT_GLIB_ATTRIBUTE_EXTENSION_H */
