/* GTK - The GIMP Toolkit
 * Copyright (C) 2013, 2017 Jan Arne Petersen
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. If not, see <http://www.gnu.org/licenses/>.
 */

#include "gtk/gtkimmodule.h"
#include "gtkimcontextwayland.h"

static const GtkIMContextInfo wayland_im_info =
{
  "wayland",
  "Wayland Input Method",
  "wayland",
  "",
  "*"
};

static const GtkIMContextInfo *info_list[] =
{
  &wayland_im_info
};

#ifndef INCLUDE_IM_wayland
#define MODULE_ENTRY(type, function) G_MODULE_EXPORT type im_module_ ## function
#else
#define MODULE_ENTRY(type, function) type _gtk_immodule_wayland_ ## function
#endif

G_MODULE_EXPORT void im_module_init(GTypeModule *type_module)
{
  gtk_im_context_wayland_register (type_module);
}

G_MODULE_EXPORT void im_module_exit(void)
{
}

G_MODULE_EXPORT void im_module_list(const GtkIMContextInfo ***contexts,
                                    gint                     *n_contexts)
{
  *contexts = info_list;
  *n_contexts = G_N_ELEMENTS (info_list);
}

G_MODULE_EXPORT GtkIMContext * im_module_create(const gchar *context_id)
{
  if (g_strcmp0 (context_id, "wayland") == 0)
    return gtk_im_context_wayland_new ();
  else
    return NULL;
}
