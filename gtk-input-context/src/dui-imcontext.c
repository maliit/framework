/*
 * Copyright (C) 2010, Intel Corporation.
 *
 * Author: Raymond Liu <raymond.liu@intel.com>
 * 
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

#include "dui-imcontext.h"
#include "debug.h"

static GType _dui_imcontext_type = 0;

static void dui_imcontext_class_init (DuiIMContextClass *klass);
static void dui_imcontext_init (DuiIMContext *dui_imcontext);

static void dui_imcontext_focus_in (GtkIMContext *context);
static void dui_imcontext_focus_out (GtkIMContext *context);
static gboolean dui_imcontext_filter_key_event (GtkIMContext *context, GdkEventKey *event);
static void dui_imcontext_reset (GtkIMContext *context);
static void dui_imcontext_get_preedit_string (GtkIMContext *context, gchar **str, PangoAttrList **attrs, gint *cursor_pos);
static void dui_imcontext_set_preedit_enabled (GtkIMContext *context, gboolean enabled);
static void dui_imcontext_set_client_window (GtkIMContext *context, GdkWindow *window);
static void dui_imcontext_set_cursor_location (GtkIMContext *context, GdkRectangle *area);


GType dui_imcontext_get_type ()
{
    return _dui_imcontext_type;
}


void
dui_imcontext_register_type (GTypeModule *type_module)
{
	static const GTypeInfo dui_imcontext_info = {
		sizeof (DuiIMContextClass),
		(GBaseInitFunc) NULL,
		(GBaseFinalizeFunc) NULL,
		(GClassInitFunc) dui_imcontext_class_init,
		NULL,
		NULL,
		sizeof (DuiIMContext),
		0,
		(GInstanceInitFunc) dui_imcontext_init,
	};

	if (_dui_imcontext_type)
		return;

        if (type_module) {
		_dui_imcontext_type = 
			g_type_module_register_type(
				type_module,
				GTK_TYPE_IM_CONTEXT,
				"DuiIMContext",
				&dui_imcontext_info,
				(GTypeFlags)0);
        } else {
		_dui_imcontext_type =
			g_type_register_static(
				GTK_TYPE_IM_CONTEXT,
				"DuiIMContext",
				&dui_imcontext_info,
				(GTypeFlags)0);
        }
}


GtkIMContext *
dui_imcontext_new (void)
{
	DuiIMContext *ic = DUI_IMCONTEXT(g_object_new(DUI_TYPE_IMCONTEXT, NULL));
	return GTK_IM_CONTEXT(ic);
}


static void
dui_imcontext_class_init (DuiIMContextClass *klass)
{
	GtkIMContextClass *imclass = GTK_IM_CONTEXT_CLASS (klass);

	imclass->focus_in = dui_imcontext_focus_in;
	imclass->focus_out = dui_imcontext_focus_out;
	imclass->filter_keypress = dui_imcontext_filter_key_event;
	imclass->reset = dui_imcontext_reset;
	imclass->set_client_window = dui_imcontext_set_client_window;
	imclass->get_preedit_string = dui_imcontext_get_preedit_string;
	imclass->set_cursor_location = dui_imcontext_set_cursor_location;
	imclass->set_use_preedit = dui_imcontext_set_preedit_enabled;

}


static void
dui_imcontext_init (DuiIMContext *self)
{
	self->proxy = dui_im_proxy_get_singleton();
	self->dbusobj = dui_imcontext_dbusobj_get_singleton();
}


static void
dui_imcontext_focus_in (GtkIMContext *context)
{
	DuiIMContext *imcontext = DUI_IMCONTEXT(context);
	DBG("imcontext = %p", imcontext);

}


static void
dui_imcontext_focus_out (GtkIMContext *context)
{
	DuiIMContext *imcontext = DUI_IMCONTEXT(context);
	DBG("imcontext = %p", imcontext);
}


static gboolean
dui_imcontext_filter_key_event (GtkIMContext *context, GdkEventKey *event)
{
	DuiIMContext *imcontext = DUI_IMCONTEXT(context);
	DBG("imcontext = %p", imcontext);
	return TRUE;
}


static void
dui_imcontext_reset (GtkIMContext *context)
{
	DuiIMContext *imcontext = DUI_IMCONTEXT(context);
	DBG("imcontext = %p", imcontext);
}


static void
dui_imcontext_get_preedit_string (GtkIMContext *context, gchar **str, PangoAttrList **attrs, gint *cursor_pos)
{
	DuiIMContext *imcontext = DUI_IMCONTEXT(context);
	DBG("imcontext = %p", imcontext);
	if (str)
		*str = g_strdup ("");
	if (attrs)
		*attrs = pango_attr_list_new();
	if (cursor_pos)
		*cursor_pos = 0;
}


static void
dui_imcontext_set_preedit_enabled (GtkIMContext *context, gboolean enabled)
{
	DuiIMContext *imcontext = DUI_IMCONTEXT(context);
	DBG("imcontext = %p", imcontext);
}


static void
dui_imcontext_set_client_window (GtkIMContext *context, GdkWindow *window)
{
	DuiIMContext *imcontext = DUI_IMCONTEXT(context);
	DBG("imcontext = %p", imcontext);
}


static void
dui_imcontext_set_cursor_location (GtkIMContext *context, GdkRectangle *area)
{
	DuiIMContext *imcontext = DUI_IMCONTEXT(context);
	DBG("imcontext = %p", imcontext);
}

