/***************************************************************************
 *   Copyright (C) 2005 by PCMan   *
 *   hzysoft@sina.com.tw   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "mainframe.h"
#include "appconfig.h"

int main(int argc, char *argv[])
{
	bind_textdomain_codeset("pcmanx", "UTF-8");
	textdomain("pcmanx");

	if (!g_thread_supported ())
		g_thread_init (NULL);

	int fake_argc = 1;
	char **fake_argv;

	/* GTK requires a program's argc and argv variables, and 
	 * requires that they be valid. Set it up. */
	fake_argv = (char **) g_malloc (sizeof (char *) * 2);
	fake_argv[0] = (char *) g_malloc(1);
	strcpy(argv[0], "");
	argv[1] = NULL;
		  
	gtk_init (&fake_argc, &fake_argv);

	g_free (fake_argv[0]);
	g_free (fake_argv);
  
	CWidget::Init();

	AppConfig.SetToDefault();
	AppConfig.Load();
	AppConfig.LoadFavorites();
	if(AppConfig.RowsPerPage < 24)
		AppConfig.RowsPerPage=24;
	if(AppConfig.ColsPerPage<80)
		AppConfig.ColsPerPage=80;

	CMainFrame* main_frm = new CMainFrame;
	gtk_window_move(GTK_WINDOW(main_frm->m_Widget), 40, 40);
	gtk_window_resize(GTK_WINDOW(main_frm->m_Widget), 640, 480);
	main_frm->Show();

	gtk_main ();

	AppConfig.SaveFavorites();
	AppConfig.Save();

	return 0;
}

