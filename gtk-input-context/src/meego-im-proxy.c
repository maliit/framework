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

#include "meego-im-proxy.h"
#include "meego-im-proxy-glue.h"
#include "debug.h"

G_DEFINE_TYPE (MeegoIMProxy, meego_im_proxy, G_TYPE_OBJECT);

#define MEEGO_IM_OBJECT_PATH "/com/meego/inputmethod/uiserver1"
#define MEEGO_IM_SERVICE_INTERFACE "com.meego.inputmethod.uiserver1"

static void meego_im_proxy_finalize(GObject *object);
static void meego_im_proxy_class_init (MeegoIMProxyClass *klass);
static void meego_im_proxy_init (MeegoIMProxy *meego_im_proxy);

enum {
	SIGNAL_CONNECTION_DROPPED = 0,
	N_SIGNALS
};

static guint meego_im_proxy_signals[N_SIGNALS];

static void
handle_disconnect(gpointer instance, MeegoIMProxy *im_proxy)
{
        g_return_if_fail(im_proxy);

        im_proxy->dbusproxy = NULL;
        g_signal_emit(im_proxy, meego_im_proxy_signals[SIGNAL_CONNECTION_DROPPED], 0, NULL);

}

MeegoIMProxy *
meego_im_proxy_get_singleton (void)
{
	static MeegoIMProxy *proxy = NULL;
	if (!proxy)
		proxy = g_object_new(MEEGO_TYPE_IM_PROXY, NULL);
	return proxy;
}


static void
meego_im_proxy_finalize (GObject *object)
{
	//MeegoIMProxy *im_proxy = MEEGO_IM_PROXY(object);

	G_OBJECT_CLASS(meego_im_proxy_parent_class)->finalize(object);
}


static void
meego_im_proxy_class_init (MeegoIMProxyClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

	gobject_class->finalize = meego_im_proxy_finalize;

	meego_im_proxy_signals[SIGNAL_CONNECTION_DROPPED] =
		g_signal_new("connection-dropped", G_TYPE_FROM_CLASS(klass),
                        0, 0, NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);
}


static void
meego_im_proxy_init (MeegoIMProxy *self)
{
	UNUSED(self);
}

void
meego_im_proxy_connect (MeegoIMProxy *proxy, DBusGConnection *connection)
{
	DBusGProxy* dbusproxy;
	g_return_if_fail(connection != NULL);

	if (proxy->dbusproxy) {
		g_object_unref(proxy->dbusproxy);
	}

	dbusproxy = dbus_g_proxy_new_for_peer (connection,
					       MEEGO_IM_OBJECT_PATH, /* obj path */
					       MEEGO_IM_SERVICE_INTERFACE /* interface */);

	if (dbusproxy == NULL) {
		g_warning("could not create dbus_proxy\n");
	}

	g_signal_connect(G_OBJECT(dbusproxy), "destroy", G_CALLBACK(handle_disconnect), proxy);

	proxy->dbusproxy = dbusproxy;
}

gboolean
meego_im_proxy_activate_context (MeegoIMProxy *proxy)
{
	GError *error = NULL;
	gboolean ret = TRUE;

	STEP();
	if (!proxy || proxy->dbusproxy == NULL)
		return FALSE;

	ret = com_meego_inputmethod_uiserver1_activate_context(proxy->dbusproxy, &error);

	if (error != NULL) {
		g_warning("%s", error->message);
	}

	return ret;
}


gboolean
meego_im_proxy_app_orientation_changed (MeegoIMProxy *proxy, const gint angle)
{
	GError *error = NULL;
	gboolean ret = TRUE;

	STEP();
	if (!proxy || proxy->dbusproxy == NULL)
		return FALSE;

	ret = com_meego_inputmethod_uiserver1_app_orientation_changed(proxy->dbusproxy, angle, &error);

	if (error != NULL) {
		g_warning("%s", error->message);
	}

	return ret;
}


gboolean
meego_im_proxy_hide_input_method (MeegoIMProxy *proxy)
{
	GError *error = NULL;
	gboolean ret = TRUE;

	STEP();
	if (!proxy || proxy->dbusproxy == NULL)
		return FALSE;

	ret = com_meego_inputmethod_uiserver1_hide_input_method(proxy->dbusproxy, &error);

	if (error != NULL) {
		g_warning("%s", error->message);
	}

	return ret;
}


#if 0
// Not yet really implemented

gboolean
meego_im_proxy_mouse_clicked_on_preedit (MeegoIMProxy *proxy, const GValueArray* pos,
					const GValueArray* preedit_rect)
{
	GError *error = NULL;
	gboolean ret = TRUE;

	if (!proxy || proxy->dbusproxy == NULL)
		return FALSE;

	ret = com_meego_inputmethod_uiserver1_mouse_clicked_on_preedit(proxy->dbusproxy,
							pos, preedit_rect, &error);

	if (error != NULL) {
		g_warning("%s", error->message);
	}

	return ret;
}
#endif

gboolean
meego_im_proxy_update_widget_info (MeegoIMProxy *proxy, const GHashTable *state_information, gboolean focus_changed)
{
	GError *error = NULL;
	gboolean ret = TRUE;

	STEP();
	if (!proxy || proxy->dbusproxy == NULL)
		return FALSE;

	ret = com_meego_inputmethod_uiserver1_update_widget_information(proxy->dbusproxy,
						state_information, focus_changed, &error);

	if (error != NULL) {
		g_warning("%s", error->message);
	}

	return ret;
}




gboolean
meego_im_proxy_process_key_event (MeegoIMProxy *proxy, const gint type, const gint code,
					const gint modifiers, const char * text,
					const gboolean auto_repeat, const gint count,
					const guint native_scan_code,
					const guint native_modifiers,
					const guint time)
{
	GError *error = NULL;
	gboolean ret = TRUE;

	DBG("QT key event type=0x%x, code=0x%x, modifiers=0x%x, text=%s",
		type, code, modifiers, text);
	if (!proxy || proxy->dbusproxy == NULL)
		return FALSE;

	ret = com_meego_inputmethod_uiserver1_process_key_event(proxy->dbusproxy,
								type, code, modifiers, text, auto_repeat,
								count, native_scan_code, native_modifiers,
								time, &error);

	if (error != NULL) {
		g_warning("%s", error->message);
	}

	return ret;
}


gboolean
meego_im_proxy_reset (MeegoIMProxy *proxy)
{
	GError *error = NULL;
	gboolean ret = TRUE;

	STEP();
	if (!proxy || proxy->dbusproxy == NULL)
		return FALSE;

	ret = com_meego_inputmethod_uiserver1_reset(proxy->dbusproxy, &error);

	if (error != NULL) {
		g_warning("%s", error->message);
	}

	return ret;
}


gboolean
meego_im_proxy_set_copy_paste_state (MeegoIMProxy *proxy, const gboolean copy_available,
					const gboolean paste_available)
{
	GError *error = NULL;
	gboolean ret = TRUE;

	STEP();
	if (!proxy || proxy->dbusproxy == NULL)
		return FALSE;

	ret = com_meego_inputmethod_uiserver1_set_copy_paste_state(proxy->dbusproxy,
						copy_available, paste_available, &error);

	if (error != NULL) {
		g_warning("%s", error->message);
	}

	return ret;
}


gboolean
meego_im_proxy_set_preedit (MeegoIMProxy *proxy, const char *text, gint cursor_pos)
{
	GError *error = NULL;
	gboolean ret = TRUE;

	STEP();
	if (!proxy || proxy->dbusproxy == NULL)
		return FALSE;

	ret = com_meego_inputmethod_uiserver1_set_preedit(proxy->dbusproxy, text, cursor_pos, &error);

	if (error != NULL) {
		g_warning("%s", error->message);
	}

	return ret;
}


gboolean
meego_im_proxy_show_input_method (MeegoIMProxy *proxy)
{
	GError *error = NULL;
	gboolean ret = TRUE;

	STEP();
	if (!proxy || proxy->dbusproxy == NULL)
		return FALSE;

	ret = com_meego_inputmethod_uiserver1_show_input_method(proxy->dbusproxy, &error);

	if (error != NULL) {
		g_warning("%s", error->message);
	}

	return ret;
}
