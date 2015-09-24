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
#include <gtk/gtk.h>

#ifdef HAVE_CONFIG_H
#include <build-config.h>
#endif

#include "qth_editor.h"

static GtkVBoxClass *parent_class = NULL;


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
}

static void qth_editor_init(QthEditor *editor)
{
    (void) editor;
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
    GtkWidget          *widget;

    widget = g_object_new(GTK_TYPE_QTH_EDITOR, NULL);
    QTH_EDITOR(widget)->update = qth_editor_update;

    return widget;
}


void qth_editor_update(GtkWidget *widget)
{
    (void)  widget;
}
