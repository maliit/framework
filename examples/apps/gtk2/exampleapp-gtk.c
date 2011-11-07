/* * This file is part of meego-im-framework *
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * Contact: Nokia Corporation (directui@nokia.com)
 *
 * If you have questions regarding the use of this file, please contact
 * Nokia at directui@nokia.com.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 */

#include <gtk/gtk.h>

gint
main(gint    argc,
     gchar **argv)
{
    GtkWidget *window, *vbox;
    GtkWidget *button, *entry, *text_view;

    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Maliit GTK+ example");

    button = gtk_button_new_with_label("Hide keyboard");
    entry = gtk_entry_new();
    text_view = gtk_text_view_new();
    gtk_widget_set_size_request(text_view, 150, 200);
    vbox = gtk_vbox_new(FALSE, 10);

    gtk_box_pack_start(GTK_BOX(vbox), button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), entry, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), text_view, TRUE, TRUE, 0); /* Expands */
    gtk_container_add(GTK_CONTAINER(window), vbox);

    g_signal_connect(G_OBJECT(window), "delete-event",
                     G_CALLBACK(gtk_main_quit), window);
    gtk_widget_show_all(window);

    gtk_main();
    return 0;
}
