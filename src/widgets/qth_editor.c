/*
 * Gpredict: Real-time satellite tracking and orbit prediction program
 *
 * Copyright (C) 2015  Alexandru Csete, OZ9AEC.
 *
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
#include <math.h>

#ifdef HAVE_CONFIG_H
#include <build-config.h>
#endif

#include "locator.h"
#include "qth_editor.h"
#include "sat-log.h"

static void     name_changed(GtkWidget * widget, gpointer data);
static void     latlon_changed(GtkWidget * widget, gpointer data);
static void     qra_changed(GtkEntry * entry, gpointer data);

static GtkVBoxClass *parent_class = NULL;
static guint qth_editor_signals[1] = { 0 };

static void qth_editor_destroy(GtkObject *object)
{
    //QthEditor       *editor = QTH_EDITOR(object);

    (* GTK_OBJECT_CLASS(parent_class)->destroy) (object);
}

static void qth_editor_class_init(QthEditorClass *class)
{
    GtkObjectClass    *object_class;

    object_class = (GtkObjectClass*) class;
    parent_class = g_type_class_peek_parent(class);
    object_class->destroy = qth_editor_destroy;

    qth_editor_signals[0] = g_signal_new("changed",
        G_TYPE_FROM_CLASS(class),
        G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
        G_STRUCT_OFFSET(QthEditorClass, changed),
        NULL, NULL,
        g_cclosure_marshal_VOID__VOID,
        G_TYPE_NONE,
        0); /* FIXME: Add gboolean G_TYPE_BOOLEAN */
}

static void qth_editor_init(QthEditor *editor)
{
    editor->name_entry = NULL;
    editor->qra_entry = NULL;
    editor->lat_spin = NULL;
    editor->lon_spin = NULL;
    editor->alt_spin = NULL;
    editor->ns = NULL;
    editor->ew = NULL;
}


GType qth_editor_get_type()
{
    static GType        qth_editor_type = 0;

    if (!qth_editor_type) {

        static const GTypeInfo qth_editor_info = {
            sizeof (QthEditorClass),
            NULL,  /* base_init */
            NULL,  /* base_finalize */
            (GClassInitFunc) qth_editor_class_init,
            NULL,  /* class_finalize */
            NULL,  /* class_data */
            sizeof (QthEditor),
            5,     /* n_preallocs */
            (GInstanceInitFunc) qth_editor_init,
            NULL
        };

        qth_editor_type = g_type_register_static(GTK_TYPE_VBOX,
                                                 "QthEditor",
                                                 &qth_editor_info,
                                                 0);
    }

    return qth_editor_type;
}

GtkWidget *qth_editor_new()
{
    GtkWidget      *widget;
    GtkWidget      *table;
    GtkWidget      *label;

    widget = g_object_new(GTK_TYPE_QTH_EDITOR, NULL);

    QthEditor *editor = QTH_EDITOR(widget);

    editor->update = qth_editor_update;

    table = gtk_table_new(5, 4, FALSE);
    gtk_table_set_col_spacings(GTK_TABLE(table), 10);
    gtk_table_set_row_spacings(GTK_TABLE(table), 5);

    /* QTH name */
    label = gtk_label_new(_("Name:"));
    gtk_misc_set_alignment(GTK_MISC(label), 1.0, 0.5);
    gtk_table_attach_defaults(GTK_TABLE(table), label, 0, 1, 0, 1);
    editor->name_entry = gtk_entry_new();
    g_signal_connect(editor->name_entry, "changed", G_CALLBACK(name_changed), editor);
    gtk_entry_set_max_length(GTK_ENTRY(editor->name_entry), 100);
    gtk_table_attach_defaults(GTK_TABLE(table), editor->name_entry, 1, 4, 0, 1);

    /* QTH locator */
    label = gtk_label_new(_("Grid square:"));
    gtk_misc_set_alignment(GTK_MISC(label), 1.0, 0.5);
    gtk_table_attach_defaults(GTK_TABLE(table), label, 0, 1, 1, 2);
    editor->qra_entry = gtk_entry_new();
    gtk_entry_set_max_length(GTK_ENTRY(editor->qra_entry), 6);
    gtk_table_attach_defaults(GTK_TABLE(table), editor->qra_entry, 1, 2, 1, 2);
    editor->qra_sigid = g_signal_connect(editor->qra_entry, "changed",
        G_CALLBACK(qra_changed), editor);

    /* QTH longitude */
    label = gtk_label_new(_("Longitude:"));
    gtk_misc_set_alignment(GTK_MISC(label), 1.0, 0.5);
    gtk_table_attach_defaults(GTK_TABLE(table), label, 0, 1, 2, 3);

    editor->lon_spin = gtk_spin_button_new_with_range(0.0, 180.0, 0.01);
    gtk_spin_button_set_increments(GTK_SPIN_BUTTON(editor->lon_spin), 0.01, 1.0);
    gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(editor->lon_spin), TRUE);
    gtk_spin_button_set_digits(GTK_SPIN_BUTTON(editor->lon_spin), 4);
    gtk_table_attach_defaults(GTK_TABLE(table), editor->lon_spin, 1, 2, 2, 3);
    editor->lon_sigid = g_signal_connect(editor->lon_spin, "value-changed",
        G_CALLBACK(latlon_changed), editor);

    label = gtk_label_new(_("deg"));
    gtk_misc_set_alignment(GTK_MISC(label), 0.0, 0.5);
    gtk_table_attach_defaults(GTK_TABLE(table), label, 2, 3, 2, 3);

    editor->ew = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(editor->ew), _("East"));
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(editor->ew), _("West"));
    gtk_combo_box_set_active(GTK_COMBO_BOX(editor->ew), 0);
    gtk_table_attach_defaults(GTK_TABLE(table), editor->ew, 3, 4, 2, 3);
    editor->ew_sigid = g_signal_connect(editor->ew, "changed",
        G_CALLBACK(latlon_changed), editor);

    /* QTH lattitude */
    label = gtk_label_new(_("Lattitude:"));
    gtk_misc_set_alignment(GTK_MISC(label), 1.0, 0.5);
    gtk_table_attach_defaults(GTK_TABLE(table), label, 0, 1, 3, 4);

    editor->lat_spin = gtk_spin_button_new_with_range(0.0, 90.0, 0.01);
    gtk_spin_button_set_increments(GTK_SPIN_BUTTON(editor->lat_spin), 0.01, 1.0);
    gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(editor->lat_spin), TRUE);
    gtk_spin_button_set_digits(GTK_SPIN_BUTTON(editor->lat_spin), 4);
    gtk_table_attach_defaults(GTK_TABLE(table), editor->lat_spin, 1, 2, 3, 4);
    editor->lat_sigid = g_signal_connect(editor->lat_spin, "value-changed",
        G_CALLBACK(latlon_changed), editor);

    label = gtk_label_new(_("deg"));
    gtk_misc_set_alignment(GTK_MISC(label), 0.0, 0.5);
    gtk_table_attach_defaults(GTK_TABLE(table), label, 2, 3, 3, 4);

    editor->ns = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(editor->ns), _("North"));
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(editor->ns), _("South"));
    gtk_combo_box_set_active(GTK_COMBO_BOX(editor->ns), 0);
    gtk_table_attach_defaults(GTK_TABLE(table), editor->ns, 3, 4, 3, 4);
    editor->ns_sigid = g_signal_connect(editor->ns, "changed",
        G_CALLBACK(latlon_changed), editor);

    /* QTH altitude */
    label = gtk_label_new(_("Altitude:"));
    gtk_misc_set_alignment(GTK_MISC(label), 1.0, 0.5);
    gtk_table_attach_defaults(GTK_TABLE(table), label, 0, 1, 4, 5);
    editor->alt_spin = gtk_spin_button_new_with_range(-1000.0, 20000.0, 1);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(editor->alt_spin), 0);
    gtk_table_attach_defaults(GTK_TABLE(table), editor->alt_spin, 1, 2, 4, 5);
    label = gtk_label_new(_("m"));
    gtk_misc_set_alignment(GTK_MISC(label), 0.0, 0.5);
    gtk_table_attach_defaults(GTK_TABLE(table), label, 2, 3, 4, 5);

    gtk_box_pack_start(GTK_BOX(widget), table, TRUE, TRUE, 5);
    gtk_container_set_border_width(GTK_CONTAINER(widget), 20);

    return widget;
}

void qth_editor_get_qth(GtkWidget * widget, qth_t * qth)
{
    QthEditor *editor = QTH_EDITOR(widget);

    if (qth->name)
        g_free(qth->name);

    if (qth->qra)
        g_free(qth->qra);

    qth->name = g_strdup(gtk_entry_get_text(GTK_ENTRY(editor->name_entry)));
    qth->qra = g_strdup(gtk_entry_get_text(GTK_ENTRY(editor->qra_entry)));
    qth->lon = gtk_spin_button_get_value(GTK_SPIN_BUTTON(editor->lon_spin));
    if (gtk_combo_box_get_active(GTK_COMBO_BOX(editor->ew)))
        qth->lon = -qth->lon;

    qth->lat = gtk_spin_button_get_value(GTK_SPIN_BUTTON(editor->lat_spin));
    if (gtk_combo_box_get_active(GTK_COMBO_BOX(editor->ns)))
        qth->lat = -qth->lat;

    qth->alt = gtk_spin_button_get_value(GTK_SPIN_BUTTON(editor->alt_spin));
}

void qth_editor_update(GtkWidget *widget)
{
    (void)  widget;
}

/**
 * Manage name changes.
 *
 * This function is called when the contents of the name entry changes.
 * The primary purpose of this function is to ensure that only valid characters
 * are entered and to check whether the length of the name is greater than
 * zero.
 */
static void name_changed(GtkWidget * widget, gpointer data)
{
    const gchar    *text;
    gchar          *entry, *end, *j;
    gint            len, pos;

    QthEditor *editor = QTH_EDITOR(data);

    /* step 1: ensure that only valid characters are entered
       (stolen from xlog, tnx pg4i)
     */
    entry = gtk_editable_get_chars(GTK_EDITABLE(widget), 0, -1);
    if ((len = g_utf8_strlen(entry, -1)) > 0)
    {
        end = entry + g_utf8_strlen(entry, -1);
        for (j = entry; j < end; ++j)
        {
            switch (*j)
            {
            case '0' ... '9':
            case 'a' ... 'z':
            case 'A' ... 'Z':
            case ' ':
            case '.':
            case '-':
            case '_':
                break;
            default:
                gdk_beep();
                pos = gtk_editable_get_position(GTK_EDITABLE(widget));
                gtk_editable_delete_text(GTK_EDITABLE(widget), pos, pos + 1);
                break;
            }
        }
    }

    text = gtk_entry_get_text(GTK_ENTRY(widget));
    if (g_utf8_strlen(text, -1) > 0)
        g_signal_emit(editor, qth_editor_signals[0], 0);
}


/**
 * Manage lattitude and longitude changes.
 *
 * This function is called when the qth coordinates change. The change can
 * be either one of the spin buttons or the combo boxes. It reads the
 * coordinates and the calculates the new Maidenhead locator square.
 */
static void latlon_changed(GtkWidget * widget, gpointer data)
{
    gchar          *locator;
    gint            retcode;
    gdouble         latf, lonf;

    QthEditor *editor = QTH_EDITOR(data);
    (void)widget;

    locator = g_try_malloc(7);

    /* no need to check locator != NULL, since hamlib func will do it for us
     * and return RIGEINVAL
     */
    lonf = gtk_spin_button_get_value(GTK_SPIN_BUTTON(editor->lon_spin));
    latf = gtk_spin_button_get_value(GTK_SPIN_BUTTON(editor->lat_spin));

    /* set the correct sign */
    if (gtk_combo_box_get_active(GTK_COMBO_BOX(editor->ns)))
    {
        /* index 1 => South */
        latf = -latf;
    }

    if (gtk_combo_box_get_active(GTK_COMBO_BOX(editor->ew)))
    {
        /* index 1 => West */
        lonf = -lonf;
    }

    retcode = longlat2locator(lonf, latf, locator, 3);

    if (retcode == RIG_OK)
    {
        /* debug message */
        sat_log_log(SAT_LOG_LEVEL_DEBUG,
                    _("%s:%s: %.2f %.2f => %s"),
                    __FILE__, __func__, lonf, latf, locator);

        g_signal_handler_block(editor->qra_entry, editor->qra_sigid);
        gtk_entry_set_text(GTK_ENTRY(editor->qra_entry), locator);
        g_signal_handler_unblock(editor->qra_entry, editor->qra_sigid);
    }
    else
    {
        /* send an error message and don't update */
        sat_log_log(SAT_LOG_LEVEL_ERROR,
                    _("%s:%d: Error converting lon/lat to locator"),
                    __FILE__, __LINE__);
    }

    if (locator)
        g_free(locator);
}

/**
 * Manage locator changes.
 *
 * This function is called when the Maidenhead locator is changed.
 * It will calculate the new coordinates and update the spin butrtons and
 * the combo boxes.
 */
static void qra_changed(GtkEntry * entry, gpointer data)
{
    gint            retcode;
    gdouble         latf, lonf;
    gchar          *msg;

    (void) entry;
    QthEditor *editor = QTH_EDITOR(data);

    retcode = locator2longlat(&lonf, &latf,
        gtk_entry_get_text(GTK_ENTRY(editor->qra_entry)));

    if (retcode == RIG_OK)
    {
        sat_log_log(SAT_LOG_LEVEL_DEBUG,  _("%s:%s: %s => %.2f %.2f"),
                    __FILE__, __func__,
                    gtk_entry_get_text(GTK_ENTRY(editor->qra_entry)),
                    lonf, latf);

        /* block signal emissions for lat/lon widgets */
        g_signal_handler_block(editor->lat_spin, editor->lat_sigid);
        g_signal_handler_block(editor->lon_spin, editor->lon_sigid);
        g_signal_handler_block(editor->ns, editor->ns_sigid);
        g_signal_handler_block(editor->ew, editor->ew_sigid);
        g_signal_handler_block(editor->qra_entry, editor->qra_sigid);

        /* update widgets */
        if (latf > 0.0)
            gtk_combo_box_set_active(GTK_COMBO_BOX(editor->ns), 0);
        else
            gtk_combo_box_set_active(GTK_COMBO_BOX(editor->ns), 1);

        if (lonf > 0.0)
            gtk_combo_box_set_active(GTK_COMBO_BOX(editor->ew), 0);
        else
            gtk_combo_box_set_active(GTK_COMBO_BOX(editor->ew), 1);

        gtk_spin_button_set_value(GTK_SPIN_BUTTON(editor->lat_spin), fabs(latf));
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(editor->lon_spin), fabs(lonf));

        /* make sure text is upper case */
        msg = g_ascii_strup(gtk_entry_get_text(GTK_ENTRY(editor->qra_entry)), -1);
        gtk_entry_set_text(GTK_ENTRY(editor->qra_entry), msg);
        g_free(msg);

        /* unblock signal emissions */
        g_signal_handler_unblock(editor->lat_spin, editor->lat_sigid);
        g_signal_handler_unblock(editor->lon_spin, editor->lon_sigid);
        g_signal_handler_unblock(editor->ns, editor->ns_sigid);
        g_signal_handler_unblock(editor->ew, editor->ew_sigid);
        g_signal_handler_unblock(editor->qra_entry, editor->qra_sigid);
    }
    else
    {
        sat_log_log(SAT_LOG_LEVEL_ERROR,  _("%s:%d: Invalid locator: %s"),
                    __FILE__, __LINE__,
                    gtk_entry_get_text(GTK_ENTRY(editor->qra_entry)));
    }

}
