/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*
 * folder-browser-factory.c: A Bonobo Control factory for Folder Browsers
 *
 * Author:
 *   Miguel de Icaza (miguel@helixcode.com)
 *
 * (C) 2000 Helix Code, Inc.
 */

#include <config.h>

#include <gnome.h>
#include <bonobo/bonobo-main.h>
#include <bonobo/bonobo-object.h>
#include <bonobo/bonobo-generic-factory.h>
#include <bonobo/bonobo-control.h>
#include <bonobo/bonobo-ui-component.h>

#include <gal/util/e-util.h>
#include <gal/widgets/e-gui-utils.h>

#include "widgets/menus/gal-view-menus.h"

#include <gal/menus/gal-view-factory-etable.h>
#include <gal/menus/gal-view-etable.h>

#include "folder-browser-factory.h"

#include "folder-browser.h"
#include "mail.h"
#include "mail-callbacks.h"
#include "shell/Evolution.h"
#include "mail-config.h"
#include "mail-ops.h"
#include "mail-session.h"

/* The FolderBrowser BonoboControls we have.  */
static EList *control_list = NULL;

/*
 * Add with 'folder_browser'
 */
BonoboUIVerb verbs [] = {
	BONOBO_UI_UNSAFE_VERB ("PrintMessage", print_msg),
	BONOBO_UI_UNSAFE_VERB ("PrintPreviewMessage", print_preview_msg),
	BONOBO_UI_UNSAFE_VERB ("MailGetSend", send_receive_mail),
	BONOBO_UI_UNSAFE_VERB ("MailCompose", compose_msg),
	
	/* Edit Menu */
	BONOBO_UI_UNSAFE_VERB ("EditSelectAll", select_all),
	BONOBO_UI_UNSAFE_VERB ("EditInvertSelection", invert_selection),
	
	/* Settings Menu */
	BONOBO_UI_UNSAFE_VERB ("SetMailFilter", filter_edit),
	BONOBO_UI_UNSAFE_VERB ("SetVFolder", vfolder_edit_vfolders),
	BONOBO_UI_UNSAFE_VERB ("SetMailConfig", providers_config),
	BONOBO_UI_UNSAFE_VERB ("SetSubscribe", manage_subscriptions),
	BONOBO_UI_UNSAFE_VERB ("SetForgetPwd", mail_session_forget_passwords),
	
	/* Message Menu */
	BONOBO_UI_UNSAFE_VERB ("MessageOpen", open_message),
	BONOBO_UI_UNSAFE_VERB ("MessageSaveAs", save_msg),
	BONOBO_UI_UNSAFE_VERB ("MessagePrint", print_msg),
	BONOBO_UI_UNSAFE_VERB ("MessageReplySndr", reply_to_sender),
	BONOBO_UI_UNSAFE_VERB ("MessageReplyAll", reply_to_all),
	BONOBO_UI_UNSAFE_VERB ("MessageForwardAttached", forward_attached),
	BONOBO_UI_UNSAFE_VERB ("MessageForwardInlined", forward_inlined),
	BONOBO_UI_UNSAFE_VERB ("MessageForwardQuoted", forward_quoted),
	BONOBO_UI_UNSAFE_VERB ("MessageForwardAttachment", forward_attached),
	
	BONOBO_UI_UNSAFE_VERB ("MessageMarkAsRead", mark_as_seen),
	BONOBO_UI_UNSAFE_VERB ("MessageMarkAsUnRead", mark_as_unseen),
	BONOBO_UI_UNSAFE_VERB ("MessageMarkAllAsRead", mark_all_as_seen),
	
	BONOBO_UI_UNSAFE_VERB ("MessageMove", move_msg),
	BONOBO_UI_UNSAFE_VERB ("MessageCopy", copy_msg),
	BONOBO_UI_UNSAFE_VERB ("MessageDelete", delete_msg),
	BONOBO_UI_UNSAFE_VERB ("MessageUndelete", undelete_msg),
	
	/*BONOBO_UI_UNSAFE_VERB ("MessageAddSenderToAddressBook", addrbook_sender),*/
	
	BONOBO_UI_UNSAFE_VERB ("MessageApplyFilters", apply_filters),
	
	BONOBO_UI_UNSAFE_VERB ("MessageVFolderSubj", vfolder_subject),
	BONOBO_UI_UNSAFE_VERB ("MessageVFolderSndr", vfolder_sender),
	BONOBO_UI_UNSAFE_VERB ("MessageVFolderRecip", vfolder_recipient),
	
	BONOBO_UI_UNSAFE_VERB ("MessageFilterSubj", filter_subject),
	BONOBO_UI_UNSAFE_VERB ("MessageFilterSndr", filter_sender),
	BONOBO_UI_UNSAFE_VERB ("MessageFilterRecip", filter_recipient),
	
	BONOBO_UI_UNSAFE_VERB ("MessageHideClear", hide_none),
	BONOBO_UI_UNSAFE_VERB ("MessageHideRead", hide_read),
	BONOBO_UI_UNSAFE_VERB ("MessageHideDeleted", hide_deleted),
	BONOBO_UI_UNSAFE_VERB ("MessageHideSelected", hide_selected),
	
	/* Folder Menu */
	BONOBO_UI_UNSAFE_VERB ("FolderExpunge", expunge_folder),
	BONOBO_UI_UNSAFE_VERB ("FolderConfig", configure_folder),
	
	/* Toolbar specific */
	BONOBO_UI_UNSAFE_VERB ("MailStop", stop_threads),
	BONOBO_UI_UNSAFE_VERB ("MailPrevious", previous_msg),
	BONOBO_UI_UNSAFE_VERB ("MailNext", next_msg),
	
	BONOBO_UI_VERB_END
};

static void
set_pixmap (BonoboUIComponent *uic,
	    const char        *xml_path,
	    const char        *icon)
{
	char *path;
	GdkPixbuf *pixbuf;

	path = g_concat_dir_and_file (EVOLUTION_DATADIR "/images/evolution", icon);

	pixbuf = gdk_pixbuf_new_from_file (path);
	if (pixbuf == NULL) {
		g_warning ("Cannot load image -- %s", path);
		g_free (path);
		return;
	}

	bonobo_ui_util_set_pixbuf (uic, xml_path, pixbuf);

	gdk_pixbuf_unref (pixbuf);

	g_free (path);
}

static void
update_pixmaps (BonoboUIComponent *uic)
{
	set_pixmap (uic, "/menu/File/Print/Print Preview", "16_print.xpm");

	set_pixmap (uic, "/menu/Component/Actions/MessageMove", "16_move_message.xpm");
	set_pixmap (uic, "/menu/Component/Actions/MessageReplyAll", "16_reply_to_all.xpm");
	set_pixmap (uic, "/menu/Component/Actions/MessageReplySndr", "16_reply.xpm");

	set_pixmap (uic, "/menu/File/Folder/FolderConfig", "16_configure_folder.xpm");

	set_pixmap (uic, "/menu/Tools/Component/SetMailConfig", "16_configure_mail.xpm");

	set_pixmap (uic, "/Toolbar/MailGet", "buttons/fetch-mail.png");
	set_pixmap (uic, "/Toolbar/MailCompose", "buttons/compose-message.png");
	set_pixmap (uic, "/Toolbar/Reply", "buttons/reply.png");
	set_pixmap (uic, "/Toolbar/ReplyAll", "buttons/reply-to-all.png");
	set_pixmap (uic, "/Toolbar/Forward", "buttons/forward.png");
	set_pixmap (uic, "/Toolbar/Move", "buttons/move-message.png");
	set_pixmap (uic, "/Toolbar/Copy", "buttons/copy-message.png");
}

static void
display_view(GalViewCollection *collection,
	     GalView *view,
	     gpointer data)
{
	FolderBrowser *fb = data;
	if (GAL_IS_VIEW_ETABLE(view)) {
		e_table_set_state_object(e_table_scrolled_get_table(E_TABLE_SCROLLED(fb->message_list)), GAL_VIEW_ETABLE(view)->state);
	}
}

static void
folder_browser_setup_view_menus (FolderBrowser *fb,
				 BonoboUIComponent *uic)
{
	GalViewCollection *collection;
	GalViewMenus *views;
	GalViewFactory *factory;
	ETableSpecification *spec;
	char *spec_string;

	collection = gal_view_collection_new();
	/* FIXME: Memory leak. */
	gal_view_collection_set_storage_directories(collection,
						    EVOLUTION_DATADIR "/evolution/views/mail/",
						    gnome_util_prepend_user_home("/evolution/views/mail/"));

	spec_string = message_list_get_layout(fb->message_list);
	spec = e_table_specification_new();
	e_table_specification_load_from_string(spec, spec_string);
	g_free(spec_string);

	factory = gal_view_factory_etable_new(spec);
	gal_view_collection_add_factory(collection, factory);
	gtk_object_sink(GTK_OBJECT(factory));

	gal_view_collection_load(collection);

	views = gal_view_menus_new(collection);
	gal_view_menus_apply(views, uic, NULL); /* This function probably needs to sink the views object. */
	gtk_signal_connect(GTK_OBJECT(collection), "display_view",
			   display_view, fb);
	/*	gtk_object_sink(GTK_OBJECT(views)); */

	gtk_object_sink(GTK_OBJECT(collection));
}

static void
folder_browser_setup_property_menu (FolderBrowser *fb,
				    BonoboUIComponent *uic)
{
	char *name, *base = NULL;

	if (fb->uri)
		base = g_basename (fb->uri);

	if (base && base [0] != 0)
		name = g_strdup_printf (_("Properties for \"%s\""), base);
	else
		name = g_strdup (_("Properties"));

	bonobo_ui_component_set_prop (
		uic, "/menu/File/Folder/FolderConfig",
		"label", name, NULL);
	g_free (name);
}

static void
control_activate (BonoboControl     *control,
		  BonoboUIComponent *uic,
		  FolderBrowser     *fb)
{
	GtkWidget         *folder_browser;
	Bonobo_UIContainer container;

	container = bonobo_control_get_remote_ui_container (control);
	bonobo_ui_component_set_container (uic, container);
	bonobo_object_release_unref (container, NULL);

	g_assert (container == bonobo_ui_component_get_container (uic));
	g_return_if_fail (container != CORBA_OBJECT_NIL);
		
	folder_browser = bonobo_control_get_widget (control);

	bonobo_ui_component_add_verb_list_with_data (
		uic, verbs, folder_browser);

	bonobo_ui_component_freeze (uic, NULL);

	bonobo_ui_util_set_ui (
		uic, EVOLUTION_DATADIR,
		"evolution-mail.xml", "evolution-mail");

	if (mail_config_get_thread_list ())
		bonobo_ui_component_set_prop (
			uic, "/commands/ViewThreaded", "state", "1", NULL);
	else
		bonobo_ui_component_set_prop (
			uic, "/commands/ViewThreaded", "state", "0", NULL);
	
	bonobo_ui_component_add_listener (
		uic, "ViewThreaded",
		folder_browser_toggle_threads, folder_browser);
	
	if (mail_config_get_view_source ())
		bonobo_ui_component_set_prop (uic, "/commands/ViewSource",
					      "state", "1", NULL);
	else
		bonobo_ui_component_set_prop (uic, "/commands/ViewSource",
					      "state", "0", NULL);
	
	bonobo_ui_component_add_listener (uic, "ViewSource",
					  folder_browser_toggle_view_source,
					  folder_browser);

	folder_browser_setup_view_menus (fb, uic);
	folder_browser_setup_property_menu (fb, uic);
	
	update_pixmaps (uic);

        bonobo_ui_component_set_prop(uic, "/commands/MailStop", "sensitive", "0", NULL);

	bonobo_ui_component_thaw (uic, NULL);
}

static void
control_deactivate (BonoboControl     *control,
		    BonoboUIComponent *uic,
		    FolderBrowser     *fb)
{
	bonobo_ui_component_rm (uic, "/", NULL);
 	bonobo_ui_component_unset_container (uic);

	if (fb->folder)
		mail_sync_folder (fb->folder, NULL, NULL);
}

static void
control_activate_cb (BonoboControl *control, 
		     gboolean activate, 
		     gpointer user_data)
{
	BonoboUIComponent *uic;

	uic = bonobo_control_get_ui_component (control);
	g_assert (uic != NULL);

	if (activate)
		control_activate (control, uic, user_data);
	else
		control_deactivate (control, uic, user_data);
}

static void
control_destroy_cb (BonoboControl *control,
		    GtkObject     *folder_browser)
{
	gtk_object_destroy (folder_browser);
}

static void
browser_destroy_cb (FolderBrowser *fb,
		    BonoboControl *control)
{
	EIterator *it;

	/* We do this from browser_destroy_cb rather than
	 * control_destroy_cb because currently, the controls
	 * don't seem to all get destroyed properly at quit
	 * time (but the widgets get destroyed by X). FIXME.
	 */

	for (it = e_list_get_iterator (control_list); e_iterator_is_valid (it); e_iterator_next (it)) {
		if (e_iterator_get (it) == control) {
			e_iterator_delete (it);
			break;
		}
	}
	gtk_object_unref (GTK_OBJECT (it));
}

BonoboControl *
folder_browser_factory_new_control (const char *uri,
				    const GNOME_Evolution_Shell shell)
{
	BonoboControl *control;
	GtkWidget *folder_browser;

	folder_browser = folder_browser_new (shell);
	if (folder_browser == NULL)
		return NULL;

	if (!folder_browser_set_uri (FOLDER_BROWSER (folder_browser), uri)) {
		gtk_object_sink (GTK_OBJECT (folder_browser));
		return NULL;
	}

	gtk_widget_show (folder_browser);
	
	control = bonobo_control_new (folder_browser);
	
	if (control == NULL) {
		gtk_object_destroy (GTK_OBJECT (folder_browser));
		return NULL;
	}
	
	gtk_signal_connect (GTK_OBJECT (control), "activate",
			    control_activate_cb, folder_browser);

	gtk_signal_connect (GTK_OBJECT (control), "destroy",
			    control_destroy_cb, folder_browser);
	gtk_signal_connect (GTK_OBJECT (folder_browser), "destroy",
			    browser_destroy_cb, control);

	if (!control_list)
		control_list = e_list_new (NULL, NULL, NULL);

	e_list_append (control_list, control);

	return control;
}

EList *
folder_browser_factory_get_control_list (void)
{
	if (!control_list)
		control_list = e_list_new (NULL, NULL, NULL);
	return control_list;
}
