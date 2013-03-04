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

#ifndef MALIIT_GLIB_SETTINGS_MANAGER_H
#define MALIIT_GLIB_SETTINGS_MANAGER_H

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define MALIIT_TYPE_SETTINGS_MANAGER           (maliit_settings_manager_get_type())
#define MALIIT_SETTINGS_MANAGER(obj)           (G_TYPE_CHECK_INSTANCE_CAST(obj, MALIIT_TYPE_SETTINGS_MANAGER, MaliitSettingsManager))
#define MALIIT_SETTINGS_MANAGER_CLASS(cls)     (G_TYPE_CHECK_CLASS_CAST(cls, MALIIT_TYPE_SETTINGS_MANAGER, MaliitSettingsManagerClass))
#define MALIIT_IS_SETTINGS_MANAGER(obj)        (G_TYPE_CHECK_INSTANCE_TYPE(obj, MALIIT_TYPE_SETTINGS_MANAGER))
#define MALIIT_IS_SETTINGS_MANAGER_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE(obj, MALIIT_TYPE_SETTINGS_MANAGER))
#define MALIIT_SETTINGS_MANAGER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), MALIIT_TYPE_SETTINGS_MANAGER, MaliitSettingsManagerClass))

typedef struct _MaliitSettingsManager MaliitSettingsManager;
typedef struct _MaliitSettingsManagerClass MaliitSettingsManagerClass;
typedef struct _MaliitSettingsManagerPrivate MaliitSettingsManagerPrivate;

struct _MaliitSettingsManager
{
    GObject parent;

    /*< private >*/
    MaliitSettingsManagerPrivate *priv;
};

/**
 * MaliitSettingsManagerClass:
 * @parent_class: The parent class.
 */
struct _MaliitSettingsManagerClass
{
    GObjectClass parent_class;
};

GType
maliit_settings_manager_get_type (void) G_GNUC_CONST;

MaliitSettingsManager *
maliit_settings_manager_new (void);

void
maliit_settings_manager_load_plugin_settings (MaliitSettingsManager *manager);

void
maliit_settings_manager_set_preferred_description_locale (const gchar *locale_name);

const gchar *
maliit_settings_manager_get_preferred_description_locale (void);

G_END_DECLS

#endif /* MALIIT_GLIB_SETTINGS_MANAGER_H */
