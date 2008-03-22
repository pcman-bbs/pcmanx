/**
 * Copyright (c) 2005 PCMan <pcman.tw@gmail.com>
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
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "core/pcmanx_utils.h"

#if defined(HAVE_GETTEXT)
#include <libintl.h>
#define _(T) gettext(T)
#else
#define _(T) (T)
#endif
#if defined(HAVE_LC_MESSAGES)
#include <locale.h>
#endif

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <glib/goption.h>
#include <string.h>

#include "mainframe.h"
#include "appconfig.h"
#include "telnetcon.h"

#ifdef USE_DOCKLET
#include "docklet/api.h"
#endif

#ifdef USE_NOTIFIER
#ifdef USE_LIBNOTIFY
#include <libnotify/notify.h>
#else
#include "notifier/api.h"
#endif
#endif

#ifdef USE_SCRIPT
#include "script/api.h"
#endif

static int multiple_instance = 0;

static GOptionEntry entries[] = {
	{ (const gchar *) "multiple-instance", (gchar) 'm', 0, 
	  G_OPTION_ARG_NONE, &multiple_instance, 
	  (gchar *) "Allow multiple instances",
	  (gchar *) "N" },
	{ NULL }
};

int main(int argc, char *argv[])
{
	bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
	bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
	textdomain(GETTEXT_PACKAGE);

#ifdef USE_DEBUG
	/* well-known tip to figure out GObject runtime warnings. */
	setenv("G_DEBUG", "fatal_warnings", 1);
#endif

#ifdef USE_DEBUG
	/* glib introduces its own memory management mechanism, which
         * confuses memory debuggers such as valgrind and disable their
         * malloc/free wrapper against the applications.
         *
         * Here, we enforce glib to use malloc instead of original ones
         * for debugging need.
         */
	if (getenv("BYPASS_GLIB_POOLS") != NULL) {
		g_slice_set_config(G_SLICE_CONFIG_ALWAYS_MALLOC, TRUE);

	}
#endif

	if (!g_thread_supported ())
		g_thread_init (NULL);

	/*--- Initialize Gtk+ ---*/
	{
		/* GTK requires a program's argc and argv variables, and
		 * requires that they be valid. Set it up. */
		int fake_argc = 1;
		char *_fake_argv[] = { "", NULL };
		char **fake_argv = _fake_argv;

		gtk_init (&fake_argc, &fake_argv);
	}

	/*--- Initialize Runtime options ---*/
	{
		GError *error = NULL;
		GOptionContext *context;
		context = g_option_context_new ("Runtime options");
		g_option_context_add_main_entries (context, entries, GETTEXT_PACKAGE);
		g_option_context_parse (context, &argc, &argv, &error);
	}

	/*--- Check if multiple-instance is allowed. ---*/
	if (!multiple_instance) {
#ifdef USE_DOCKLET
		/* if we are already running, silently exit */
		if (! detect_get_clipboard())
		{
#ifndef USE_DEBUG
			return 1;
#endif	/* USE_DEBUG */
		}
#endif	/* USE_DOCKLET */
	}

	AppConfig.SetToDefault();
	AppConfig.Load();
	AppConfig.LoadFavorites();
	if(AppConfig.RowsPerPage < 24)
		AppConfig.RowsPerPage=24;
	if(AppConfig.ColsPerPage<80)
		AppConfig.ColsPerPage=80;

	CTelnetCon::Init();
	CTelnetCon::SetSocketTimeout( AppConfig.SocketTimeout );

	CMainFrame* main_frm = new CMainFrame;
	gtk_window_move(GTK_WINDOW(main_frm->m_Widget), AppConfig.MainWndX, AppConfig.MainWndY);
	gtk_window_resize(GTK_WINDOW(main_frm->m_Widget), AppConfig.MainWndW, AppConfig.MainWndH);
	main_frm->Show();
#ifdef USE_DOCKLET
	if( AppConfig.ShowTrayIcon )
		main_frm->ShowTrayIcon();
#endif

#ifdef USE_NOTIFIER
#ifdef USE_LIBNOTIFY
	if (!notify_is_initted())
	{
	  notify_init("pcmanx-gtk2");
	}
#else
	popup_notifier_init(main_frm->GetMainIcon());
	popup_notifier_set_timeout( AppConfig.PopupTimeout );
#endif
#endif

#ifdef USE_SCRIPT
	InitScriptInterface(".");
#endif

	gtk_main ();

#ifdef USE_LIBNOTIFY
	notify_uninit();
#endif	
	CTelnetCon::Cleanup();

	AppConfig.SaveFavorites();
	AppConfig.Save();

	return 0;
}

