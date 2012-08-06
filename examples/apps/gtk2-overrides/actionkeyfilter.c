/* This file is part of Maliit framework
 *
 * Copyright (C) 2012 One Laptop per Child Association
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

#include <gdk/gdkkeysyms.h>

#include "actionkeyfilter.h"

struct _ActionKeyFilter
{
    GtkEntry* login;
    GtkEntry* password;

    gboolean enter_login_accepts;
    gboolean enter_password_accepts;

    guint login_activated_signal_id;
    guint password_activated_signal_id;
};

static GtkWindow *
get_parent_gtk_window (GtkWidget *widget)
{
    if (widget == NULL) {
        return NULL;
    } else if (GTK_IS_WINDOW (widget)) {
        return GTK_WINDOW (widget);
    } else {
        return get_parent_gtk_window (gtk_widget_get_parent (widget));
    }
}

static void
on_entry_activated (ActionKeyFilter *filter,
                    GtkEntry *entry)
{
    GtkWidget* widget;

    if (filter == NULL || !GTK_IS_ENTRY (entry)) {
        return;
    }

    widget = GTK_WIDGET (entry);

    if (entry != filter->login && entry != filter->password) {
        return;
    }

    if (entry == filter->login && action_key_filter_get_enter_login_accepts (filter)) {
        GtkWidget* target_widget = GTK_WIDGET (filter->password);

        gtk_widget_grab_focus (target_widget);
        return;
    } else if (entry == filter->password && action_key_filter_get_enter_password_accepts (filter)) {
        const gchar *login_text = gtk_entry_get_text (filter->login);
        const gchar *password_text = gtk_entry_get_text (filter->password);
        const gchar *message = NULL;
        GtkWidget *dialog;
        GtkMessageType message_type;

        if (login_text == NULL || *login_text == '\0' ||
            password_text == NULL || *password_text == '\0') {
            message = "Please enter your credentials.";
            message_type = GTK_MESSAGE_WARNING;
        } else {
            message = "Login successful!";
            message_type = GTK_MESSAGE_INFO;
        }

        dialog = gtk_message_dialog_new (get_parent_gtk_window (widget),
                                         GTK_DIALOG_DESTROY_WITH_PARENT,
                                         message_type,
                                         GTK_BUTTONS_OK,
                                         "%s",
                                         message);
        gtk_dialog_run (GTK_DIALOG (dialog));
        gtk_widget_destroy (dialog);
        return;
    }
    return;
}

ActionKeyFilter *
action_key_filter_new (GtkEntry *login, GtkEntry *password)
{
    ActionKeyFilter *filter;

    g_return_val_if_fail (GTK_IS_ENTRY (login), NULL);
    g_return_val_if_fail (GTK_IS_ENTRY (password), NULL);

    filter = g_slice_new0 (ActionKeyFilter);
    filter->enter_login_accepts = TRUE;
    filter->enter_password_accepts = TRUE;
    filter->login = login;
    filter->password = password;

    filter->login_activated_signal_id = g_signal_connect_swapped (filter->login,
                                                                  "activate",
                                                                  G_CALLBACK (on_entry_activated),
                                                                  filter);

    filter->password_activated_signal_id = g_signal_connect_swapped (filter->password,
                                                                     "activate",
                                                                     G_CALLBACK (on_entry_activated),
                                                                     filter);

    g_object_add_weak_pointer (G_OBJECT (login), (gpointer *)&filter->login);
    g_object_add_weak_pointer (G_OBJECT (password), (gpointer *)&filter->password);

    return filter;
}

void
action_key_filter_free (ActionKeyFilter *filter)
{
    if (!filter) {
        return;
    }

    if (filter->login) {
        if (filter->login_activated_signal_id) {
            g_signal_handler_disconnect (filter->login, filter->login_activated_signal_id);
            filter->login_activated_signal_id = 0;
        }
        g_object_remove_weak_pointer (G_OBJECT (filter->login), (gpointer *)&filter->login);
        filter->login = NULL;
    }

    if (filter->password) {
        if (filter->password_activated_signal_id) {
            g_signal_handler_disconnect (filter->password, filter->password_activated_signal_id);
            filter->password_activated_signal_id = 0;
        }
        g_object_remove_weak_pointer (G_OBJECT (filter->password), (gpointer *)&filter->password);
        filter->password = NULL;
    }
    g_slice_free (ActionKeyFilter, filter);
}

gboolean
action_key_filter_get_enter_login_accepts (ActionKeyFilter *filter)
{
    g_return_val_if_fail (filter != NULL, FALSE);

    return filter->enter_login_accepts;
}

void
action_key_filter_set_enter_login_accepts (ActionKeyFilter *filter,
                                           gboolean enter_login_accepts)
{
    g_return_if_fail (filter != NULL);

    filter->enter_login_accepts = enter_login_accepts;
}

gboolean
action_key_filter_get_enter_password_accepts (ActionKeyFilter *filter)
{
    g_return_val_if_fail (filter != NULL, FALSE);

    return filter->enter_password_accepts;
}

void
action_key_filter_set_enter_password_accepts (ActionKeyFilter *filter,
                                              gboolean enter_password_accepts)
{
    g_return_if_fail (filter != NULL);

    filter->enter_password_accepts = enter_password_accepts;
}
