/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/* e-shell-view.h
 *
 * Copyright (C) 1999-2008 Novell, Inc. (www.novell.com)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of version 2 of the GNU General Public
 * License as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef E_SHELL_VIEW_H
#define E_SHELL_VIEW_H

#include "e-shell-common.h"

/* Standard GObject macros */
#define E_TYPE_SHELL_VIEW \
	(e_shell_view_get_type ())
#define E_SHELL_VIEW(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST \
	((obj), E_TYPE_SHELL_VIEW, EShellView))
#define E_SHELL_VIEW_CLASS(cls) \
	(G_TYPE_CHECK_CLASS_CAST \
	((cls), E_TYPE_SHELL_VIEW, EShellViewClass))
#define E_IS_SHELL_VIEW(obj) \
	(G_TYPE_CHECK_INSTANCE_TYPE \
	((obj), E_TYPE_SHELL_VIEW))
#define E_IS_SHELL_VIEW_CLASS(cls) \
	(G_TYPE_CHECK_CLASS_TYPE \
	((cls), E_TYPE_SHELL_VIEW))
#define E_SHELL_VIEW_GET_CLASS(obj) \
	(G_TYPE_INSTANCE_GET_CLASS \
	((obj), E_TYPE_SHELL_VIEW, EShellViewClass))

G_BEGIN_DECLS

typedef struct _EShellView EShellView;
typedef struct _EShellViewClass EShellViewClass;
typedef struct _EShellViewPrivate EShellViewPrivate;

struct _EShellView {
	GObject parent;
	EShellViewPrivate *priv;
};

struct _EShellViewClass {
	GObjectClass parent_class;

	/* Initial GtkRadioAction values */
	const gchar *label;
	const gchar *icon_name;

	GtkWidget *	(*create_new_menu)	(EShellView *shell_view);
	GtkWidget *	(*get_content_widget)	(EShellView *shell_view);
	GtkWidget *	(*get_sidebar_widget)	(EShellView *shell_view);
	GtkWidget *	(*get_status_widget)	(EShellView *shell_view);
};

GType		e_shell_view_get_type		(void);
const gchar *	e_shell_view_get_title		(EShellView *shell_view);
void		e_shell_view_set_title		(EShellView *shell_view,
						 const gchar *title);
GtkWidget *	e_shell_view_get_window		(EShellView *shell_view);
GtkWidget *	e_shell_view_create_new_menu	(EShellView *shell_view);
GtkWidget *	e_shell_view_get_content_widget (EShellView *shell_view);
GtkWidget *	e_shell_view_get_sidebar_widget (EShellView *shell_view);
GtkWidget *	e_shell_view_get_status_widget	(EShellView *shell_view);

void		e_shell_view_register_new_item_actions
						(EShellView *shell_view,
						 const GtkActionEntry *entries,
						 guint n_entries);
void		e_shell_view_register_new_source_actions
						(EShellView *shell_view,
						 const GtkActionEntry *entries,
						 guint n_entries);

G_END_DECLS

#endif /* E_SHELL_VIEW_H */
