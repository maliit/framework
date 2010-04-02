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
#include "qt-gtk-translate.h"
#include "debug.h"


static GType _dui_imcontext_type = 0;
static GtkIMContextClass *parent_class = NULL;

static DuiIMContext *focused_imcontext = NULL;
static GtkWidget *focused_widget = NULL;

static void dui_imcontext_finalize(GObject *object);

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


static gboolean
dui_imcontext_init_dbus (DuiIMContext *self)
{
	static gboolean first_time = TRUE;
	gboolean ret = TRUE;

	self->proxy = dui_im_proxy_get_singleton();
	self->dbusobj = dui_imcontext_dbusobj_get_singleton();

	if (first_time) {
		ret = dui_im_proxy_set_context_object(self->proxy,
				dui_imcontext_dbusobj_get_path(self->dbusobj));
		first_time = FALSE;
	}

	return ret;
}


GtkIMContext *
dui_imcontext_new (void)
{
	DuiIMContext *ic = DUI_IMCONTEXT(g_object_new(DUI_TYPE_IMCONTEXT, NULL));
	return GTK_IM_CONTEXT(ic);
}


static void
dui_imcontext_finalize (GObject *object)
{
	DuiIMContext *imcontext = DUI_IMCONTEXT(object);
	G_OBJECT_CLASS(parent_class)->finalize(object);

	if (imcontext->client_window)
		g_object_unref(imcontext->client_window);
}


static void
dui_imcontext_class_init (DuiIMContextClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
	parent_class = (GtkIMContextClass *)g_type_class_peek_parent(klass);
	GtkIMContextClass *imclass = GTK_IM_CONTEXT_CLASS(klass);

	gobject_class->finalize = dui_imcontext_finalize;

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
	self->client_window = NULL;

	self->cursor_location.x = -1;
	self->cursor_location.y = -1;
	self->cursor_location.width = 0;
	self->cursor_location.height = 0;

	dui_imcontext_init_dbus(self);
}


static void
dui_imcontext_focus_in (GtkIMContext *context)
{
	DuiIMContext *imcontext = DUI_IMCONTEXT(context);
	gboolean ret = TRUE;

	DBG("imcontext = %p", imcontext);

	if (focused_imcontext && focused_imcontext != imcontext)
		dui_imcontext_focus_out(GTK_IM_CONTEXT(focused_imcontext));
	focused_imcontext = imcontext;

	ret = dui_im_proxy_activate_context(imcontext->proxy);
	if (ret)
		dui_im_proxy_show_input_method(imcontext->proxy);

	// TODO: anything else than call "activateContext" and "showInputMethod" ?

}


static void
dui_imcontext_focus_out (GtkIMContext *context)
{
	DuiIMContext *imcontext = DUI_IMCONTEXT(context);
	DBG("imcontext = %p", imcontext);

	dui_im_proxy_hide_input_method(imcontext->proxy);

	// TODO: anything else than call "hideInputMethod" ?

	focused_imcontext = NULL;
	focused_widget = NULL;
}


static gboolean
dui_imcontext_filter_key_event (GtkIMContext *context, GdkEventKey *event)
{
	DuiIMContext *imcontext = DUI_IMCONTEXT(context);
	DBG("imcontext = %p", imcontext);

	focused_widget = gtk_get_event_widget((GdkEvent*)event);

	// TODO: call "processKeyEvent" and anything else?


	return TRUE;
}


static void
dui_imcontext_reset (GtkIMContext *context)
{
	DuiIMContext *imcontext = DUI_IMCONTEXT(context);
	DBG("imcontext = %p", imcontext);

	dui_im_proxy_reset(imcontext->proxy);
}


static void
dui_imcontext_get_preedit_string (GtkIMContext *context, gchar **str, PangoAttrList **attrs, gint *cursor_pos)
{
	DuiIMContext *imcontext = DUI_IMCONTEXT(context);

	// TODO: get preedit string from somewhere....

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

	// TODO: Seems QT/DUI don't need it, it will always showing preedit.
}


static void
dui_imcontext_set_client_window (GtkIMContext *context, GdkWindow *window)
{
	DuiIMContext *imcontext = DUI_IMCONTEXT(context);
	STEP();

	if (imcontext->client_window)
		g_object_unref(imcontext->client_window);

	if (window)
		g_object_ref(window);

	imcontext->client_window = window;

	// TODO: might need to update cursor position or other staff later using this info?
}


static void
dui_imcontext_set_cursor_location (GtkIMContext *context, GdkRectangle *area)
{
	DuiIMContext *imcontext = DUI_IMCONTEXT(context);
	DBG("imcontext = %p, x=%d, y=%d, w=%d, h=%d", imcontext,
		area->x, area->y, area->width, area->height);

	imcontext->cursor_location = *area;

	// TODO: call updateWidgetInformation?
	//The cursor location from GTK widget is simillar to ImMicroFocus info of a QWidget
	//Thus we might need to update Qt::ImMicroFocus info according to this. 
	//But DUI IM seems not using this info at all
}


// Call back functions for dbus obj

gboolean
dui_imcontext_client_activation_lost_event (DuiIMContextDbusObj *obj)
{
	STEP();
	return TRUE;
}


gboolean
dui_imcontext_client_im_initiated_hide (DuiIMContextDbusObj *obj)
{
	STEP();
	return TRUE;
}


gboolean
dui_imcontext_client_commit_string (DuiIMContextDbusObj *obj, char *string)
{
	DBG("string is:%s", string);
	if (focused_imcontext)
		g_signal_emit_by_name(focused_imcontext, "commit", string);

	return TRUE;
}


gboolean
dui_imcontext_client_update_preedit (DuiIMContextDbusObj *obj, char *string, int preedit_face)
{
	STEP();
	return TRUE;
}


gboolean
dui_imcontext_client_key_event (DuiIMContextDbusObj *obj, int type, int key, int modifiers, char *text,
				gboolean auto_repeat, int count)
{
	STEP();
	qt_key_event_to_gdk(type, key, modifiers);
	return TRUE;
}


gboolean
dui_imcontext_client_update_input_method_area (DuiIMContextDbusObj *obj, GPtrArray *data)
{
	STEP();
	return TRUE;
}


gboolean
dui_imcontext_client_set_global_correction_enabled (DuiIMContextDbusObj *obj, gboolean correction)
{
	STEP();
	return TRUE;
}


gboolean
dui_imcontext_client_copy (DuiIMContextDbusObj *obj)
{
	STEP();
	return TRUE;
}


gboolean
dui_imcontext_client_paste (DuiIMContextDbusObj *obj)
{
	STEP();
	return TRUE;
}


gboolean
dui_imcontext_client_set_redirect_keys (DuiIMContextDbusObj *obj, gboolean enabled)
{
	STEP();
	return TRUE;
}


gboolean
dui_imcontext_client_preedit_rectangle(DuiIMContextDbusObj *obj, GValueArray** rect, gboolean *valid)
{
	STEP();
	return TRUE;
}

