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

#ifndef _MEEGO_IM_PROXY_H
#define _MEEGO_IM_PROXY_H

#include <glib.h>
#include <glib-object.h>
#include <dbus/dbus-glib.h>

G_BEGIN_DECLS

typedef struct _MeegoIMProxy MeegoIMProxy;
typedef struct _MeegoIMProxyClass MeegoIMProxyClass;

#define MEEGO_TYPE_IM_PROXY		(meego_im_proxy_get_type())
#define MEEGO_IM_PROXY(obj)		(G_TYPE_CHECK_INSTANCE_CAST((obj), MEEGO_TYPE_IM_PROXY, MeegoIMProxy))
#define MEEGO_IM_PROXY_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST((klass), MEEGO_TYPE_IM_PROXY, MeegoIMProxyClass))
#define MEEGO_IS_IM_PROXY(obj)		(G_TYPE_CHECK_INSTANCE_TYPE((obj), MEEGO_TYPE_IM_PROXY))
#define MEEGO_IS_IM_PROXY_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE((klass), MEEGO_TYPE_IM_PROXY))
#define MEEGO_IM_PROXY_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS((obj), MEEGO_TYPE_IM_PROXY, MeegoIMProxyClass))


struct _MeegoIMProxy {
	GObject parent;

	DBusGProxy *dbusproxy;
};

struct _MeegoIMProxyClass {
	GObjectClass parent;
};

GType meego_im_proxy_get_type (void);

MeegoIMProxy *meego_im_proxy_get_singleton (void);
void meego_im_proxy_connect (MeegoIMProxy *proxy, DBusGConnection *connection);
gboolean meego_im_proxy_activate_context (MeegoIMProxy *proxy);
gboolean meego_im_proxy_app_orientation_changed (MeegoIMProxy *proxy, const gint angle);
gboolean meego_im_proxy_hide_input_method (MeegoIMProxy *proxy);
#if 0
// Not yet really implemented
gboolean meego_im_proxy_mouse_clicked_on_preedit (MeegoIMProxy *proxy, const GValueArray* pos,
					const GValueArray* preedit_rect);
#endif

gboolean meego_im_proxy_update_widget_info (MeegoIMProxy *proxy, const GHashTable *state_information, gboolean focus_changed);

gboolean meego_im_proxy_process_key_event (MeegoIMProxy *proxy, const gint type, const gint code,
					const gint modifiers, const char * text,
					const gboolean auto_repeat, const gint count,
					const guint native_scan_code,
					const guint native_modifiers,
					const guint time);

gboolean meego_im_proxy_reset (MeegoIMProxy *proxy);
gboolean meego_im_proxy_set_context_object (MeegoIMProxy *proxy, const char *object_name);
gboolean meego_im_proxy_set_copy_paste_state (MeegoIMProxy *proxy, const gboolean copy_available,
					const gboolean paste_available);
gboolean meego_im_proxy_set_preedit (MeegoIMProxy *proxy, const char *text, const gint cursor_pos);
gboolean meego_im_proxy_show_input_method (MeegoIMProxy *proxy);

G_END_DECLS

#endif //_MEEGO_IM_PROXY_H
