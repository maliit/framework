/*
 * Copyright (C) 2010, Intel Corporation.
 *
 * Author: Raymond Liu <raymond.liu@intel.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 * 
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */

#ifndef _CLIENT_IMCONTEXT_GTK_H
#define _CLIENT_IMCONTEXT_GTK_H

#include <gtk/gtk.h>
#include "meego-im-proxy.h"
#include "meego-imcontext-dbus.h"

G_BEGIN_DECLS

// Be careful not to override the existing flag of GDK
// Currently bit 15-25 unused, so we pick a middle one.
typedef enum {
	IM_FORWARD_MASK = 1 << 20
} IMModifierType;

typedef struct _MeegoIMContext MeegoIMContext;
typedef struct _MeegoIMContextClass MeegoIMContextClass;

#define MEEGO_TYPE_IMCONTEXT		(meego_imcontext_get_type())
#define MEEGO_IMCONTEXT(obj)		(G_TYPE_CHECK_INSTANCE_CAST((obj), MEEGO_TYPE_IMCONTEXT, MeegoIMContext))
#define MEEGO_IMCONTEXT_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST((klass), MEEGO_TYPE_IMCONTEXT, MeegoIMContextClass))
#define MEEGO_IS_IMCONTEXT(obj)		(G_TYPE_CHECK_INSTANCE_TYPE((obj), MEEGO_TYPE_IMCONTEXT))
#define MEEGO_IS_IMCONTEXT_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE((klass), MEEGO_TYPE_IMCONTEXT))
#define MEEGO_IMCONTEXT_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS((obj), MEEGO_TYPE_IMCONTEXT, MeegoIMContextClass))


struct _MeegoIMContext {
	GtkIMContext parent;

	MeegoIMProxy *proxy;
	MeegoIMContextDbusObj *dbusobj;

	GdkWindow *client_window;
	GdkRectangle cursor_location;

	gchar *preedit_str;
	PangoAttrList *preedit_attrs;
	gint preedit_cursor_pos;
};

struct _MeegoIMContextClass {
	GtkIMContextClass parent;
};

GType meego_imcontext_get_type (void);

void meego_imcontext_register_type (GTypeModule *type_module);
GtkIMContext *meego_imcontext_new (void);


G_END_DECLS

#endif //_CLIENT_IMCONTEXT_GTK_H