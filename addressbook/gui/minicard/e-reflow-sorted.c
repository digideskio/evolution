/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * e-reflow-sorted.c
 * Copyright (C) 2000  Helix Code, Inc.
 * Author: Chris Lahey <clahey@helixcode.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include <config.h>
#include <math.h>
#include <gnome.h>
#include "e-reflow-sorted.h"
#include <e-util/e-canvas-utils.h>
#include <e-util/e-canvas.h>
#include <e-util/e-util.h>

static void e_reflow_sorted_init		(EReflowSorted		 *card);
static void e_reflow_sorted_class_init	(EReflowSortedClass	 *klass);
static void e_reflow_sorted_set_arg (GtkObject *o, GtkArg *arg, guint arg_id);
static void e_reflow_sorted_get_arg (GtkObject *object, GtkArg *arg, guint arg_id);
static void e_reflow_sorted_add_item(EReflow *e_reflow, GnomeCanvasItem *item);

#define E_REFLOW_DIVIDER_WIDTH 2
#define E_REFLOW_BORDER_WIDTH 7
#define E_REFLOW_FULL_GUTTER (E_REFLOW_DIVIDER_WIDTH + E_REFLOW_BORDER_WIDTH * 2)

static EReflowClass *parent_class = NULL;

/* The arguments we take */
enum {
	ARG_0,
	ARG_COMPARE_FUNC,
	ARG_STRING_FUNC
};

GtkType
e_reflow_sorted_get_type (void)
{
  static GtkType reflow_type = 0;

  if (!reflow_type)
    {
      static const GtkTypeInfo reflow_info =
      {
        "EReflowSorted",
        sizeof (EReflowSorted),
        sizeof (EReflowSortedClass),
        (GtkClassInitFunc) e_reflow_sorted_class_init,
        (GtkObjectInitFunc) e_reflow_sorted_init,
        /* reserved_1 */ NULL,
        /* reserved_2 */ NULL,
        (GtkClassInitFunc) NULL,
      };

      reflow_type = gtk_type_unique (e_reflow_get_type (), &reflow_info);
    }

  return reflow_type;
}

static void
e_reflow_sorted_class_init (EReflowSortedClass *klass)
{
	GtkObjectClass *object_class;
	EReflowClass *reflow_class;
	
	object_class = (GtkObjectClass*) klass;
	reflow_class = E_REFLOW_CLASS (klass);
	
	parent_class = gtk_type_class (e_reflow_get_type ());
	
	gtk_object_add_arg_type ("EReflowSorted::compare_func", GTK_TYPE_POINTER,
				 GTK_ARG_READWRITE, ARG_COMPARE_FUNC);
	gtk_object_add_arg_type ("EReflowSorted::string_func", GTK_TYPE_POINTER,
				 GTK_ARG_READWRITE, ARG_STRING_FUNC);
	
	reflow_class->add_item  = e_reflow_sorted_add_item;
	
	object_class->set_arg   = e_reflow_sorted_set_arg;
	object_class->get_arg   = e_reflow_sorted_get_arg;
}

static void
e_reflow_sorted_init (EReflowSorted *reflow)
{
	reflow->compare_func = NULL;
	reflow->string_func = NULL;
}

static void
e_reflow_sorted_set_arg (GtkObject *o, GtkArg *arg, guint arg_id)
{
	GnomeCanvasItem *item;
	EReflowSorted *e_reflow_sorted;

	item = GNOME_CANVAS_ITEM (o);
	e_reflow_sorted = E_REFLOW_SORTED (o);
	
	switch (arg_id){
	case ARG_COMPARE_FUNC:
		e_reflow_sorted->compare_func = GTK_VALUE_POINTER (*arg);
		break;
	case ARG_STRING_FUNC:
		e_reflow_sorted->string_func  = GTK_VALUE_POINTER (*arg);
		break;
	}
}

static void
e_reflow_sorted_get_arg (GtkObject *object, GtkArg *arg, guint arg_id)
{
	EReflowSorted *e_reflow_sorted;

	e_reflow_sorted = E_REFLOW_SORTED (object);

	switch (arg_id) {
	case ARG_COMPARE_FUNC:
		GTK_VALUE_POINTER (*arg) = e_reflow_sorted->compare_func;
		break;
	case ARG_STRING_FUNC:
		GTK_VALUE_POINTER (*arg) = e_reflow_sorted->string_func;
		break;
	default:
	  arg->type = GTK_TYPE_INVALID;
	  break;
	}
}

static GList *
e_reflow_sorted_get_list(EReflowSorted *e_reflow_sorted, const gchar *id)
{
	if (e_reflow_sorted->string_func) {
		EReflow *reflow = E_REFLOW(e_reflow_sorted);
		GList *list;
		for (list = reflow->items; list; list = g_list_next(list)) {
			GnomeCanvasItem *item = list->data;
			char *string = e_reflow_sorted->string_func (item);
			if (string && !strcmp(string, id)) {
				return list;
			}
		}
	}
	return NULL;
}

void
e_reflow_sorted_remove_item(EReflowSorted *e_reflow_sorted, const gchar *id)
{
	GList *list;
	GnomeCanvasItem *item = NULL;

	list = e_reflow_sorted_get_list(e_reflow_sorted, id);
	if (list)
		item = list->data;

	if (item) {
		EReflow *reflow = E_REFLOW(e_reflow_sorted);
		reflow->items = g_list_remove_link(reflow->items, list);
		g_list_free_1(list);
		gtk_object_unref(GTK_OBJECT(item));
		gtk_object_destroy(GTK_OBJECT(item));
		if ( GTK_OBJECT_FLAGS( e_reflow_sorted ) & GNOME_CANVAS_ITEM_REALIZED ) {
			e_canvas_item_request_reflow(GNOME_CANVAS_ITEM(e_reflow_sorted));
			e_reflow_post_add_item(E_REFLOW(e_reflow_sorted), NULL);
		}
	}
}

void
e_reflow_sorted_replace_item(EReflowSorted *e_reflow_sorted, GnomeCanvasItem *item)
{
	if (e_reflow_sorted->string_func) {
		char *string = e_reflow_sorted->string_func (item);
		e_reflow_sorted_remove_item(e_reflow_sorted, string);
		e_reflow_sorted_add_item(E_REFLOW(e_reflow_sorted), item);
	}
}

GnomeCanvasItem *
e_reflow_sorted_get_item(EReflowSorted *e_reflow_sorted, const gchar *id)
{
	GList *list;
	list = e_reflow_sorted_get_list(e_reflow_sorted, id);
	if (list)
		return list->data;
	else
		return NULL;
}

void
e_reflow_sorted_reorder_item(EReflowSorted *e_reflow_sorted, const gchar *id)
{
	GList *list;
	GnomeCanvasItem *item = NULL;

	list = e_reflow_sorted_get_list(e_reflow_sorted, id);
	if (list)
		item = list->data;
	if (item) {
		EReflow *reflow = E_REFLOW(e_reflow_sorted);
		reflow->items = g_list_remove_link(reflow->items, list);
		gtk_object_unref(GTK_OBJECT(item));
		g_list_free_1(list);
		e_reflow_sorted_add_item(reflow, item);
		e_canvas_item_request_reflow(GNOME_CANVAS_ITEM(e_reflow_sorted));
	}
}

static void
e_reflow_sorted_add_item(EReflow *reflow, GnomeCanvasItem *item)
{
	EReflowSorted *e_reflow_sorted = E_REFLOW_SORTED(reflow);
	if ( e_reflow_sorted->compare_func ) {
		reflow->items = g_list_insert_sorted(reflow->items, item, e_reflow_sorted->compare_func);
		gtk_object_ref(GTK_OBJECT(item));

		if ( GTK_OBJECT_FLAGS( e_reflow_sorted ) & GNOME_CANVAS_ITEM_REALIZED ) {
			gnome_canvas_item_set(item,
					      "width", (double) reflow->column_width,
					      NULL);

			e_canvas_item_request_reflow(GNOME_CANVAS_ITEM(e_reflow_sorted));
		}
	}
	e_reflow_post_add_item(reflow, item);
}

void e_reflow_sorted_jump   (EReflowSorted *sorted,
			     GCompareFunc   compare_func,
			     void          *value)
{
	int columns = 0;
	EReflow *reflow = E_REFLOW(sorted);
	GList *list;

	for (list = reflow->columns; list; list = g_list_next(list)) {
		if (compare_func(((GList *)list->data)->data, value) >= 0) {
			GList *last = list->prev;
			GtkAdjustment *adjustment;
			if (last) {
				GList *walk;
				for (walk = last->data; walk != list->data; walk = g_list_next(walk)) {
					if (compare_func(walk->data, value) >= 0) {
						columns --;
						break;
					}
				}
			}
			adjustment = gtk_layout_get_hadjustment(GTK_LAYOUT(GNOME_CANVAS_ITEM(sorted)->canvas));
			gtk_adjustment_set_value(adjustment, (reflow->column_width + E_REFLOW_FULL_GUTTER) * columns);
			return;
		}
		columns ++;
	}
}
