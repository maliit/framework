/*
 * Copyright (C) 2010 Intel Corporation
 *
 * Author:  Raymond Liu <raymond.li@intel.com>
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
 *
 */

#include <stdlib.h>
#include <dbus/dbus-glib.h>

#include "dui-imcontext-dbus.h"
#include "debug.h"

#define DUI_IMCONTEXT_DBUSOBJ_SERVICE_NAME "org.meego.gtk.duiimcontext"
#define DUI_IMCONTEXT_DBUSOBJ_SERVICE_OBJECT_PATH "/org/meego/gtk/duiimcontext"


G_DEFINE_TYPE( DuiIMContextDbusObj, dui_imcontext_dbusobj, G_TYPE_OBJECT);

gboolean dui_imcontext_dbus_activation_lost_event (DuiIMContextDbusObj *obj, GError **error);
gboolean dui_imcontext_dbus_im_initiated_hide (DuiIMContextDbusObj *obj, GError **error);
gboolean dui_imcontext_dbus_commit_string (DuiIMContextDbusObj *obj, char *string, GError **error);
gboolean dui_imcontext_dbus_update_preedit (DuiIMContextDbusObj *obj, char *string, int preedit_face, GError **error);
gboolean dui_imcontext_dbus_key_event (DuiIMContextDbusObj *obj, int type, int key, int modifiers, char *text,
				gboolean auto_repeat, int count, GError **error);
gboolean dui_imcontext_dbus_update_input_method_area (DuiIMContextDbusObj *obj, GPtrArray *data, GError **error);
gboolean dui_imcontext_dbus_set_global_correction_enabled (DuiIMContextDbusObj *obj, gboolean correction, GError **error);
gboolean dui_imcontext_dbus_copy (DuiIMContextDbusObj *obj, GError **error);
gboolean dui_imcontext_dbus_paste (DuiIMContextDbusObj *obj, GError **error);
gboolean dui_imcontext_dbus_set_redirect_keys (DuiIMContextDbusObj *obj, gboolean enabled, GError **error);
gboolean dui_imcontext_dbus_preedit_rectangle(DuiIMContextDbusObj *obj, GValueArray** rect, gboolean *valid, GError **error);


#include "dui-imcontext-dbus-glue.h"

static void
dui_imcontext_dbusobj_init(DuiIMContextDbusObj* obj)
{
	STEP();
	g_assert(obj != NULL);
}


static void
dui_imcontext_dbusobj_class_init(DuiIMContextDbusObjClass* klass) 
{
	g_assert(klass != NULL);

	dbus_g_object_type_install_info(DUI_IMCONTEXT_TYPE_DBUSOBJ,
					&dbus_glib_dui_imcontext_dbus_object_info);
}


DuiIMContextDbusObj *
dui_imcontext_dbus_register(void)
{
	DBusGConnection* bus = NULL;
	DBusGProxy* busProxy = NULL;
	DuiIMContextDbusObj* dbusobj = NULL;
	guint result;
	GError* error = NULL;

	bus = dbus_g_bus_get(DBUS_BUS_SESSION, &error);
	if (error != NULL) {
		g_warning("Couldn't connect to session bus\n");
		return NULL;
	}

	DBG("Registering the well-known name (%s)\n", DUI_IMCONTEXT_DBUSOBJ_SERVICE_NAME);

	busProxy = dbus_g_proxy_new_for_name(bus,
					DBUS_SERVICE_DBUS,
					DBUS_PATH_DBUS,
					DBUS_INTERFACE_DBUS);

	if (busProxy == NULL) {
		DBG("Failed to get a proxy for D-Bus\n");
		return NULL;
	}

	/* Attempt to register the well-known name.*/
	if (!dbus_g_proxy_call(busProxy,
				"RequestName",
				&error,
				G_TYPE_STRING,
				DUI_IMCONTEXT_DBUSOBJ_SERVICE_NAME,
				G_TYPE_UINT,
				0,
				G_TYPE_INVALID,
				G_TYPE_UINT,
				&result,
				G_TYPE_INVALID)) {
		g_warning("D-Bus.RequestName RPC failed\n");
	}

	if (result != 1) {
		g_warning("Failed to get the primary well-known name.\n");
		return NULL;
	}

	dbusobj = g_object_new(DUI_IMCONTEXT_TYPE_DBUSOBJ, NULL);

	if (dbusobj == NULL) {
		g_warning("Failed to create dbus_obj.\n");
		return NULL;
	}

	dbus_g_connection_register_g_object(bus, DUI_IMCONTEXT_DBUSOBJ_SERVICE_OBJECT_PATH,
						G_OBJECT(dbusobj));

	return dbusobj;
}


DuiIMContextDbusObj *
dui_imcontext_dbusobj_get_singleton (void)
{
	static DuiIMContextDbusObj *dbusobj = NULL;

	if (!dbusobj)
		dbusobj = dui_imcontext_dbus_register();
	return dbusobj;
}


gboolean
dui_imcontext_dbus_activation_lost_event (DuiIMContextDbusObj *obj, GError **error)
{
	STEP();
	return TRUE;
}


gboolean
dui_imcontext_dbus_im_initiated_hide (DuiIMContextDbusObj *obj, GError **error)
{
	STEP();
	return TRUE;
}


gboolean
dui_imcontext_dbus_commit_string (DuiIMContextDbusObj *obj, char *string, GError **error)
{
	DBG("string is:%s", string);
	return dui_imcontext_client_commit_string(obj, string);
}


gboolean
dui_imcontext_dbus_update_preedit (DuiIMContextDbusObj *obj, char *string, int preedit_face, GError **error)
{
	STEP();
	return TRUE;
}


gboolean
dui_imcontext_dbus_key_event (DuiIMContextDbusObj *obj, int type, int key, int modifiers, char *text,
				gboolean auto_repeat, int count, GError **error)
{
	STEP();
	return TRUE;
}


gboolean
dui_imcontext_dbus_update_input_method_area (DuiIMContextDbusObj *obj, GPtrArray *data, GError **error)
{
	STEP();
	return TRUE;
}


gboolean
dui_imcontext_dbus_set_global_correction_enabled (DuiIMContextDbusObj *obj, gboolean correction, GError **error)
{
	STEP();
	return TRUE;
}


gboolean
dui_imcontext_dbus_copy (DuiIMContextDbusObj *obj, GError **error)
{
	STEP();
	return TRUE;
}


gboolean
dui_imcontext_dbus_paste (DuiIMContextDbusObj *obj, GError **error)
{
	STEP();
	return TRUE;
}


gboolean
dui_imcontext_dbus_set_redirect_keys (DuiIMContextDbusObj *obj, gboolean enabled, GError **error)
{
	STEP();
	return TRUE;
}


gboolean
dui_imcontext_dbus_preedit_rectangle(DuiIMContextDbusObj *obj, GValueArray** rect, gboolean *valid, GError **error)
{
	STEP();
	return TRUE;
}


gchar *
dui_imcontext_dbusobj_get_path(DuiIMContextDbusObj *obj)
{
	return DUI_IMCONTEXT_DBUSOBJ_SERVICE_OBJECT_PATH;
}


