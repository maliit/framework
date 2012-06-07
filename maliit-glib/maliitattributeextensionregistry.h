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

#ifndef MALIIT_GLIB_ATTRIBUTE_EXTENSION_REGISTRY_H
#define MALIIT_GLIB_ATTRIBUTE_EXTENSION_REGISTRY_H

#include <glib.h>
#include <glib-object.h>

#include "maliitattributeextension.h"

G_BEGIN_DECLS

#define MALIIT_TYPE_ATTRIBUTE_EXTENSION_REGISTRY           (maliit_attribute_extension_registry_get_type())
#define MALIIT_ATTRIBUTE_EXTENSION_REGISTRY(obj)           (G_TYPE_CHECK_INSTANCE_CAST(obj, MALIIT_TYPE_ATTRIBUTE_EXTENSION_REGISTRY, MaliitAttributeExtensionRegistry))
#define MALIIT_ATTRIBUTE_EXTENSION_REGISTRY_CLASS(cls)     (G_TYPE_CHECK_CLASS_CAST(cls, MALIIT_TYPE_ATTRIBUTE_EXTENSION_REGISTRY, MaliitAttributeExtensionRegistryClass))
#define MALIIT_IS_ATTRIBUTE_EXTENSION_REGISTRY(obj)        (G_TYPE_CHECK_INSTANCE_TYPE(obj, MALIIT_TYPE_ATTRIBUTE_EXTENSION_REGISTRY))
#define MALIIT_IS_ATTRIBUTE_EXTENSION_REGISTRY_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE(obj, MALIIT_TYPE_ATTRIBUTE_EXTENSION_REGISTRY))
#define MALIIT_ATTRIBUTE_EXTENSION_REGISTRY_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), MALIIT_TYPE_ATTRIBUTE_EXTENSION_REGISTRY, MaliitAttributeExtensionRegistryClass))

typedef struct _MaliitAttributeExtensionRegistry MaliitAttributeExtensionRegistry;
typedef struct _MaliitAttributeExtensionRegistryClass MaliitAttributeExtensionRegistryClass;
typedef struct _MaliitAttributeExtensionRegistryPrivate MaliitAttributeExtensionRegistryPrivate;

struct _MaliitAttributeExtensionRegistry
{
    GObject parent;

    MaliitAttributeExtensionRegistryPrivate *priv;
};

struct _MaliitAttributeExtensionRegistryClass
{
    GObjectClass parent_class;
};

GType
maliit_attribute_extension_registry_get_type (void) G_GNUC_CONST;

MaliitAttributeExtensionRegistry *
maliit_attribute_extension_registry_get_instance (void);

void
maliit_attribute_extension_registry_add_extension (MaliitAttributeExtensionRegistry *registry,
                                                   MaliitAttributeExtension *extension);

void
maliit_attribute_extension_registry_remove_extension (MaliitAttributeExtensionRegistry *registry,
                                                      MaliitAttributeExtension *extension);

void
maliit_attribute_extension_registry_extension_changed (MaliitAttributeExtensionRegistry *registry,
                                                       MaliitAttributeExtension *extension,
                                                       const gchar *key,
                                                       GVariant *value);

GList *
maliit_attribute_extension_registry_get_extensions (MaliitAttributeExtensionRegistry *registry);

void
maliit_attribute_extension_registry_update_attribute (MaliitAttributeExtensionRegistry *registry,
                                                      gint id,
                                                      const gchar *target,
                                                      const gchar *target_item,
                                                      const gchar *attribute,
                                                      GVariant *value);

G_END_DECLS

#endif /* MALIIT_GLIB_ATTRIBUTE_EXTENSION_REGISTRY_H */
