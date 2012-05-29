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
#include "maliitmarshalers.h"

#include <dbus/dbus-glib.h>

#define MEEGO_IMCONTEXT_DBUSOBJ_SERVICE_OBJECT_PATH "/com/meego/inputmethod/inputcontext"

G_DEFINE_TYPE(MeegoIMContextDbusObj, meego_imcontext_dbusobj, G_TYPE_OBJECT);

gboolean meego_imcontext_dbus_activation_lost_event(MeegoIMContextDbusObj *obj, GError **error);
gboolean meego_imcontext_dbus_im_initiated_hide(MeegoIMContextDbusObj *obj, GError **error);
gboolean meego_imcontext_dbus_commit_string(MeegoIMContextDbusObj *obj, char *string, gint replacement_start,
        int replacement_length, int cursor_pos, GError **error);
gboolean meego_imcontext_dbus_update_preedit(MeegoIMContextDbusObj *obj, const char *string, GPtrArray *formatListData, gint32 replaceStart, gint32 replaceLength, gint32 cursorPos, GError** error);
gboolean meego_imcontext_dbus_key_event(MeegoIMContextDbusObj *obj, int type, int key, int modifiers, char *text,
                                        gboolean auto_repeat, int count, GError **error);
gboolean meego_imcontext_dbus_update_input_method_area(MeegoIMContextDbusObj *obj, int x, int y, int width, int height, GError **error);
gboolean meego_imcontext_dbus_set_global_correction_enabled(MeegoIMContextDbusObj *obj, gboolean correction, GError **error);
gboolean meego_imcontext_dbus_copy(MeegoIMContextDbusObj *obj, GError **error);
gboolean meego_imcontext_dbus_paste(MeegoIMContextDbusObj *obj, GError **error);
gboolean meego_imcontext_dbus_set_redirect_keys(MeegoIMContextDbusObj *obj, gboolean enabled, GError **error);
gboolean meego_imcontext_dbus_preedit_rectangle(MeegoIMContextDbusObj *obj, GValueArray **rect, gboolean *valid, GError **error);
gboolean meego_imcontext_dbus_set_language (MeegoIMContextDbusObj *obj,
                                            const gchar *language_id,
                                            GError **error);
gboolean meego_imcontext_dbus_set_detectable_auto_repeat(MeegoIMContextDbusObj *obj, gboolean enabled, GError **error);
gboolean meego_imcontext_dbus_set_selection(MeegoIMContextDbusObj *obj, int start, int length, GError **error);
gboolean meego_imcontext_dbus_selection(MeegoIMContextDbusObj *obj, gboolean *valid, gchar **gdata, GError **error);

gboolean meego_imcontext_dbus_notify_extended_attribute_changed (MeegoIMContextDbusObj *obj,
                                                                 gint id,
                                                                 const gchar *target,
                                                                 const gchar *target_item,
                                                                 const gchar *key,
                                                                 GValue *value,
                                                                 GError **error);

gboolean meego_imcontext_dbus_plugin_settings_loaded (MeegoIMContextDbusObj *obj, GPtrArray *settings_data, GError **error);

enum {
    ACTIVATION_LOST_EVENT,
    IM_INITIATED_HIDE,
    COMMIT_STRING,
    UPDATE_PREEDIT,
    KEY_EVENT,
    UPDATE_INPUT_METHOD_AREA,
    SET_GLOBAL_CORRECTION,
    PREEDIT_RECTANGLE,
    COPY,
    PASTE,
    SET_REDIRECT_KEYS,
    SET_DETECTABLE_AUTO_REPEAT,
    SET_SELECTION,
    SELECTION,
    SET_LANGUAGE,
    NOTIFY_EXTENDED_ATTRIBUTE_CHANGED,
    LAST_SIGNAL
};

static guint imcontext_signals[LAST_SIGNAL] = { 0 };

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

    imcontext_signals[ACTIVATION_LOST_EVENT] = g_signal_new("activation-lost-event",
                                                            G_TYPE_FROM_CLASS(klass),
                                                            G_SIGNAL_RUN_FIRST,
                                                            0,
                                                            NULL, NULL,
                                                            g_cclosure_marshal_VOID__VOID,
                                                            G_TYPE_NONE, 0);
    imcontext_signals[IM_INITIATED_HIDE] = g_signal_new("im-initiated-hide",
                                                        G_TYPE_FROM_CLASS(klass),
                                                        G_SIGNAL_RUN_FIRST,
                                                        0,
                                                        NULL, NULL,
                                                        g_cclosure_marshal_VOID__VOID,
                                                        G_TYPE_NONE, 0);
    imcontext_signals[COMMIT_STRING] = g_signal_new("commit-string",
                                                    G_TYPE_FROM_CLASS(klass),
                                                    G_SIGNAL_RUN_FIRST,
                                                    0,
                                                    NULL, NULL,
                                                    _maliit_marshal_VOID__STRING_INT_INT_INT,
                                                    G_TYPE_NONE, 4,
                                                    G_TYPE_STRING,
                                                    G_TYPE_INT,
                                                    G_TYPE_INT,
                                                    G_TYPE_INT);
    imcontext_signals[UPDATE_PREEDIT] = g_signal_new("update-preedit",
                                                     G_TYPE_FROM_CLASS(klass),
                                                     G_SIGNAL_RUN_FIRST,
                                                     0,
                                                     NULL, NULL,
                                                     _maliit_marshal_VOID__STRING_BOXED_INT_INT_INT,
                                                     G_TYPE_NONE, 5,
                                                     G_TYPE_STRING,
                                                     G_TYPE_PTR_ARRAY,
                                                     G_TYPE_INT,
                                                     G_TYPE_INT,
                                                     G_TYPE_INT);
    imcontext_signals[KEY_EVENT] = g_signal_new("key-event",
                                                G_TYPE_FROM_CLASS(klass),
                                                G_SIGNAL_RUN_FIRST,
                                                0,
                                                NULL, NULL,
                                                _maliit_marshal_VOID__INT_INT_INT_STRING_BOOLEAN_INT,
                                                G_TYPE_NONE, 6,
                                                G_TYPE_INT,
                                                G_TYPE_INT,
                                                G_TYPE_INT,
                                                G_TYPE_STRING,
                                                G_TYPE_BOOLEAN,
                                                G_TYPE_INT);
    imcontext_signals[UPDATE_INPUT_METHOD_AREA] = g_signal_new("update-input-method-area",
                                                               G_TYPE_FROM_CLASS(klass),
                                                               G_SIGNAL_RUN_FIRST,
                                                               0,
                                                               NULL, NULL,
                                                               _maliit_marshal_VOID__INT_INT_INT_INT,
                                                               G_TYPE_NONE, 4,
                                                               G_TYPE_INT,
                                                               G_TYPE_INT,
                                                               G_TYPE_INT,
                                                               G_TYPE_INT);
    imcontext_signals[SET_GLOBAL_CORRECTION] = g_signal_new("set-global-correction",
                                                            G_TYPE_FROM_CLASS(klass),
                                                            G_SIGNAL_RUN_FIRST,
                                                            0,
                                                            NULL, NULL,
                                                            g_cclosure_marshal_VOID__BOOLEAN,
                                                            G_TYPE_NONE, 1,
                                                            G_TYPE_BOOLEAN);
    imcontext_signals[PREEDIT_RECTANGLE] = g_signal_new("preedit-rectangle",
                                                        G_TYPE_FROM_CLASS(klass),
                                                        G_SIGNAL_RUN_LAST,
                                                        0,
                                                        NULL, NULL,
                                                        _maliit_marshal_BOXED__VOID,
                                                        G_TYPE_PTR_ARRAY, 0);
    imcontext_signals[COPY] = g_signal_new("copy",
                                           G_TYPE_FROM_CLASS(klass),
                                           G_SIGNAL_RUN_FIRST,
                                           0,
                                           NULL, NULL,
                                           g_cclosure_marshal_VOID__VOID,
                                           G_TYPE_NONE, 0);
    imcontext_signals[PASTE] = g_signal_new("paste",
                                            G_TYPE_FROM_CLASS(klass),
                                            G_SIGNAL_RUN_FIRST,
                                            0,
                                            NULL, NULL,
                                            g_cclosure_marshal_VOID__VOID,
                                            G_TYPE_NONE, 0);
    imcontext_signals[SET_REDIRECT_KEYS] = g_signal_new("set-redirect-keys",
                                                        G_TYPE_FROM_CLASS(klass),
                                                        G_SIGNAL_RUN_FIRST,
                                                        0,
                                                        NULL, NULL,
                                                        g_cclosure_marshal_VOID__BOOLEAN,
                                                        G_TYPE_NONE, 1,
                                                        G_TYPE_BOOLEAN);
    imcontext_signals[SET_DETECTABLE_AUTO_REPEAT] = g_signal_new("set-detectable-auto-repeat",
                                                                 G_TYPE_FROM_CLASS(klass),
                                                                 G_SIGNAL_RUN_FIRST,
                                                                 0,
                                                                 NULL, NULL,
                                                                 g_cclosure_marshal_VOID__BOOLEAN,
                                                                 G_TYPE_NONE, 1,
                                                                 G_TYPE_BOOLEAN);
    imcontext_signals[SET_SELECTION] = g_signal_new("set-selection",
                                                    G_TYPE_FROM_CLASS(klass),
                                                    G_SIGNAL_RUN_FIRST,
                                                    0,
                                                    NULL, NULL,
                                                    _maliit_marshal_VOID__INT_INT,
                                                    G_TYPE_NONE, 2,
                                                    G_TYPE_INT,
                                                    G_TYPE_INT);
    imcontext_signals[SELECTION] = g_signal_new("selection",
                                                G_TYPE_FROM_CLASS(klass),
                                                G_SIGNAL_RUN_LAST,
                                                0,
                                                NULL, NULL,
                                                _maliit_marshal_STRING__VOID,
                                                G_TYPE_STRING, 0);
    imcontext_signals[SET_LANGUAGE] = g_signal_new("set-language",
                                                   G_TYPE_FROM_CLASS(klass),
                                                   G_SIGNAL_RUN_FIRST,
                                                   0,
                                                   NULL, NULL,
                                                   g_cclosure_marshal_VOID__STRING,
                                                   G_TYPE_NONE, 1,
                                                   G_TYPE_STRING);
    imcontext_signals[NOTIFY_EXTENDED_ATTRIBUTE_CHANGED] = g_signal_new("notify-extended-attribute-changed",
                                                                        G_TYPE_FROM_CLASS(klass),
                                                                        G_SIGNAL_RUN_FIRST,
                                                                        0,
                                                                        NULL, NULL,
                                                                        _maliit_marshal_VOID__INT_STRING_STRING_STRING_VARIANT,
                                                                        G_TYPE_NONE, 5,
                                                                        G_TYPE_INT,
                                                                        G_TYPE_STRING,
                                                                        G_TYPE_STRING,
                                                                        G_TYPE_STRING,
                                                                        G_TYPE_VARIANT);
}

void
meego_imcontext_dbusobj_connect(MeegoIMContextDbusObj *obj, gpointer connection)
{
    DBusGConnection *dbus_connection = (DBusGConnection *)connection;
    g_return_if_fail(dbus_connection != NULL);

    dbus_g_connection_register_g_object(dbus_connection,
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

    g_signal_emit(obj, imcontext_signals[ACTIVATION_LOST_EVENT], 0);

    return TRUE;
}


gboolean
meego_imcontext_dbus_im_initiated_hide(MeegoIMContextDbusObj *obj, GError **error)
{
    STEP();

    g_signal_emit(obj, imcontext_signals[IM_INITIATED_HIDE], 0);

    return TRUE;
}


gboolean
meego_imcontext_dbus_commit_string(MeegoIMContextDbusObj *obj, char *string, gint replacement_start,
                                   int replacement_length, int cursor_pos, GError **error)
{
    UNUSED(error);

    DBG("commit string: %s", string);

    g_signal_emit(obj, imcontext_signals[COMMIT_STRING], 0, string, replacement_start, replacement_length, cursor_pos);

    return TRUE;
}


gboolean
meego_imcontext_dbus_update_preedit(MeegoIMContextDbusObj *obj, const char *string, GPtrArray *formatListData, gint32 replaceStart, gint32 replaceLength, gint32 cursorPos, GError **error)
{
    UNUSED(error);

    DBG("preedit string: %s", string);

    g_signal_emit(obj, imcontext_signals[UPDATE_PREEDIT], 0, string, formatListData, replaceStart, replaceLength, cursorPos);

    return TRUE;
}


gboolean
meego_imcontext_dbus_key_event(MeegoIMContextDbusObj *obj, int type, int key, int modifiers, char *text,
                               gboolean auto_repeat, int count, GError **error)
{
    UNUSED(error);

    DBG("type=0x%x, key=0x%x, modifiers=0x%x, text = %s, auto_repeat=%d, count=%d",
        type, key, modifiers, text, auto_repeat, count);

    g_signal_emit(obj, imcontext_signals[KEY_EVENT], 0, type, key, modifiers, text, auto_repeat, count);

    return TRUE;
}

gboolean
meego_imcontext_dbus_update_input_method_area(MeegoIMContextDbusObj *obj, int x, int y, int width, int height, GError **error G_GNUC_UNUSED)
{
    STEP();

    g_signal_emit(obj, imcontext_signals[UPDATE_INPUT_METHOD_AREA], 0, x, y, width, height);

    return TRUE;
}


gboolean
meego_imcontext_dbus_set_global_correction_enabled(MeegoIMContextDbusObj *obj, gboolean correction, GError **error)
{
    STEP();

    // TODO emit SET_GLOBAL_CORRECTION signal

    return TRUE;
}


gboolean
meego_imcontext_dbus_copy(MeegoIMContextDbusObj *obj, GError **error)
{
    UNUSED(error);

    STEP();

    g_signal_emit(obj, imcontext_signals[COPY], 0);

    return TRUE;
}


gboolean
meego_imcontext_dbus_paste(MeegoIMContextDbusObj *obj, GError **error)
{
    UNUSED(error);

    STEP();

    g_signal_emit(obj, imcontext_signals[PASTE], 0);

    return TRUE;
}


gboolean
meego_imcontext_dbus_set_redirect_keys(MeegoIMContextDbusObj *obj, gboolean enabled, GError **error)
{
    UNUSED(error);

    STEP();

    g_signal_emit(obj, imcontext_signals[SET_REDIRECT_KEYS], 0, enabled);

    return TRUE;
}


gboolean
meego_imcontext_dbus_preedit_rectangle(MeegoIMContextDbusObj *obj, GValueArray **rect, gboolean *valid, GError **error)
{
    STEP();

    // TODO emit PREEDIT_RECTANGLE signal

    return TRUE;
}

gboolean
meego_imcontext_dbus_set_detectable_auto_repeat(MeegoIMContextDbusObj *obj, gboolean enabled, GError **error)
{
    STEP();

    // TODO emit SET_DETECTABLE_AUTO_REPEAT signal

    return TRUE;
}

gboolean
meego_imcontext_dbus_set_selection(MeegoIMContextDbusObj *obj, int start, int length, GError **error)
{
    STEP();

    // TODO emit SET_SELECTION signal

    return TRUE;
}

gboolean
meego_imcontext_dbus_selection(MeegoIMContextDbusObj *obj, gboolean *valid, gchar **gdata, GError **error)
{
    STEP();

    // TODO emit SELECTION signal

    return TRUE;
}

gboolean
meego_imcontext_dbus_set_language (MeegoIMContextDbusObj *obj,
                                   const gchar *language_id,
                                   GError **error)
{
    UNUSED(error);
    STEP();

    g_signal_emit(obj, imcontext_signals[SET_LANGUAGE], 0, language_id);

    return TRUE;
}

gboolean
meego_imcontext_dbus_notify_extended_attribute_changed (MeegoIMContextDbusObj *obj,
                                                        gint id,
                                                        const gchar *target,
                                                        const gchar *target_item,
                                                        const gchar *key,
                                                        GValue *value,
                                                        GError **error)
{
    UNUSED(error);
    STEP();

    GVariant *variant_value = dbus_g_value_build_g_variant (value);

    if (variant_value) {
        if (g_variant_is_floating (variant_value)) {
            g_variant_ref_sink (variant_value);
        }

        g_signal_emit(obj, imcontext_signals[NOTIFY_EXTENDED_ATTRIBUTE_CHANGED], 0, id, target, target_item, key, variant_value);

        g_variant_unref (variant_value);

        return TRUE;
    }

    g_warning ("Unknown data type: %s", G_VALUE_TYPE_NAME (value));

    return TRUE;
}

gboolean
meego_imcontext_dbus_plugin_settings_loaded (MeegoIMContextDbusObj *obj, GPtrArray *settings_data, GError **error)
{
	UNUSED(obj);
	UNUSED(settings_data);
	UNUSED(error);
	return TRUE;
}
