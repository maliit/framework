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
#include <glib.h>

#include "meego-imcontext-dbus.h"
#include "debug.h"

#define MEEGO_IMCONTEXT_DBUSOBJ_SERVICE_OBJECT_PATH "/com/meego/inputmethod/inputcontext"

G_DEFINE_TYPE(MeegoIMContextDbusObj, meego_imcontext_dbusobj, G_TYPE_OBJECT);

gboolean meego_imcontext_dbus_activation_lost_event(MeegoIMContextDbusObj *obj, GError **error);
gboolean meego_imcontext_dbus_im_initiated_hide(MeegoIMContextDbusObj *obj, GError **error);
gboolean meego_imcontext_dbus_commit_string(MeegoIMContextDbusObj *obj, char *string, gint replacement_start,
        int replacement_length, int cursor_pos, GError **error);
gboolean meego_imcontext_dbus_update_preedit(MeegoIMContextDbusObj *obj, char *string, int preedit_face, GError **error);
gboolean meego_imcontext_dbus_key_event(MeegoIMContextDbusObj *obj, int type, int key, int modifiers, char *text,
                                        gboolean auto_repeat, int count, GError **error);
gboolean meego_imcontext_dbus_update_input_method_area(MeegoIMContextDbusObj *obj, GPtrArray *data, GError **error);
gboolean meego_imcontext_dbus_set_global_correction_enabled(MeegoIMContextDbusObj *obj, gboolean correction, GError **error);
gboolean meego_imcontext_dbus_copy(MeegoIMContextDbusObj *obj, GError **error);
gboolean meego_imcontext_dbus_paste(MeegoIMContextDbusObj *obj, GError **error);
gboolean meego_imcontext_dbus_set_redirect_keys(MeegoIMContextDbusObj *obj, gboolean enabled, GError **error);
gboolean meego_imcontext_dbus_preedit_rectangle(MeegoIMContextDbusObj *obj, GValueArray **rect, gboolean *valid, GError **error);


#include "meego-imcontext-dbus-glue.h"


static void
meego_imcontext_dbusobj_init(MeegoIMContextDbusObj *obj)
{
    STEP();
    g_assert(obj != NULL);
}


static void
meego_imcontext_dbusobj_class_init(MeegoIMContextDbusObjClass *klass)
{
    g_assert(klass != NULL);

    dbus_g_object_type_install_info(MEEGO_IMCONTEXT_TYPE_DBUSOBJ,
                                    &dbus_glib_meego_imcontext_dbus_object_info);
}



void
meego_imcontext_dbusobj_connect(MeegoIMContextDbusObj *obj, DBusGConnection *connection)
{
    dbus_g_connection_register_g_object(connection,
                                        MEEGO_IMCONTEXT_DBUSOBJ_SERVICE_OBJECT_PATH,
                                        G_OBJECT(obj));
}

MeegoIMContextDbusObj *
meego_imcontext_dbusobj_get_singleton(void)
{
    static MeegoIMContextDbusObj *dbusobj = NULL;

    if (!dbusobj)
        dbusobj = g_object_new(MEEGO_IMCONTEXT_TYPE_DBUSOBJ, NULL);
    return dbusobj;
}


gboolean
meego_imcontext_dbus_activation_lost_event(MeegoIMContextDbusObj *obj, GError **error)
{
    STEP();
    return TRUE;
}


gboolean
meego_imcontext_dbus_im_initiated_hide(MeegoIMContextDbusObj *obj, GError **error)
{
    STEP();
    return meego_imcontext_client_im_initiated_hide (obj);
}


gboolean
meego_imcontext_dbus_commit_string(MeegoIMContextDbusObj *obj, char *string, gint replacement_start,
                                   int replacement_length, int cursor_pos, GError **error)
{
    DBG("string is:%s", string);
    return meego_imcontext_client_commit_string(obj, string);
}


gboolean
meego_imcontext_dbus_update_preedit(MeegoIMContextDbusObj *obj, char *string, int preedit_face, GError **error)
{
    STEP();
    return TRUE;
}


gboolean
meego_imcontext_dbus_key_event(MeegoIMContextDbusObj *obj, int type, int key, int modifiers, char *text,
                               gboolean auto_repeat, int count, GError **error)
{
    DBG("type=0x%x, key=0x%x, modifiers=0x%x, text = %s, auto_repeat=%d, count=%d",
        type, key, modifiers, text, auto_repeat, count);
    return meego_imcontext_client_key_event(obj, type, key, modifiers, text, auto_repeat, count);
}


gboolean
meego_imcontext_dbus_update_input_method_area(MeegoIMContextDbusObj *obj, GPtrArray *data, GError **error)
{
    STEP();
    return TRUE;
}


gboolean
meego_imcontext_dbus_set_global_correction_enabled(MeegoIMContextDbusObj *obj, gboolean correction, GError **error)
{
    STEP();
    return TRUE;
}


gboolean
meego_imcontext_dbus_copy(MeegoIMContextDbusObj *obj, GError **error)
{
    STEP();
    return meego_imcontext_client_copy(obj);
    return TRUE;
}


gboolean
meego_imcontext_dbus_paste(MeegoIMContextDbusObj *obj, GError **error)
{
    STEP();
    return meego_imcontext_client_paste(obj);
    return TRUE;
}


gboolean
meego_imcontext_dbus_set_redirect_keys(MeegoIMContextDbusObj *obj, gboolean enabled, GError **error)
{
    STEP();
    meego_imcontext_client_set_redirect_keys(obj, enabled);
    return TRUE;
}


gboolean
meego_imcontext_dbus_preedit_rectangle(MeegoIMContextDbusObj *obj, GValueArray **rect, gboolean *valid, GError **error)
{
    STEP();
    return TRUE;
}

gboolean
meego_imcontext_dbus_set_detectable_auto_repeat(MeegoIMContextDbusObj *obj, gboolean enabled, GError **error)
{
    STEP();
    return TRUE;
}

gboolean
meego_imcontext_dbus_set_selection(MeegoIMContextDbusObj *obj, int start, int length, GError **error)
{
    STEP();
    return TRUE;
}

gboolean
meego_imcontext_dbus_selection(MeegoIMContextDbusObj *obj, gboolean *valid, gchar **gdata, GError **error)
{
    STEP();
    return TRUE;
}

