/* This file is part of Maliit framework
 *
 * Copyright (C) 2012 Openismus GmbH
 *
 * Contact: maliit-discuss@lists.maliit.org
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the licence, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef MEEGOIMCONTEXTDBUSPRIVATE_H
#define MEEGOIMCONTEXTDBUSPRIVATE_H

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
gboolean
meego_imcontext_dbus_plugin_settings_loaded (MeegoIMContextDbusObj *obj,
                                             GPtrArray *settings_data,
                                             GError **error G_GNUC_UNUSED);

#endif // MEEGOIMCONTEXTDBUSPRIVATE_H
