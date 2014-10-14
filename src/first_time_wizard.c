/*
 * Gpredict: Real-time satellite tracking and orbit prediction program
 *
 * Copyright (C)  2014  Alexandru Csete, OZ9AEC.
 *
 * Authors: Alexandru Csete <oz9aec@gmail.com>
 *
 * Comments, questions and bugreports should be submitted via
 * http://sourceforge.net/projects/gpredict/
 * More details can be found at the project home page:
 *
 * http://gpredict.oz9aec.net/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, visit http://www.fsf.org/
 */

#include <gtk/gtk.h>

#include "first_time_check.h"


static gint delete_wizard(GtkWidget * widget, GdkEvent * event, gpointer data)
{
    (void)widget;
    (void)event;
    (void)data;

    return FALSE;
}


static void destroy_wizard(GtkWidget * widget, gpointer data)
{
    (void)widget;
    (void)data;

    /* exit Gtk+ */
    gtk_main_quit();
}

/**
 * Signal handler for "cancel" button press.
 * @param wizard Pointer to our wizard widget.
 * @param data   Pointer to the user data (TBD)
 *
 * This function is called when the user presses the "cancel" button in any
 * of the wizard pages.
 */
static void cancel_wizard(GtkAssistant * wizard, gpointer data)
{
    /* this will trigger the destroy() signal */
    gtk_widget_destroy(wizard);
}

void first_time_wizard_run(int status)
{
    int             idx;
    GtkWidget      *wizard;

    wizard = gtk_assistant_new();
    idx = gtk_assistant_append_page(GTK_ASSISTANT(wizard), gtk_label_new("one"));
    idx = gtk_assistant_append_page(GTK_ASSISTANT(wizard), gtk_label_new("two"));
    idx = gtk_assistant_append_page(GTK_ASSISTANT(wizard), gtk_label_new("three"));


    g_signal_connect(G_OBJECT(wizard), "delete_event",
                     G_CALLBACK(delete_wizard), NULL);
    g_signal_connect(G_OBJECT(wizard), "destroy",
                     G_CALLBACK(destroy_wizard), NULL);
    g_signal_connect(G_OBJECT(wizard), "cancel",
                     G_CALLBACK(cancel_wizard), NULL);

    gtk_widget_show_all(wizard);

    gtk_main();

}

