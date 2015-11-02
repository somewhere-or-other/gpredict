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
#include <glib.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>

#ifdef HAVE_CONFIG_H
#include <build-config.h>
#endif

#include "first_time_check.h"
#include "widgets/qth_editor.h"


/** Convenience struct with all wizard data. */
typedef struct {
    int             ftc_status;
    int             retcode;
    GtkWidget      *widget;
} wizard_t;

static gint delete_wizard(GtkWidget * widget, GdkEvent * event,
                          wizard_t * wizard)
{
    (void)widget;
    (void)event;
    (void)wizard;

    return FALSE;
}

static void destroy_wizard(GtkWidget * widget, wizard_t * wizard)
{
    (void)widget;
    (void)wizard;

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
static void cancel_wizard(GtkAssistant * assistant, wizard_t * wizard)
{
    wizard->retcode = -1;

    /** TODO: Undo everything **/

    /* this will trigger the destroy() signal */
    gtk_widget_destroy(GTK_WIDGET(assistant));
}

static void add_welcome_page(wizard_t * wizard)
{

#define text N_("<span weight='bold'>" \
                "I am gpredict version " VERSION "</span>\n\n" \
                "Before I can track satellites for you I need to know where "\
                "you are and which satellites you are interested in.\n\n"\
                "On the following pages I will guide through this "\
                "first time setup. Ready to go?")

    int             idx;
    GtkWidget      *page;       /* the current wizard page */
    GtkWidget      *vbox;       /* main vertical box */
    GtkWidget      *label;

    vbox = gtk_vbox_new(FALSE, 10);
    label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(label), text);
    gtk_label_set_line_wrap(GTK_LABEL(label), TRUE);

    gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 20);

    /* create new page and get a reference to it */
    idx = gtk_assistant_append_page(GTK_ASSISTANT(wizard->widget), vbox);
    page = gtk_assistant_get_nth_page(GTK_ASSISTANT(wizard->widget), idx);

    gtk_assistant_set_page_type(GTK_ASSISTANT(wizard->widget), page,
                                GTK_ASSISTANT_PAGE_INTRO);
    gtk_assistant_set_page_title(GTK_ASSISTANT(wizard->widget), page,
                                 _("Hi there!"));
    gtk_assistant_set_page_complete(GTK_ASSISTANT(wizard->widget), page, TRUE);
}

/**
 * Manage "changed" signals from the QTH editor widget.
 * @param widget Pointer to the QTH editor widget that emiutted the signal.
 * @param valid  A boolean flag indicating whether the current QTH data is valid.
 * @param wizard Pointer to user data, in this case to the wizard.
 *
 * We use the valid flag to decide whether to enable the "forward" button on
 * the current page.
 */
static void qth_changed(GtkWidget *widget, gboolean valid, wizard_t * wizard)
{
    GtkWidget   *page = gtk_assistant_get_nth_page(GTK_ASSISTANT(wizard->widget),
        gtk_assistant_get_current_page(GTK_ASSISTANT(wizard->widget)));

    gtk_assistant_set_page_complete(GTK_ASSISTANT(wizard->widget), page, valid);
}

static void add_qth_page(wizard_t * wizard)
{
    GtkWidget      *page;       /* the current wizard page */
    GtkWidget      *vbox;       /* main vertical box */
    GtkWidget      *editor;
    int             idx;

    /* FIXME: we should use external qth_editor object, but the present one
     * depends on sat-pref window?
     */
    editor = qth_editor_new();
    g_signal_connect(editor, "changed", G_CALLBACK(qth_changed), wizard);

    vbox = gtk_vbox_new(FALSE, 10);
    gtk_box_pack_start(GTK_BOX(vbox), editor, FALSE, FALSE, 20);

    /* create new page and get a reference to it */
    idx = gtk_assistant_append_page(GTK_ASSISTANT(wizard->widget), vbox);
    page = gtk_assistant_get_nth_page(GTK_ASSISTANT(wizard->widget), idx);

    gtk_assistant_set_page_type(GTK_ASSISTANT(wizard->widget), page,
                                GTK_ASSISTANT_PAGE_CONTENT);
    gtk_assistant_set_page_title(GTK_ASSISTANT(wizard->widget), page,
                                 _("Location info"));
}

static void add_satellite_page(wizard_t * wizard)
{
    GtkWidget      *vbox;
    GtkWidget      *page;
    int             idx;

    vbox = gtk_vbox_new(FALSE, 10);
    //gtk_box_pack_start(GTK_BOX(vbox), editor, FALSE, FALSE, 20);

    /* create new page and get a reference to it */
    idx = gtk_assistant_append_page(GTK_ASSISTANT(wizard->widget), vbox);
    page = gtk_assistant_get_nth_page(GTK_ASSISTANT(wizard->widget), idx);

    gtk_assistant_set_page_type(GTK_ASSISTANT(wizard->widget), page,
                                GTK_ASSISTANT_PAGE_CONTENT);
    gtk_assistant_set_page_title(GTK_ASSISTANT(wizard->widget), page,
                                 _("Satellite data"));

    //gtk_assistant_set_page_complete(GTK_ASSISTANT(wizard->widget), page, TRUE);
}

/**
 * Callback called when the progresspage is realized on the screen.
 * @param page Pointer to the progress page widget.
 * @param wizard Pointer to the wizard object.
 *
 * The progress page is used to show progress while saving QTH data as well as
 * downloading and importing satellite data from the selected sources.
 */
static void do_progress(GtkWidget * page, wizard_t * wizard)
{
    g_print("Go wizard, go!\n");
}

/**
 * Create a progress page.
 * @param wizard
 *
 * he progress page is used to show progress while saving QTH data as well as
 * downloading and importing satellite data from the selected sources.
 */
static void add_progress_page(wizard_t * wizard)
{
    GtkWidget      *vbox;
    GtkWidget      *page;
    int             idx;

    vbox = gtk_vbox_new(FALSE, 10);
    //gtk_box_pack_start(GTK_BOX(vbox), editor, FALSE, FALSE, 20);

    /* create new page and get a reference to it */
    idx = gtk_assistant_append_page(GTK_ASSISTANT(wizard->widget), vbox);
    page = gtk_assistant_get_nth_page(GTK_ASSISTANT(wizard->widget), idx);

    gtk_assistant_set_page_type(GTK_ASSISTANT(wizard->widget), page,
                                GTK_ASSISTANT_PAGE_PROGRESS);
    gtk_assistant_set_page_title(GTK_ASSISTANT(wizard->widget), page,
                                 _("Setting up stuff..."));

    g_signal_connect(G_OBJECT(page), "realize", G_CALLBACK(do_progress), wizard);
}

static void add_final_page(wizard_t * wizard)
{
    GtkWidget      *vbox;
    GtkWidget      *page;
    int             idx;

    vbox = gtk_vbox_new(FALSE, 10);
    //gtk_box_pack_start(GTK_BOX(vbox), editor, FALSE, FALSE, 20);

    /* create new page and get a reference to it */
    idx = gtk_assistant_append_page(GTK_ASSISTANT(wizard->widget), vbox);
    page = gtk_assistant_get_nth_page(GTK_ASSISTANT(wizard->widget), idx);

    gtk_assistant_set_page_type(GTK_ASSISTANT(wizard->widget), page,
                                GTK_ASSISTANT_PAGE_SUMMARY);
    gtk_assistant_set_page_title(GTK_ASSISTANT(wizard->widget), page,
                                 _("We are ready!"));
}

/**
 * Main entry point for first time configuration wizard.
 * @param status  The status bitfield returned by #first_time_check_run()
 * @retval     0  The configuration was successfull.
 * @retval    -1  The configuration was cancelled.
 */
int first_time_wizard_run(int status)
{
    int             retcode;
    wizard_t       *wizard = g_try_malloc(sizeof(wizard));

    if (wizard == NULL)
        return -1;

    wizard->ftc_status = status;
    wizard->widget = gtk_assistant_new();

    add_welcome_page(wizard);

    if (status & FTC_STATUS_NO_QTH)
        add_qth_page(wizard);

    if (status & FTC_STATUS_NO_SAT)
        add_satellite_page(wizard);

    add_progress_page(wizard);
    add_final_page(wizard);

    g_signal_connect(G_OBJECT(wizard->widget), "delete_event",
                     G_CALLBACK(delete_wizard), wizard);
    g_signal_connect(G_OBJECT(wizard->widget), "destroy",
                     G_CALLBACK(destroy_wizard), wizard);
    g_signal_connect(G_OBJECT(wizard->widget), "cancel",
                     G_CALLBACK(cancel_wizard), wizard);

    gtk_widget_show_all(wizard->widget);

    gtk_main();

    retcode = wizard->retcode;
    g_free(wizard);

    return retcode;
}
