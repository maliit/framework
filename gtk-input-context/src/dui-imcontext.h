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

#ifndef _DUI_IMCONTEXT_H
#define _DUI_IMCONTEXT_H

#include <gtk/gtk.h>
#include "dui-im-proxy.h"
#include "dui-imcontext-dbus.h"

G_BEGIN_DECLS

typedef struct _DuiIMContext DuiIMContext;
typedef struct _DuiIMContextClass DuiIMContextClass;

#define DUI_TYPE_IMCONTEXT		(dui_imcontext_get_type())
#define DUI_IMCONTEXT(obj)		(G_TYPE_CHECK_INSTANCE_CAST((obj), DUI_TYPE_IMCONTEXT, DuiIMContext))
#define DUI_IMCONTEXT_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST((klass), DUI_TYPE_IMCONTEXT, DuiIMContextClass))
#define DUI_IS_IMCONTEXT(obj)		(G_TYPE_CHECK_INSTANCE_TYPE((obj), DUI_TYPE_IMCONTEXT))
#define DUI_IS_IMCONTEXT_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE((klass), DUI_TYPE_IMCONTEXT))
#define DUI_IMCONTEXT_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS((obj), DUI_TYPE_IMCONTEXT, DuiIMContextClass))


struct _DuiIMContext {
	GtkIMContext parent;

	DuiIMProxy *proxy;
	DuiIMContextDbusObj *dbusobj;

	GdkWindow *client_window;
	GdkRectangle cursor_location;
};

struct _DuiIMContextClass {
	GtkIMContextClass parent;
};

GType dui_imcontext_get_type (void);

void dui_imcontext_register_type (GTypeModule *type_module);
GtkIMContext *dui_imcontext_new (void);


G_END_DECLS

#endif //_DUI_IMCONTEXT_H