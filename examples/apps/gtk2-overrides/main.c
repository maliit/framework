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

#include <gtk/gtk.h>
#include <maliit-glib/maliitattributeextension.h>

#include "actionkeyfilter.h"

static void
put_widgets_into_container (GtkContainer* window, ...) G_GNUC_NULL_TERMINATED;

static void
put_widgets_into_container (GtkContainer* window, ...)
{
    va_list var_args;

    va_start (var_args, window);
    {
#if GTK_MAJOR_VERSION >= 3
        GtkGrid *grid = GTK_GRID (gtk_grid_new ());
        GtkWidget *previous_widget = NULL;

        for (;;) {
            GtkWidget *widget = va_arg (var_args, GtkWidget *);

            if (widget && GTK_IS_WIDGET (widget)) {
                gtk_grid_attach_next_to (grid, widget, previous_widget, GTK_POS_BOTTOM, 1, 1);
                previous_widget = widget;
            } else {
                break;
            }
        }

        gtk_container_add (window, GTK_WIDGET (grid));
#else
        GtkBox *box = GTK_BOX (gtk_vbox_new (FALSE, 10));

        for (;;) {
            GtkWidget *widget = va_arg (var_args, GtkWidget *);

            if (widget && GTK_IS_WIDGET (widget)) {
                gtk_box_pack_start (box, widget, FALSE, FALSE, 0);
            } else {
                break;
            }
        }

        gtk_container_add (window, GTK_WIDGET (box));
#endif
    }
    va_end (var_args);
}

static void
label_override (GtkToggleButton *button,
                gpointer user_data,
                const gchar* label)
{
    MaliitAttributeExtension *extension = MALIIT_ATTRIBUTE_EXTENSION (user_data);
    const gchar* new_label;
    GVariant *value;

    if (gtk_toggle_button_get_active (button)) {
        new_label = label;
    } else {
        new_label = "";
    }

    value = g_variant_new_string (new_label);
    if (g_variant_is_floating (value)) {
        g_variant_ref_sink (value);
    }
    maliit_attribute_extension_set_attribute (extension, "/keys/actionKey/label", value);
    g_variant_unref (value);
}

static void
login_override (GtkToggleButton* button,
                gpointer user_data)
{
    label_override (button, user_data, "Next");
}

static void
login_change_enter_accepts (GtkToggleButton* button,
                            gpointer user_data)
{
    ActionKeyFilter *filter = (ActionKeyFilter*)user_data;

    action_key_filter_set_enter_login_accepts (filter,
                                               gtk_toggle_button_get_active (button));
}

static void
password_override (GtkToggleButton* button,
                   gpointer user_data)
{
    label_override (button, user_data, "Login");
}

static void
password_change_enter_accepts (GtkToggleButton* button,
                               gpointer user_data)
{
    ActionKeyFilter *filter = (ActionKeyFilter*)user_data;

    action_key_filter_set_enter_login_accepts (filter,
                                               gtk_toggle_button_get_active (button));
}

static gboolean
grab_focus (GtkWidget *widget G_GNUC_UNUSED,
            GdkEvent *event G_GNUC_UNUSED,
            gpointer user_data)
{
    GtkWidget *target_widget = GTK_WIDGET (user_data);

    gtk_widget_grab_focus (target_widget);
    return TRUE;
}

static void
check_button_realized (GtkWidget *widget,
                       gpointer user_data)
{
    GdkWindow *window = gtk_widget_get_window (widget);

    if (window) {
        GdkEventMask mask = gdk_window_get_events (window);

        mask |= GDK_FOCUS_CHANGE_MASK;
        gdk_window_set_events (window, mask);

        g_signal_connect (widget, "focus-in-event",
                          G_CALLBACK (grab_focus), user_data);
    }
}

gint
main(gint    argc,
     gchar **argv)
{
    MaliitAttributeExtension *login_extension;
    MaliitAttributeExtension *password_extension;
    GtkWidget *window;
    GtkWidget *hide_keyboard_button;
    GtkWidget *login_label;
    GtkWidget *login;
    GtkEntry *login_entry;
    GtkWidget *login_check_button;
    GtkWidget *password_label;
    GtkWidget *password;
    GtkEntry *password_entry;
    GtkWidget *password_check_button;
    GtkWidget *comment_label;
    GtkWidget *comment;
    GtkWidget *close;
    ActionKeyFilter *filter;

    gtk_init (&argc, &argv);

    login_extension = maliit_attribute_extension_new ();
    password_extension = maliit_attribute_extension_new ();

    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title (GTK_WINDOW (window), "Maliit GTK+ key override test application");

    /* Clicking the button will steal focus from the text entry, thus
     * hiding the virtual keyboard. */
    hide_keyboard_button = gtk_button_new_with_label("Hide keyboard");
    login_label = gtk_label_new ("Login:");
    login = gtk_entry_new();
    login_entry = GTK_ENTRY (login);
    login_check_button = gtk_check_button_new_with_label ("Enter accepts login");
    password_label = gtk_label_new ("Password:");
    password = gtk_entry_new();
    password_entry = GTK_ENTRY (password);
    password_check_button = gtk_check_button_new_with_label ("Enter accepts password");
    comment_label = gtk_label_new ("Comment (not required):");
    comment = gtk_entry_new();
    close = gtk_button_new_with_label ("Close application");
    filter = action_key_filter_new (login_entry, password_entry);

    maliit_attribute_extension_attach_to_object (login_extension, G_OBJECT (login));
    g_signal_connect (login_check_button, "realize",
                      G_CALLBACK (check_button_realized), login);
    g_signal_connect (login_check_button, "toggled",
                      G_CALLBACK (login_override), login_extension);
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (login_check_button),
                                  action_key_filter_get_enter_login_accepts (filter));
    g_signal_connect (login_check_button, "toggled",
                      G_CALLBACK (login_change_enter_accepts), filter);

    gtk_entry_set_visibility (GTK_ENTRY (password), FALSE);
    maliit_attribute_extension_attach_to_object (password_extension, G_OBJECT (password));
    g_signal_connect (password_check_button, "realize",
                      G_CALLBACK (check_button_realized), password);
    g_signal_connect (password_check_button, "toggled",
                      G_CALLBACK (password_override), password_extension);
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (password_check_button),
                                  action_key_filter_get_enter_password_accepts (filter));
    g_signal_connect (password_check_button, "toggled",
                      G_CALLBACK (password_change_enter_accepts), filter);

    put_widgets_into_container (GTK_CONTAINER (window),
                                hide_keyboard_button,
                                login_label,
                                login,
                                login_check_button,
                                password_label,
                                password,
                                password_check_button,
                                comment_label,
                                comment,
                                close,
                                NULL);

    g_signal_connect(window, "delete-event",
                     G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect (close, "clicked",
                      G_CALLBACK (gtk_main_quit), NULL);
    gtk_widget_show_all(window);

    gtk_main();

    action_key_filter_free (filter);
    return 0;
}
