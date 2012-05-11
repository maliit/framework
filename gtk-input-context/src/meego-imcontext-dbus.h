/*
 * Copyright (C) 2010 Intel Corporation
 *
 * Author:  Raymond Liu <raymond.liu@intel.com>
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

#ifndef _MEEGO_IMCONTEXT_DBUS_H
#define _MEEGO_IMCONTEXT_DBUS_H

#include <glib.h>
#include <glib-object.h>

typedef struct {
    GObject parent;

} MeegoIMContextDbusObj;

typedef struct {
    GObjectClass parent;
} MeegoIMContextDbusObjClass;

GType meego_imcontext_dbusobj_get_type(void);

#define MEEGO_IMCONTEXT_TYPE_DBUSOBJ (meego_imcontext_dbusobj_get_type())
#define MEEGO_IMCONTEXT_DBUSOBJ(obj) \
        (G_TYPE_CHECK_INSTANCE_CAST((obj), MEEGO_IMCONTEXT_TYPE_DBUSOBJ, MeegoIMContextDbusObj))
#define MEEGO_IMCONTEXT_DBUSOBJ_CLASS(klass) \
        (G_TYPE_CHECK_CLASS_CAST((klass), MEEGO_IMCONTEXT_TYPE_DBUSOBJ, MeegoIMContextDbusObjClass))
#define MEEGO_IMCONTEXT_IS_DBUSOBJ(obj) \
        (G_TYPE_CHECK_INSTANCE_TYPE((obj), MEEGO_IMCONTEXT_TYPE_DBUSOBJ))
#define MEEGO_IMCONTEXT_IS_DBUSOBJ_CLASS(klass) \
        (G_TYPE_CHECK_CLASS_TYPE((klass), MEEGO_IMCONTEXT_TYPE_DBUSOBJ))
#define MEEGO_IMCONTEXT_DBUSOBJ_GET_CLASS(obj) \
        (G_TYPE_INSTANCE_GET_CLASS((obj), MEEGO_IMCONTEXT_TYPE_DBUSOBJ, MeegoIMContextDbusObjClass))


MeegoIMContextDbusObj *meego_imcontext_dbusobj_get_singleton(void);
void meego_imcontext_dbusobj_connect(MeegoIMContextDbusObj *obj, gpointer connection);

// Call back functions for dbus obj, need to be implemented in imcontext client side

gboolean meego_imcontext_client_activation_lost_event(MeegoIMContextDbusObj *obj);
gboolean meego_imcontext_client_im_initiated_hide(MeegoIMContextDbusObj *obj);
gboolean meego_imcontext_client_commit_string(MeegoIMContextDbusObj *obj, char *string);
gboolean meego_imcontext_client_update_preedit(MeegoIMContextDbusObj *obj, const char *string, GPtrArray *formatListData, gint32 replaceStart, gint32 replaceLength, gint32 cursorPos, GError **error);
gboolean meego_imcontext_client_key_event(MeegoIMContextDbusObj *obj, int type, int key, int modifiers, char *text,
        gboolean auto_repeat, int count);
gboolean meego_imcontext_client_update_input_method_area(MeegoIMContextDbusObj *obj, GPtrArray *data);
gboolean meego_imcontext_client_set_global_correction_enabled(MeegoIMContextDbusObj *obj, gboolean correction);
gboolean meego_imcontext_client_copy(MeegoIMContextDbusObj *obj);
gboolean meego_imcontext_client_paste(MeegoIMContextDbusObj *obj);
gboolean meego_imcontext_client_set_redirect_keys(MeegoIMContextDbusObj *obj, gboolean enabled);
gboolean meego_imcontext_client_preedit_rectangle(MeegoIMContextDbusObj *obj, GValueArray **rect, gboolean *valid);
gboolean meego_imcontext_dbus_set_detectable_auto_repeat(MeegoIMContextDbusObj *obj, gboolean enabled, GError **error);
gboolean meego_imcontext_dbus_set_selection(MeegoIMContextDbusObj *obj, int start, int length, GError **error);
gboolean meego_imcontext_dbus_selection(MeegoIMContextDbusObj *obj, gboolean *valid, gchar **gdata, GError **error);

gboolean
meego_imcontext_client_set_language (MeegoIMContextDbusObj *obj,
                                     const gchar *language_id,
                                     GError **error);

gboolean
meego_imcontext_client_notify_extended_attribute_changed (MeegoIMContextDbusObj *obj,
                                                          gint id,
                                                          const gchar *target,
                                                          const gchar *targetItem,
                                                          const gchar *key,
                                                          GValue *value,
                                                          GError **error);

#endif /* _MEEGO_IMCONTEXT_DBUS_H */
