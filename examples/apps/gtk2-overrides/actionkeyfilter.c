/* This file is part of Maliit framework
 *
 * Copyright (C) 2012 Openismus GmbH
 *
 * Contact: project@maliit.org
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

    guint login_realize_signal_id;
    guint login_key_release_event_signal_id;

    guint password_realize_signal_id;
    guint password_key_release_event_signal_id;
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

static gboolean
key_release_event (ActionKeyFilter *filter,
                   GdkEvent *event,
                   GtkWidget *widget)
{
    GtkEntry *entry;

    if (filter == NULL || event == NULL || !GTK_IS_ENTRY (widget)) {
        return FALSE;
    }

    if (event->type != GDK_KEY_RELEASE) {
        return FALSE;
    }

    entry = GTK_ENTRY (widget);

    if (entry != filter->login && entry != filter->password) {
        return FALSE;
    }

    if (event->key.keyval != GDK_KEY_Return) {
        return FALSE;
    }

    if (entry == filter->login && action_key_filter_get_enter_login_accepts (filter)) {
        gtk_widget_grab_focus (GTK_WIDGET (filter->password));
        return TRUE;
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
        return TRUE;
    }
    return FALSE;
}

static void
connect_filter_signals (ActionKeyFilter *filter,
                        GtkWidget *widget)
{
    GdkWindow *window;
    GdkEventMask mask;
    guint* signal_id;
    GtkEntry *entry;

    g_return_if_fail (filter != NULL);
    g_return_if_fail (GTK_IS_WIDGET (widget));
    g_return_if_fail (GTK_IS_ENTRY (widget));

    entry = GTK_ENTRY (widget);

    g_return_if_fail (entry == filter->login || entry == filter->password);

    window = gtk_widget_get_window (widget);

    g_return_if_fail (GDK_IS_WINDOW (window));

    mask = gdk_window_get_events (window);
    mask |= GDK_KEY_RELEASE_MASK;
    gdk_window_set_events (window, mask);

    if (entry == filter->login) {
        signal_id = &filter->login_key_release_event_signal_id;
    } else {
        signal_id = &filter->password_key_release_event_signal_id;
    }

    *signal_id = g_signal_connect_swapped (widget,
                                           "key-release-event",
                                           G_CALLBACK (key_release_event),
                                           filter);
}

ActionKeyFilter *
action_key_filter_new (GtkEntry *login, GtkEntry *password)
{
    ActionKeyFilter *filter;
    GtkWidget *login_widget;
    GtkWidget *password_widget;

    g_return_val_if_fail (GTK_IS_ENTRY (login), NULL);
    g_return_val_if_fail (GTK_IS_ENTRY (password), NULL);

    login_widget = GTK_WIDGET (login);
    password_widget = GTK_WIDGET (password);

    filter = g_slice_new0 (ActionKeyFilter);
    filter->enter_login_accepts = TRUE;
    filter->enter_password_accepts = TRUE;
    filter->login = login;
    filter->password = password;

    if (gtk_widget_get_realized (login_widget)) {
        connect_filter_signals (filter, login_widget);
    } else {
        filter->login_realize_signal_id = g_signal_connect_swapped (login_widget,
                                                                    "realize",
                                                                    G_CALLBACK (connect_filter_signals),
                                                                    filter);
    }

    if (gtk_widget_get_realized (password_widget)) {
        connect_filter_signals (filter, password_widget);
    } else {
        filter->password_realize_signal_id = g_signal_connect_swapped (password_widget,
                                                                       "realize",
                                                                       G_CALLBACK (connect_filter_signals),
                                                                       filter);
    }

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
        if (filter->login_realize_signal_id) {
            g_signal_handler_disconnect (filter->login, filter->login_realize_signal_id);
            filter->login_realize_signal_id = 0;
        }
        if (filter->login_key_release_event_signal_id) {
            g_signal_handler_disconnect (filter->login, filter->login_key_release_event_signal_id);
            filter->login_key_release_event_signal_id = 0;
        }
        g_object_remove_weak_pointer (G_OBJECT (filter->login), (gpointer *)&filter->login);
        filter->login = NULL;
    }

    if (filter->password) {
        if (filter->password_realize_signal_id) {
            g_signal_handler_disconnect (filter->password, filter->password_realize_signal_id);
            filter->password_realize_signal_id = 0;
        }
        if (filter->password_key_release_event_signal_id) {
            g_signal_handler_disconnect (filter->password, filter->password_key_release_event_signal_id);
            filter->password_key_release_event_signal_id = 0;
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
