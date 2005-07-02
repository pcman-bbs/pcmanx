/*
 * src/clipboard.c - X clipboard hack to detect if X application is running
 *
 * Copyright (c) 2005
 *	Jim Huang <jserv@kaffe.org>
 *
 * Copyright (c) 1999
 *	Elliot Lee <sopwith@redhat.com>, Red Hat, Inc.
 *
 * Licensed under the GNU GPL v2.  See COPYING.
 */

#include <gdk/gdkx.h>
#include <gtk/gtk.h>

#define CLIPBOARD_NAME \
	"PCMANX_SELECTION"

/*
 * clipboard_get_func - dummy get_func for gtk_clipboard_set_with_data ()
 */
static void
clipboard_get_func(
		GtkClipboard *clipboard G_GNUC_UNUSED,
		GtkSelectionData *selection_data G_GNUC_UNUSED,
		guint info G_GNUC_UNUSED,
		gpointer user_data_or_owner G_GNUC_UNUSED)
{
}

/*
 * clipboard_clear_func - dummy clear_func for gtk_clipboard_set_with_data ()
 */
static void clipboard_clear_func(
		GtkClipboard *clipboard G_GNUC_UNUSED,
		gpointer user_data_or_owner G_GNUC_UNUSED)
{
}

/*
 * netapplet_get_clipboard - try and get the CLIPBOARD_NAME clipboard
 *
 * Returns TRUE if successfully retrieved and FALSE otherwise.
 */
gboolean _get_clipboard()
{
	static const GtkTargetEntry targets[] = { {CLIPBOARD_NAME, 0, 0} };
	gboolean retval = FALSE;
	GtkClipboard *clipboard;
	Atom atom;

	atom = gdk_x11_get_xatom_by_name(CLIPBOARD_NAME);

	XGrabServer(GDK_DISPLAY());

	if (XGetSelectionOwner(GDK_DISPLAY(), atom) != None)
		goto out;

	clipboard = gtk_clipboard_get(gdk_atom_intern(CLIPBOARD_NAME, FALSE));

	if (gtk_clipboard_set_with_data(
			clipboard, targets,
			G_N_ELEMENTS (targets),
			clipboard_get_func,
			clipboard_clear_func, NULL))
		retval = TRUE;

out:
	XUngrabServer (GDK_DISPLAY ());
	gdk_flush ();

	return retval;
}
