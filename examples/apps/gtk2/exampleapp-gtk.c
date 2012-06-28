/* * This file is part of Maliit framework *
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 *
 * Contact: maliit-discuss@lists.maliit.org
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 */

#include <gtk/gtk.h>

#include <maliit-glib/maliitinputmethod.h>

static void
show_button_clicked(GtkButton *button G_GNUC_UNUSED,
                    gpointer user_data)
{
    MaliitInputMethod *input_method = MALIIT_INPUT_METHOD(user_data);

    maliit_input_method_show(input_method);
}

static void
hide_button_clicked(GtkButton *button G_GNUC_UNUSED,
                    gpointer user_data)
{
    MaliitInputMethod *input_method = MALIIT_INPUT_METHOD(user_data);

    maliit_input_method_hide(input_method);
}

static void
input_method_area_changed(MaliitInputMethod *input_method G_GNUC_UNUSED,
                          int x, int y, int width, int height,
                          gpointer user_data)
{
    GtkLabel *label = GTK_LABEL(user_data);
    gchar *formatted_string;

    if (width == 0 || height == 0)
        formatted_string = g_strdup("Empty Input Method area");
    else
        formatted_string = g_strdup_printf("Input Method area: [%d, %d, %d, %d]", x, y, width, height);

    gtk_label_set_text(label, formatted_string);

    g_free(formatted_string);
}

gint
main(gint    argc,
     gchar **argv)
{
    GtkWidget *window, *vbox;
    GtkWidget *show_button, *hide_button, *entry, *text_view, *label;
    MaliitInputMethod *input_method;

    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Maliit GTK+ example");

    input_method = maliit_input_method_new();

    show_button = gtk_button_new_with_label("Show keyboard");
    g_signal_connect(show_button, "clicked",
                     G_CALLBACK(show_button_clicked), input_method);
    hide_button = gtk_button_new_with_label("Hide keyboard");
    g_signal_connect(hide_button, "clicked",
                     G_CALLBACK(hide_button_clicked), input_method);
    entry = gtk_entry_new();
    text_view = gtk_text_view_new();
    gtk_widget_set_size_request(text_view, 150, 200);

    label = gtk_label_new("Empty Input Method area");
    g_signal_connect(input_method, "area-changed",
                     G_CALLBACK(input_method_area_changed), label);

#if GTK_MAJOR_VERSION >= 3
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
#else
    vbox = gtk_vbox_new(FALSE, 10);
#endif

    gtk_box_pack_start(GTK_BOX(vbox), show_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), hide_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), entry, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), text_view, TRUE, TRUE, 0); /* Expands */
    gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 0);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    g_signal_connect(G_OBJECT(window), "delete-event",
                     G_CALLBACK(gtk_main_quit), window);
    gtk_widget_show_all(window);

    gtk_main();

    g_object_unref(input_method);

    return 0;
}
