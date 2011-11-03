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


#include <X11/keysym.h>
#include <gdk/gdkx.h> // For retrieving XID

#include "client-imcontext-gtk.h"
#include "qt-gtk-translate.h"
#include "debug.h"


static GType _meego_imcontext_type = 0;
static GtkIMContextClass *parent_class = NULL;

static MeegoIMContext *focused_imcontext = NULL;
static GtkWidget *focused_widget = NULL;

gboolean redirect_keys = FALSE;

static void meego_imcontext_finalize(GObject *object);

static void meego_imcontext_class_init (MeegoIMContextClass *klass);
static void meego_imcontext_init (MeegoIMContext *meego_imcontext);

static void meego_imcontext_focus_in (GtkIMContext *context);
static void meego_imcontext_focus_out (GtkIMContext *context);
static gboolean meego_imcontext_filter_key_event (GtkIMContext *context, GdkEventKey *event);
static void meego_imcontext_reset (GtkIMContext *context);
static void meego_imcontext_get_preedit_string (GtkIMContext *context, gchar **str, PangoAttrList **attrs, gint *cursor_pos);
static void meego_imcontext_set_preedit_enabled (GtkIMContext *context, gboolean enabled);
static void meego_imcontext_set_client_window (GtkIMContext *context, GdkWindow *window);
static void meego_imcontext_set_cursor_location (GtkIMContext *context, GdkRectangle *area);

static void meego_imcontext_update_widget_info(MeegoIMContext *imcontext);

static GtkIMContext *meego_imcontext_get_slave_imcontext (void);

static const gchar* const WIDGET_INFO_WIN_ID = "winId";
static const gchar* const WIDGET_INFO_FOCUS_STATE = "focusState";

void destroy_g_value(GValue *value)
{
    g_value_unset(value);
    g_free(value);
}

GType meego_imcontext_get_type ()
{
    return _meego_imcontext_type;
}


void
meego_imcontext_register_type (GTypeModule *type_module)
{
	static const GTypeInfo meego_imcontext_info = {
		sizeof (MeegoIMContextClass),
		(GBaseInitFunc) NULL,
		(GBaseFinalizeFunc) NULL,
		(GClassInitFunc) meego_imcontext_class_init,
		NULL,
		NULL,
		sizeof (MeegoIMContext),
		0,
		(GInstanceInitFunc) meego_imcontext_init,
	};

	if (_meego_imcontext_type)
		return;

	if (type_module) {
		_meego_imcontext_type = 
			g_type_module_register_type(
				type_module,
				GTK_TYPE_IM_CONTEXT,
				"MeegoIMContext",
				&meego_imcontext_info,
				(GTypeFlags)0);
	} else {
		_meego_imcontext_type =
			g_type_register_static(
				GTK_TYPE_IM_CONTEXT,
				"MeegoIMContext",
				&meego_imcontext_info,
				(GTypeFlags)0);
	}
}


static gboolean
meego_imcontext_init_dbus (MeegoIMContext *self)
{
	static gboolean first_time = TRUE;
	gboolean ret = TRUE;

	self->dbusobj = meego_imcontext_dbusobj_get_singleton();
	self->proxy = meego_im_proxy_get_singleton();

	if (!self->dbusobj || !self->proxy) {
		return FALSE;
	}

	meego_im_proxy_set_connection (self->proxy,
				       meego_imcontext_dbusobj_get_connection(self->dbusobj));

	if (first_time) {
		ret = meego_im_proxy_set_context_object(self->proxy,
				meego_imcontext_dbusobj_get_path(self->dbusobj));
		first_time = FALSE;
	}

	return ret;
}


// staff for fallback slave GTK simple imcontext
static void
slave_commit (GtkIMContext *slave, const char *text, gpointer data)
{
	DBG("text = %s", text);
	if (focused_imcontext && text) {
		g_signal_emit_by_name(focused_imcontext, "commit", text);
	}
}


static void
slave_preedit_changed (GtkIMContext *slave, gpointer data)
{
	gchar *str = NULL;
	gint cursor_pos = 0;
	PangoAttrList *attrs = NULL;

	STEP();
	if (!focused_imcontext || !slave)
		return;

	gtk_im_context_get_preedit_string(slave, &str, &attrs, &cursor_pos);

	if (str != NULL) {
		g_free(focused_imcontext->preedit_str);
		focused_imcontext->preedit_str = str;
	}

	focused_imcontext->preedit_cursor_pos = cursor_pos;

	if (focused_imcontext->preedit_attrs != NULL)
		pango_attr_list_unref(focused_imcontext->preedit_attrs);

	focused_imcontext->preedit_attrs = attrs;

	g_signal_emit_by_name(focused_imcontext, "preedit-changed");
}


static GtkIMContext *
meego_imcontext_get_slave_imcontext (void)
{
	static GtkIMContext *slave_ic = NULL;

	if (!slave_ic) {
		slave_ic = gtk_im_context_simple_new ();
		//g_signal_connect(G_OBJECT(slave_ic), "preedit-start", G_CALLBACK(slave_preedit_start), NULL);
		//g_signal_connect(G_OBJECT(slave_ic), "preedit-end", G_CALLBACK(slave_preedit_end), NULL);
		g_signal_connect(G_OBJECT(slave_ic), "preedit-changed", G_CALLBACK(slave_preedit_changed), NULL);
		g_signal_connect(G_OBJECT(slave_ic), "commit", G_CALLBACK(slave_commit), NULL);
	}

	return slave_ic;
}


GtkIMContext *
meego_imcontext_new (void)
{
	MeegoIMContext *ic = MEEGO_IMCONTEXT(g_object_new(MEEGO_TYPE_IMCONTEXT, NULL));
	return GTK_IM_CONTEXT(ic);
}


static void
meego_imcontext_finalize (GObject *object)
{
	MeegoIMContext *imcontext = MEEGO_IMCONTEXT(object);

	g_hash_table_destroy(imcontext->widget_state);

	if (imcontext->client_window)
		g_object_unref(imcontext->client_window);

	G_OBJECT_CLASS(parent_class)->finalize(object);
}


static void
meego_imcontext_class_init (MeegoIMContextClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
	parent_class = (GtkIMContextClass *)g_type_class_peek_parent(klass);
	GtkIMContextClass *imclass = GTK_IM_CONTEXT_CLASS(klass);

	gobject_class->finalize = meego_imcontext_finalize;

	imclass->focus_in = meego_imcontext_focus_in;
	imclass->focus_out = meego_imcontext_focus_out;
	imclass->filter_keypress = meego_imcontext_filter_key_event;
	imclass->reset = meego_imcontext_reset;
	imclass->set_client_window = meego_imcontext_set_client_window;
	imclass->get_preedit_string = meego_imcontext_get_preedit_string;
	imclass->set_cursor_location = meego_imcontext_set_cursor_location;
	imclass->set_use_preedit = meego_imcontext_set_preedit_enabled;
}


static void
meego_imcontext_init (MeegoIMContext *self)
{
	self->client_window = NULL;

	self->cursor_location.x = -1;
	self->cursor_location.y = -1;
	self->cursor_location.width = 0;
	self->cursor_location.height = 0;

	self->preedit_str = NULL;
	self->preedit_attrs = NULL;
	self->preedit_cursor_pos = 0;

	self->widget_state = g_hash_table_new_full(&g_str_hash, &g_str_equal,
				&g_free, (GDestroyNotify)destroy_g_value);
	self->focus_state = FALSE;

	meego_imcontext_init_dbus(self);
}


static void
meego_imcontext_focus_in (GtkIMContext *context)
{
	MeegoIMContext *imcontext = MEEGO_IMCONTEXT(context);
	gboolean ret = TRUE;
	gboolean focus_changed = TRUE;

	DBG("imcontext = %p", imcontext);

	if (focused_imcontext && focused_imcontext != imcontext)
		meego_imcontext_focus_out(GTK_IM_CONTEXT(focused_imcontext));
	focused_imcontext = imcontext;

	imcontext->focus_state = TRUE;
	meego_imcontext_update_widget_info(imcontext);

	ret = meego_im_proxy_activate_context(imcontext->proxy);
	if (ret) {
		meego_im_proxy_update_widget_info(imcontext->proxy,
					imcontext->widget_state, focus_changed);
		meego_im_proxy_show_input_method(imcontext->proxy);
	}
	// TODO: anything else than call "activateContext" and "showInputMethod" ?

}


static void
meego_imcontext_focus_out (GtkIMContext *context)
{
	MeegoIMContext *imcontext = MEEGO_IMCONTEXT(context);
	DBG("imcontext = %p", imcontext);

	meego_im_proxy_hide_input_method(imcontext->proxy);

	// TODO: anything else than call "hideInputMethod" ?

	imcontext->focus_state = FALSE;
	focused_imcontext = NULL;
	focused_widget = NULL;
}


static gboolean
meego_imcontext_filter_key_event (GtkIMContext *context, GdkEventKey *event)
{
	MeegoIMContext *imcontext = MEEGO_IMCONTEXT(context);
	int qevent_type = 0, qt_keycode = 0, qt_modifier = 0;
	gchar *text = "";

	focused_widget = gtk_get_event_widget((GdkEvent*)event);

	DBG("event type=0x%x, state=0x%x, keyval=0x%x, keycode=0x%x, group=%d",
		event->type, event->state, event->keyval, event->hardware_keycode, event->group);

	if (focused_imcontext != imcontext)
		meego_imcontext_focus_in(context);

	if ((event->state & IM_FORWARD_MASK) || !redirect_keys) {
		GtkIMContext *slave = meego_imcontext_get_slave_imcontext();
		return gtk_im_context_filter_keypress(slave, event);
	}

	if (!gdk_key_event_to_qt(event, &qevent_type, &qt_keycode, &qt_modifier))
		return FALSE;

	meego_im_proxy_process_key_event(imcontext->proxy, qevent_type, qt_keycode, qt_modifier,
					 text, 0, 1, event->hardware_keycode, event->state, event->time);

	return TRUE;
}


static void
meego_imcontext_reset (GtkIMContext *context)
{
	MeegoIMContext *imcontext = MEEGO_IMCONTEXT(context);
	DBG("imcontext = %p", imcontext);

	meego_im_proxy_reset(imcontext->proxy);
}


static void
meego_imcontext_get_preedit_string (GtkIMContext *context, gchar **str, PangoAttrList **attrs, gint *cursor_pos)
{
	MeegoIMContext *imcontext = MEEGO_IMCONTEXT(context);

	DBG("imcontext = %p", imcontext);

	if (str) {
		if (imcontext->preedit_str)
			*str = g_strdup(imcontext->preedit_str);
		else
			*str = g_strdup ("");
	}

	if (attrs) {
		if (imcontext->preedit_attrs) {
			*attrs = imcontext->preedit_attrs;
			pango_attr_list_ref(imcontext->preedit_attrs);
		} else {
			*attrs = pango_attr_list_new();
		}
	}

	if (cursor_pos)
		*cursor_pos = imcontext->preedit_cursor_pos;
}


static void
meego_imcontext_set_preedit_enabled (GtkIMContext *context, gboolean enabled)
{
	// TODO: Seems QT/MEEGO don't need it, it will always showing preedit.
	return;
}


static void
meego_imcontext_set_client_window (GtkIMContext *context, GdkWindow *window)
{
	MeegoIMContext *imcontext = MEEGO_IMCONTEXT(context);
	STEP();

	if (imcontext->client_window)
		g_object_unref(imcontext->client_window);

	if (window)
		g_object_ref(window);

	imcontext->client_window = window;

	// TODO: might need to update cursor position or other staff later using this info?
}


static void
meego_imcontext_set_cursor_location (GtkIMContext *context, GdkRectangle *area)
{
	MeegoIMContext *imcontext = MEEGO_IMCONTEXT(context);
	//DBG("imcontext = %p, x=%d, y=%d, w=%d, h=%d", imcontext,
	//	area->x, area->y, area->width, area->height);

	imcontext->cursor_location = *area;

	// TODO: call updateWidgetInformation?
	//The cursor location from GTK widget is simillar to ImMicroFocus info of a QWidget
	//Thus we might need to update Qt::ImMicroFocus info according to this. 
	//But MEEGO IM seems not using this info at all
}

/* Update the widget_state map with current information about the widget. */
void
meego_imcontext_update_widget_info(MeegoIMContext *imcontext)
{

    /* Window ID */
    if (imcontext->client_window) {
	guint64 xid = GDK_WINDOW_XID(imcontext->client_window);
	GValue *xid_value = g_new0(GValue, 1);
	g_value_init(xid_value, G_TYPE_UINT64);
	g_value_set_uint64(xid_value, xid);
	g_hash_table_insert(imcontext->widget_state, g_strdup(WIDGET_INFO_WIN_ID), xid_value);
    } else {
	g_hash_table_remove(imcontext->widget_state, WIDGET_INFO_WIN_ID);
    }

    /* Focus state */
    GValue *focus_value = g_new0(GValue, 1);
    g_value_init(focus_value, G_TYPE_BOOLEAN);
    g_value_set_boolean(focus_value, imcontext->focus_state);
    g_hash_table_insert(imcontext->widget_state, g_strdup(WIDGET_INFO_FOCUS_STATE), focus_value);
}

// Call back functions for dbus obj

gboolean
meego_imcontext_client_activation_lost_event (MeegoIMContextDbusObj *obj)
{
	STEP();
	return TRUE;
}


gboolean
meego_imcontext_client_im_initiated_hide (MeegoIMContextDbusObj *obj)
{
	STEP();
	return TRUE;
}


gboolean
meego_imcontext_client_commit_string (MeegoIMContextDbusObj *obj, char *string)
{
	DBG("string is:%s", string);
	if (focused_imcontext)
		g_signal_emit_by_name(focused_imcontext, "commit", string);

	return TRUE;
}


gboolean
meego_imcontext_client_update_preedit (MeegoIMContextDbusObj *obj, char *string, int preedit_face)
{
	STEP();
	return TRUE;
}


gboolean
meego_imcontext_client_key_event (MeegoIMContextDbusObj *obj, int type, int key, int modifiers, char *text,
				gboolean auto_repeat, int count)
{
	GdkEventKey *event = NULL;
	GdkWindow *window = NULL;

	STEP();
	if (focused_imcontext)
		window = focused_imcontext->client_window;

	event = qt_key_event_to_gdk(type, key, modifiers, text, window);
	if (!event)
		return TRUE;
	event->send_event = TRUE;
	event->state |= IM_FORWARD_MASK;

	gdk_event_put((GdkEvent *)event);
	gdk_event_free((GdkEvent *)event);
	return TRUE;
}


gboolean
meego_imcontext_client_update_input_method_area (MeegoIMContextDbusObj *obj, GPtrArray *data)
{
	STEP();
	return TRUE;
}


gboolean
meego_imcontext_client_set_global_correction_enabled (MeegoIMContextDbusObj *obj, gboolean correction)
{
	STEP();
	return TRUE;
}


gboolean
meego_imcontext_client_copy (MeegoIMContextDbusObj *obj)
{
	GdkWindow *window = NULL;
	GdkEventKey *event = NULL;

	STEP();

	if (focused_imcontext)
		window = focused_imcontext->client_window;

	event = compose_gdk_keyevent(GDK_KEY_PRESS, XK_C, GDK_CONTROL_MASK, window);
	if (event) {
		event->send_event = TRUE;
		event->state |= IM_FORWARD_MASK;
		gdk_event_put((GdkEvent *)event);
		gdk_event_free((GdkEvent *)event);
	}

	event = compose_gdk_keyevent(GDK_KEY_RELEASE, XK_C, GDK_CONTROL_MASK, window);
	if (event) {
		event->send_event = TRUE;
		event->state |= IM_FORWARD_MASK;
		gdk_event_put((GdkEvent *)event);
		gdk_event_free((GdkEvent *)event);
	}

	return TRUE;
}


gboolean
meego_imcontext_client_paste (MeegoIMContextDbusObj *obj)
{
	GdkWindow *window = NULL;
	GdkEventKey *event = NULL;

	STEP();

	if (focused_imcontext)
		window = focused_imcontext->client_window;

	event = compose_gdk_keyevent(GDK_KEY_PRESS, XK_V, GDK_CONTROL_MASK, window);
	if (event) {
		event->send_event = TRUE;
		event->state |= IM_FORWARD_MASK;
		gdk_event_put((GdkEvent *)event);
		gdk_event_free((GdkEvent *)event);
	}

	event = compose_gdk_keyevent(GDK_KEY_RELEASE, XK_V, GDK_CONTROL_MASK, window);
	if (event) {
		event->send_event = TRUE;
		event->state |= IM_FORWARD_MASK;
		gdk_event_put((GdkEvent *)event);
		gdk_event_free((GdkEvent *)event);
	}


	return TRUE;
}


gboolean
meego_imcontext_client_set_redirect_keys (MeegoIMContextDbusObj *obj, gboolean enabled)
{
	DBG("enabled = %d", enabled);
	redirect_keys = enabled;
	return TRUE;
}


gboolean
meego_imcontext_client_preedit_rectangle(MeegoIMContextDbusObj *obj, GValueArray** rect, gboolean *valid)
{
	STEP();
	return TRUE;
}

