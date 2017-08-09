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

#ifndef WAYLAND_IMCONTEXT_H
#define WAYLAND_IMCONTEXT_H

#include <gtk/gtk.h>
#include <gtk/gtkimmodule.h>

G_BEGIN_DECLS

#define GTK_TYPE_IM_CONTEXT_WAYLAND            (gtk_im_context_wayland_get_type ())
#define GTK_IM_CONTEXT_WAYLAND(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_TYPE_IM_CONTEXT_WAYLAND, GtkIMContextWayland))
#define GTK_IM_CONTEXT_WAYLAND_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GTK_TYPE_IM_CONTEXT_WAYLAND, GtkIMContextWaylandClass))
#define GTK_IS_IM_CONTEXT_WAYLAND(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_TYPE_IM_CONTEXT_WAYLAND))
#define GTK_IS_IM_CONTEXT_WAYLAND_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GTK_TYPE_IM_CONTEXT_WAYLAND))
#define GTK_IM_CONTEXT_WAYLAND_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), GTK_TYPE_IM_CONTEXT_WAYLAND, GtkIMContextWaylandClass))

typedef struct _GtkIMContextWayland GtkIMContextWayland;
typedef struct _GtkIMContextWaylandClass GtkIMContextWaylandClass;
typedef struct _GtkIMContextWaylandPrivate GtkIMContextWaylandPrivate;

struct _GtkIMContextWayland
{
  GtkIMContext parent_instance;

  /*< private >*/
  GtkIMContextWaylandPrivate *priv;
};

struct _GtkIMContextWaylandClass
{
  /*< private >*/
  GtkIMContextClass parent_class;
};

void          gtk_im_context_wayland_register (GTypeModule *type_module);
GtkIMContext *gtk_im_context_wayland_new      (void);

G_END_DECLS

#endif
