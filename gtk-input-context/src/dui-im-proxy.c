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

#include "dui-im-proxy.h"
#include "dui-im-proxy-glue.h"

G_DEFINE_TYPE (DuiIMProxy, dui_im_proxy, G_TYPE_OBJECT);

#define DUI_IM_SERVICE_NAME "org.maemo.duiinputmethodserver1"
#define DUI_IM_OBJECT_PATH "/org/maemo/duiinputmethodserver1"
#define DUI_IM_SERVICE_INTERFACE "org.maemo.duiinputmethodserver1"

static void dui_im_proxy_finalize(GObject *object);
static void dui_im_proxy_class_init (DuiIMProxyClass *klass);
static void dui_im_proxy_init (DuiIMProxy *dui_im_proxy);


DuiIMProxy *
dui_im_proxy_get_singleton (void)
{
	static DuiIMProxy *proxy = NULL;
	if (!proxy)
		proxy = g_object_new(DUI_TYPE_IM_PROXY, NULL);
	return proxy;
}


static void
dui_im_proxy_finalize (GObject *object)
{
	//DuiIMProxy *im_proxy = DUI_IM_PROXY(object);

	G_OBJECT_CLASS(dui_im_proxy_parent_class)->finalize(object);
}


static void
dui_im_proxy_class_init (DuiIMProxyClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

	gobject_class->finalize = dui_im_proxy_finalize;
}


static void
dui_im_proxy_init (DuiIMProxy *self)
{
	DBusGConnection* bus;
	DBusGProxy* dbusproxy;
	GError* error = NULL;

	bus = dbus_g_bus_get(DBUS_BUS_SESSION, &error);

	if (error != NULL) {
		g_warning("%s", error->message);
	}

	dbusproxy = dbus_g_proxy_new_for_name(bus,
			DUI_IM_SERVICE_NAME, /* name */
			DUI_IM_OBJECT_PATH, /* obj path */
			DUI_IM_SERVICE_INTERFACE /* interface */);

	if (dbusproxy == NULL) {
		g_warning("could not create dbus_proxy for %s \n", DUI_IM_SERVICE_NAME);
	}

	self->dbusproxy = dbusproxy;
}


gboolean
dui_im_proxy_activate_context (DuiIMProxy *proxy)
{
	GError *error = NULL;
	gboolean ret = TRUE;

	if (!proxy || proxy->dbusproxy == NULL)
		return FALSE;

	ret = org_maemo_duiinputmethodserver1_activate_context(proxy->dbusproxy, &error);

	if (error != NULL) {
		g_warning("%s", error->message);
	}

	return ret;
}


gboolean
dui_im_proxy_app_orientation_changed (DuiIMProxy *proxy, const gint angle)
{
	GError *error = NULL;
	gboolean ret = TRUE;

	if (!proxy || proxy->dbusproxy == NULL)
		return FALSE;

	ret = org_maemo_duiinputmethodserver1_app_orientation_changed(proxy->dbusproxy, angle, &error);

	if (error != NULL) {
		g_warning("%s", error->message);
	}

	return ret;
}


gboolean
dui_im_proxy_hide_input_method (DuiIMProxy *proxy)
{
	GError *error = NULL;
	gboolean ret = TRUE;

	if (!proxy || proxy->dbusproxy == NULL)
		return FALSE;

	ret = org_maemo_duiinputmethodserver1_hide_input_method(proxy->dbusproxy, &error);

	if (error != NULL) {
		g_warning("%s", error->message);
	}

	return ret;
}


#if 0
// Not yet really implemented

gboolean
dui_im_proxy_mouse_clicked_on_preedit (DuiIMProxy *proxy, const GValueArray* pos,
					const GValueArray* preedit_rect)
{
	GError *error = NULL;
	gboolean ret = TRUE;

	if (!proxy || proxy->dbusproxy == NULL)
		return FALSE;

	ret = org_maemo_duiinputmethodserver1_mouse_clicked_on_preedit(proxy->dbusproxy,
							pos, preedit_rect, &error);

	if (error != NULL) {
		g_warning("%s", error->message);
	}

	return ret;
}


gboolean
dui_im_proxy_update_widget_info (DuiIMProxy *proxy, const GHashTable *state_information)
{
	GError *error = NULL;
	gboolean ret = TRUE;

	if (!proxy || proxy->dbusproxy == NULL)
		return FALSE;

	ret = org_maemo_duiinputmethodserver1_update_widget_information(proxy->dbusproxy,
						state_information, &error);

	if (error != NULL) {
		g_warning("%s", error->message);
	}

	return ret;
}

#endif


gboolean
dui_im_proxy_process_key_event (DuiIMProxy *proxy, const gint type, const gint code,
					const gint modifiers, const char * text,
					const gboolean auto_repeat, const gint count,
					const gint native_scan_code)
{
	GError *error = NULL;
	gboolean ret = TRUE;

	if (!proxy || proxy->dbusproxy == NULL)
		return FALSE;

	ret = org_maemo_duiinputmethodserver1_process_key_event(proxy->dbusproxy,
					type, code, modifiers, text, auto_repeat,
					count, native_scan_code, &error);

	if (error != NULL) {
		g_warning("%s", error->message);
	}

	return ret;
}


gboolean
dui_im_proxy_reset (DuiIMProxy *proxy)
{
	GError *error = NULL;
	gboolean ret = TRUE;

	if (!proxy || proxy->dbusproxy == NULL)
		return FALSE;

	ret = org_maemo_duiinputmethodserver1_reset(proxy->dbusproxy, &error);

	if (error != NULL) {
		g_warning("%s", error->message);
	}

	return ret;
}


gboolean
dui_im_proxy_set_context_object (DuiIMProxy *proxy, const char *object_name)
{
	GError *error = NULL;
	gboolean ret = TRUE;

	if (!proxy || proxy->dbusproxy == NULL)
		return FALSE;

	ret = org_maemo_duiinputmethodserver1_set_context_object(proxy->dbusproxy, object_name, &error);

	if (error != NULL) {
		g_warning("%s", error->message);
	}

	return ret;
}


gboolean
dui_im_proxy_set_copy_paste_state (DuiIMProxy *proxy, const gboolean copy_available,
					const gboolean paste_available)
{
	GError *error = NULL;
	gboolean ret = TRUE;

	if (!proxy || proxy->dbusproxy == NULL)
		return FALSE;

	ret = org_maemo_duiinputmethodserver1_set_copy_paste_state(proxy->dbusproxy,
						copy_available, paste_available, &error);

	if (error != NULL) {
		g_warning("%s", error->message);
	}

	return ret;
}


gboolean
dui_im_proxy_set_preeidt (DuiIMProxy *proxy, const char *text)
{
	GError *error = NULL;
	gboolean ret = TRUE;

	if (!proxy || proxy->dbusproxy == NULL)
		return FALSE;

	ret = org_maemo_duiinputmethodserver1_set_preedit(proxy->dbusproxy, text, &error);

	if (error != NULL) {
		g_warning("%s", error->message);
	}

	return ret;
}


gboolean
dui_im_proxy_show_input_method (DuiIMProxy *proxy)
{
	GError *error = NULL;
	gboolean ret = TRUE;

	if (!proxy || proxy->dbusproxy == NULL)
		return FALSE;

	ret = org_maemo_duiinputmethodserver1_show_input_method(proxy->dbusproxy, &error);

	if (error != NULL) {
		g_warning("%s", error->message);
	}

	return ret;
}

