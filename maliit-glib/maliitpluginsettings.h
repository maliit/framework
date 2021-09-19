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

#ifndef MALIIT_GLIB_PLUGIN_SETTINGS_H
#define MALIIT_GLIB_PLUGIN_SETTINGS_H

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define MALIIT_TYPE_PLUGIN_SETTINGS           (maliit_plugin_settings_get_type())
#define MALIIT_PLUGIN_SETTINGS(obj)           (G_TYPE_CHECK_INSTANCE_CAST(obj, MALIIT_TYPE_PLUGIN_SETTINGS, MaliitPluginSettings))
#define MALIIT_PLUGIN_SETTINGS_CLASS(cls)     (G_TYPE_CHECK_CLASS_CAST(cls, MALIIT_TYPE_PLUGIN_SETTINGS, MaliitPluginSettingsClass))
#define MALIIT_IS_PLUGIN_SETTINGS(obj)        (G_TYPE_CHECK_INSTANCE_TYPE(obj, MALIIT_TYPE_PLUGIN_SETTINGS))
#define MALIIT_IS_PLUGIN_SETTINGS_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE(obj, MALIIT_TYPE_PLUGIN_SETTINGS))
#define MALIIT_PLUGIN_SETTINGS_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), MALIIT_TYPE_PLUGIN_SETTINGS, MaliitPluginSettingsClass))

typedef struct _MaliitPluginSettings MaliitPluginSettings;
typedef struct _MaliitPluginSettingsClass MaliitPluginSettingsClass;
typedef struct _MaliitPluginSettingsPrivate MaliitPluginSettingsPrivate;

struct _MaliitPluginSettings
{
    GObject parent;

    /*< private >*/
    MaliitPluginSettingsPrivate *priv;
};

/**
 * MaliitPluginSettingsClass:
 * @parent_class: The parent class.
 */
struct _MaliitPluginSettingsClass
{
    GObjectClass parent_class;
};

GType
maliit_plugin_settings_get_type (void) G_GNUC_CONST;

const gchar *
maliit_plugin_settings_get_description_language (MaliitPluginSettings *settings);

const gchar *
maliit_plugin_settings_get_plugin_name (MaliitPluginSettings *settings);

const gchar *
maliit_plugin_settings_get_plugin_description (MaliitPluginSettings *settings);

GPtrArray *
maliit_plugin_settings_get_configuration_entries (MaliitPluginSettings *settings);

G_END_DECLS

#endif /* MALIIT_GLIB_PLUGIN_SETTINGS_H */
