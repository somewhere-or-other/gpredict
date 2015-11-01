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
#ifndef QTH_EDITOR_H
#define QTH_EDITOR_H 1

#include <glib.h>
#include <gtk/gtk.h>

#include "qth-data.h"

/* *INDENT-OFF* */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
/* *INDENT-ON* */


#define GTK_TYPE_QTH_EDITOR         (qth_editor_get_type())

#define QTH_EDITOR(obj)             G_TYPE_CHECK_INSTANCE_CAST(obj, \
                                        qth_editor_get_type(), \
                                        QthEditor)

#define QTH_EDITOR_CLASS(klass)     G_TYPE_CHECK_CLASS_CAST(klass, \
                                        qth_editor_get_type(), \
                                        QthEditorClass)

#define IS_QTH_EDITOR(obj)          G_TYPE_CHECK_INSTANCE_TYPE(obj, \
                                        qth_editor_get_type())

typedef struct _qth_editor          QthEditor;
typedef struct _QthEditorClass      QthEditorClass;

struct _qth_editor
{
    GtkVBox         vbox;

    /* active widgets */
    GtkWidget      *name_entry;
    GtkWidget      *qra_entry;
    GtkWidget      *lat_spin;
    GtkWidget      *lon_spin;
    GtkWidget      *alt_spin;
    GtkWidget      *ns;             /* North / south selector */
    GtkWidget      *ew;             /* East / west selector */

    /* signal IDs */
    gulong          qra_sigid;
    gulong          lon_sigid;
    gulong          lat_sigid;
    gulong          ns_sigid;
    gulong          ew_sigid;

    void          (* update) (GtkWidget *widget);
};

struct _QthEditorClass
{
    GtkVBoxClass       parent_class;

    /* changed signal */
    void (* changed)  (QthEditor * editor);
};

GType           qth_editor_get_type(void);
GtkWidget      *qth_editor_new(void);
void            qth_editor_get_qth(GtkWidget * widget, qth_t * qth);
void            qth_editor_update(GtkWidget * widget);

/* *INDENT-OFF* */
#ifdef __cplusplus
}
#endif /* __cplusplus */
/* *INDENT-ON* */

#endif      /* QTH_EDITOR_H */
