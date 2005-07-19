/**
 * Copyright (c) 2005 PCMan <hzysoft@sina.com.tw>
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

#ifdef __GNUG__
  #pragma implementation "mainframe.h"
#endif


#include <glib/gi18n.h>
#include <gdk/gdkkeysyms.h>
#include <stdio.h>

#include "mainframe.h"

#include "telnetview.h"
#include "notebook.h"
#include "telnetcon.h"

#include "inputdialog.h"
#include "editfavdlg.h"
#include "sitedlg.h"
#include "prefdlg.h"
#include "appconfig.h"
#include "sitelistdlg.h"
#include "font.h"
#include "emoticondlg.h"


#ifdef USE_DOCKLET
#include "docklet/eggtrayicon.h"


void CMainFrame::OnTrayButton_Toggled(
	GtkToggleButton *button,
	CMainFrame *_this)
{
	static bool hide_next_time = TRUE;
	if (hide_next_time) {
		_this->Hide();
		hide_next_time = FALSE;
	}
	else {
		_this->Show();
		hide_next_time = TRUE;
	}
}

/*
void CMainFrame::OnTrayButton_Changed(GtkWidget* widget, GtkAllocation *allocation, CMainFrame* _this)
{
	if (! _this->m_MainIcon)
		return;
	_this->set_tray_icon();
}
*/

void CMainFrame::set_tray_icon()
{
        int panel_w;
        int panel_h;
        gtk_window_get_size (GTK_WINDOW(gtk_widget_get_toplevel(m_TrayButton)),
                        &panel_w, &panel_h);

        int icon_size = (panel_h < 30) ? 16 : 24;
        GdkPixbuf *tray_icon_pixbuf = gdk_pixbuf_copy (m_MainIcon);

        /*
         * Scale the icon rather than clip it if our allocation just isn't
         * what we want it to be.
         */
        int ori_icon_size = gdk_pixbuf_get_height(tray_icon_pixbuf);
		if( ori_icon_size > icon_size )
        {
                int new_size = icon_size;
                GdkPixbuf *new_pixbuf;

                new_pixbuf = gdk_pixbuf_scale_simple (tray_icon_pixbuf,
                                new_size, new_size,
                                GDK_INTERP_BILINEAR);

                g_object_unref (tray_icon_pixbuf);
                tray_icon_pixbuf = new_pixbuf;
        }

        gtk_image_set_from_pixbuf(GTK_IMAGE (m_TrayIcon), tray_icon_pixbuf);
        g_object_unref (tray_icon_pixbuf);
}
#endif

gboolean CMainFrame::OnSize( GtkWidget* widget, GdkEventConfigure* evt, CMainFrame* _this )
{
	gtk_window_get_position( GTK_WINDOW(widget), &AppConfig.MainWndX, & AppConfig.MainWndY );
	AppConfig.MainWndW = evt->width;
	AppConfig.MainWndH = evt->height;
//	g_print("x=%d, y=%d, w=%d, h=%d\n", evt->x, evt->y, evt->width, evt->height );
//	g_print("get_pos: x=%d, y=%d\n",AppConfig.MainWndX, AppConfig.MainWndY );
	return false;
}

CMainFrame::CMainFrame()
{
	m_pView = NULL;
	m_FavoritesMenuItem = NULL;
	m_FavoritesMenu = NULL;
	m_IsFlashing = false;
	m_IsActivated = false;

	m_Widget = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	PostCreate();

#ifdef USE_DOCKLET
	/* We need to make sure m_MainIcon is null at startup. */
	m_MainIcon = (GdkPixbuf *) NULL;
#endif

	LoadIcons();

#ifdef USE_DOCKLET
	m_TrayIcon_Instance = egg_tray_icon_new ("applet");

	m_TrayButton = gtk_toggle_button_new ();
	gtk_button_set_relief (GTK_BUTTON (m_TrayButton), GTK_RELIEF_NONE);
	gtk_container_add (GTK_CONTAINER (m_TrayIcon_Instance), m_TrayButton);
	gtk_widget_show (m_TrayButton);

	g_signal_connect (G_OBJECT (m_TrayButton), "toggled",
			G_CALLBACK (CMainFrame::OnTrayButton_Toggled), this);

/*
	g_signal_connect (G_OBJECT (m_TrayButton), "size-allocate",
			G_CALLBACK (CMainFrame::OnTrayButton_Changed), this);
*/

	m_TrayIcon = gtk_image_new ();
	gtk_container_add (GTK_CONTAINER (m_TrayButton), m_TrayIcon);
	gtk_widget_show (m_TrayIcon);

	set_tray_icon();
#endif


	gtk_window_set_title (GTK_WINDOW (m_Widget), "PCMan X "VERSION );
	
	m_pNotebook = new CNotebook();
	gtk_notebook_set_scrollable(GTK_NOTEBOOK(m_pNotebook->m_Widget), true);
	
	CreateMenu();
	CreateToolbar();

	gtk_window_set_icon((GtkWindow*)m_Widget, m_MainIcon);

	GtkWidget* vbox = gtk_vbox_new(false, 0);
	gtk_widget_show (vbox);

	GtkWidget* status_bar = gtk_statusbar_new ();
	gtk_widget_show (status_bar);

	gtk_container_add(GTK_CONTAINER(m_Widget), vbox);

	gtk_box_pack_start (GTK_BOX (vbox), m_Menubar, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (vbox), m_Toolbar, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (vbox), m_pNotebook->m_Widget, TRUE, TRUE, 0);
	gtk_widget_set_size_request(m_pNotebook->m_Widget, 300, 200);
	gtk_box_pack_start (GTK_BOX (vbox), status_bar, FALSE, FALSE, 0);

	gtk_widget_grab_focus(m_pNotebook->m_Widget);

	GTK_WIDGET_UNSET_FLAGS(m_pNotebook->m_Widget, GTK_CAN_FOCUS);

	g_signal_connect(G_OBJECT(m_Widget), "delete-event", G_CALLBACK(CMainFrame::OnClose), this);

	g_signal_connect(m_pNotebook->m_Widget, "switch-page", G_CALLBACK(CMainFrame::OnNotebookChangeCurPage), this);

	g_signal_connect(m_Widget, "configure-event", G_CALLBACK(CMainFrame::OnSize), this);

	g_signal_connect(G_OBJECT(m_Widget), "focus-in-event", G_CALLBACK(CMainFrame::OnActivated), this);

	g_signal_connect(G_OBJECT(m_Widget), "focus-out-event", G_CALLBACK(CMainFrame::OnDeactivated), this);

	m_BlinkTimer = g_timeout_add(600, (GSourceFunc)CMainFrame::OnBlinkTimer, this );
	m_EverySecondTimer = g_timeout_add(1000, (GSourceFunc)CMainFrame::OnEverySecondTimer, this );

	CTelnetView::SetParentFrame(this);
}

CMainFrame::~CMainFrame()
{
}


CTelnetCon* CMainFrame::NewCon(const char* title, const char* url, CSite* site )
{
	m_pView = new CTelnetView;
	m_Views.push_back(m_pView);

	int idx = m_pNotebook->AddPage( m_pView, title, m_ConnIcon );

	CTelnetCon* pCon;

	pCon = new CTelnetCon( m_pView, *site );

	m_pView->m_pTermData = pCon;
	m_pView->SetContextMenu(m_EditMenu);
	CFont* font = new CFont(AppConfig.FontFamily, AppConfig.FontSize, AppConfig.AntiAliasFont);
	m_pView->SetFont(font);
	m_pView->SetHyperLinkColor( &AppConfig.HyperLinkColor );
	m_pView->SetHorizontalCenterAlign( AppConfig.HCenterAlign );
	m_pView->m_CharPaddingX = AppConfig.CharPaddingX;
	m_pView->m_CharPaddingY = AppConfig.CharPaddingY;

	pCon->m_Site.m_Name = title;
	pCon->m_Site.m_URL = url;
	pCon->m_Encoding = pCon->m_Site.m_Encoding;

	pCon->AllocScreenBuf( site->m_RowsPerPage, site->m_RowsPerPage, site->m_ColsPerPage );
	
	pCon->Connect();

	m_pNotebook->SetCurPage(idx);

	m_pView->SetFocus();

	return pCon;
}

void CMainFrame::CreateMenu()
{
	GtkWidget *connect;
	GtkWidget *connect_menu;
	GtkWidget *site_list_menu;
	GtkWidget *image487;
	GtkWidget *new_con_menu;
	GtkWidget *image488;
	GtkWidget *reconnect_menu;
	GtkWidget *image489;
	GtkWidget *close_menu;
	GtkWidget *image490;
	GtkWidget *separatormenuitem1;
	GtkWidget *next_con_menu;
	GtkWidget *image491;
	GtkWidget *previous_con_menu;
	GtkWidget *image492;
	GtkWidget *jump;
	GtkWidget *image493;
	GtkWidget *separator2;
	GtkWidget *quit_menu;
	GtkWidget *image494;
	GtkWidget *edit;
	GtkWidget *edit_menu;
	GtkWidget *copy_menu;
	GtkWidget *image495;
	GtkWidget *copy_with_ansi_menu;
	GtkWidget *image496;
	GtkWidget *paste_menu;
	GtkWidget *image497;
	GtkWidget *select_all_menu;
	GtkWidget *separator1;
	GtkWidget *emoticon_menu;
	GtkWidget *preference_menu;
	GtkWidget *image498;
	GtkWidget *favorites;
	GtkWidget *favorites_menu;
	GtkWidget *separator3;
	GtkWidget *add_to_fav_menu;
	GtkWidget *image499;
	GtkWidget *edit_fav_menu;
	GtkWidget *image500;
	GtkWidget *view;
	GtkWidget *view_menu;
	GtkWidget *font_menu;
	GtkWidget *help;
	GtkWidget *help_menu;
	GtkWidget *about_menu;

	GtkAccelGroup *accel_group = gtk_accel_group_new ();

	m_Menubar = gtk_menu_bar_new ();
	gtk_widget_show (m_Menubar);

	connect = gtk_menu_item_new_with_mnemonic (_("_Connect"));
	gtk_widget_show (connect);
	gtk_container_add (GTK_CONTAINER (m_Menubar), connect);
	
	connect_menu = gtk_menu_new ();
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (connect), connect_menu);
	
	site_list_menu = gtk_image_menu_item_new_with_mnemonic (_("_Site List"));
	gtk_widget_show (site_list_menu);
	gtk_container_add (GTK_CONTAINER (connect_menu), site_list_menu);
	gtk_widget_add_accelerator (site_list_menu, "activate", accel_group,
								GDK_S, GDK_MOD1_MASK,
								GTK_ACCEL_VISIBLE);
	
	image487 = gtk_image_new_from_stock ("gtk-open", GTK_ICON_SIZE_MENU);
	gtk_widget_show (image487);
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (site_list_menu), image487);
	
	new_con_menu = gtk_image_menu_item_new_with_mnemonic (_("_New Connection"));
	gtk_widget_show (new_con_menu);
	gtk_container_add (GTK_CONTAINER (connect_menu), new_con_menu);
	gtk_widget_add_accelerator (new_con_menu, "activate", accel_group,
								GDK_Q, GDK_MOD1_MASK,
								GTK_ACCEL_VISIBLE);
	
	image488 = gtk_image_new_from_stock ("gtk-network", GTK_ICON_SIZE_MENU);
	gtk_widget_show (image488);
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (new_con_menu), image488);
	
	reconnect_menu = gtk_image_menu_item_new_with_mnemonic (_("_Reconnect"));
	gtk_widget_show (reconnect_menu);
	gtk_container_add (GTK_CONTAINER (connect_menu), reconnect_menu);
	gtk_widget_add_accelerator (reconnect_menu, "activate", accel_group,
								GDK_R, GDK_MOD1_MASK,
								GTK_ACCEL_VISIBLE);
	
	image489 = gtk_image_new_from_stock ("gtk-undo", GTK_ICON_SIZE_MENU);
	gtk_widget_show (image489);
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (reconnect_menu), image489);
	
	close_menu = gtk_image_menu_item_new_with_mnemonic (_("_Close Connection"));
	gtk_widget_show (close_menu);
	gtk_container_add (GTK_CONTAINER (connect_menu), close_menu);
	gtk_widget_add_accelerator (close_menu, "activate", accel_group,
								GDK_W, GDK_MOD1_MASK,
								GTK_ACCEL_VISIBLE);
	
	image490 = gtk_image_new_from_stock ("gtk-close", GTK_ICON_SIZE_MENU);
	gtk_widget_show (image490);
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (close_menu), image490);
	
	separatormenuitem1 = gtk_separator_menu_item_new ();
	gtk_widget_show (separatormenuitem1);
	gtk_container_add (GTK_CONTAINER (connect_menu), separatormenuitem1);
	gtk_widget_set_sensitive (separatormenuitem1, FALSE);
	
	next_con_menu = gtk_image_menu_item_new_with_mnemonic (_("Next Page"));
	gtk_widget_show (next_con_menu);
	gtk_container_add (GTK_CONTAINER (connect_menu), next_con_menu);
	gtk_widget_add_accelerator (next_con_menu, "activate", accel_group,
								GDK_X, GDK_MOD1_MASK,
								GTK_ACCEL_VISIBLE);
	
	image491 = gtk_image_new_from_stock ("gtk-go-down", GTK_ICON_SIZE_MENU);
	gtk_widget_show (image491);
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (next_con_menu), image491);
	
	previous_con_menu = gtk_image_menu_item_new_with_mnemonic (_("Previous Page"));
	gtk_widget_show (previous_con_menu);
	gtk_container_add (GTK_CONTAINER (connect_menu), previous_con_menu);
	gtk_widget_add_accelerator (previous_con_menu, "activate", accel_group,
								GDK_Z, GDK_MOD1_MASK,
								GTK_ACCEL_VISIBLE);
	
	image492 = gtk_image_new_from_stock ("gtk-go-up", GTK_ICON_SIZE_MENU);
	gtk_widget_show (image492);
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (previous_con_menu), image492);
	
	jump = gtk_image_menu_item_new_with_mnemonic (_("_Jump to"));
	gtk_widget_show (jump);
	gtk_container_add (GTK_CONTAINER (connect_menu), jump);
	
	image493 = gtk_image_new_from_stock ("gtk-jump-to", GTK_ICON_SIZE_MENU);
	gtk_widget_show (image493);
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (jump), image493);
	
	separator2 = gtk_separator_menu_item_new ();
	gtk_widget_show (separator2);
	gtk_container_add (GTK_CONTAINER (connect_menu), separator2);
	gtk_widget_set_sensitive (separator2, FALSE);
	
	quit_menu = gtk_image_menu_item_new_with_mnemonic (_("_Quit"));
	gtk_widget_show (quit_menu);
	gtk_container_add (GTK_CONTAINER (connect_menu), quit_menu);
	
	image494 = gtk_image_new_from_stock ("gtk-quit", GTK_ICON_SIZE_MENU);
	gtk_widget_show (image494);
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (quit_menu), image494);
	
	edit = gtk_menu_item_new_with_mnemonic (_("_Edit"));
	gtk_widget_show (edit);
	gtk_container_add (GTK_CONTAINER (m_Menubar), edit);
	
	edit_menu = gtk_menu_new ();
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (edit), edit_menu);
	
	copy_menu = gtk_image_menu_item_new_with_mnemonic (_("_Copy"));
	gtk_widget_show (copy_menu);
	gtk_container_add (GTK_CONTAINER (edit_menu), copy_menu);
	gtk_widget_add_accelerator (copy_menu, "activate", accel_group,
								GDK_O, GDK_MOD1_MASK,
								GTK_ACCEL_VISIBLE);
	
	image495 = gtk_image_new_from_stock ("gtk-copy", GTK_ICON_SIZE_MENU);
	gtk_widget_show (image495);
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (copy_menu), image495);
	
	copy_with_ansi_menu = gtk_image_menu_item_new_with_mnemonic (_("Copy with A_NSI Color"));
	gtk_widget_show (copy_with_ansi_menu);
	gtk_container_add (GTK_CONTAINER (edit_menu), copy_with_ansi_menu);
	
	image496 = gtk_image_new_from_stock ("gtk-select-color", GTK_ICON_SIZE_MENU);
	gtk_widget_show (image496);
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (copy_with_ansi_menu), image496);
	
	paste_menu = gtk_image_menu_item_new_with_mnemonic (_("_Paste"));
	gtk_widget_show (paste_menu);
	gtk_container_add (GTK_CONTAINER (edit_menu), paste_menu);
	gtk_widget_add_accelerator (paste_menu, "activate", accel_group,
								GDK_P, GDK_MOD1_MASK,
								GTK_ACCEL_VISIBLE);
	
	image497 = gtk_image_new_from_stock ("gtk-paste", GTK_ICON_SIZE_MENU);
	gtk_widget_show (image497);
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (paste_menu), image497);
	
	select_all_menu = gtk_menu_item_new_with_mnemonic (_("Select A_ll"));
	gtk_widget_show (select_all_menu);
	gtk_container_add (GTK_CONTAINER (edit_menu), select_all_menu);
	
	separator1 = gtk_separator_menu_item_new ();
	gtk_widget_show (separator1);
	gtk_container_add (GTK_CONTAINER (edit_menu), separator1);
	gtk_widget_set_sensitive (separator1, FALSE);
	
	emoticon_menu = gtk_menu_item_new_with_mnemonic (_("_Emoticons"));
	gtk_widget_show (emoticon_menu);
	gtk_container_add (GTK_CONTAINER (edit_menu), emoticon_menu);
	gtk_widget_add_accelerator (emoticon_menu, "activate", accel_group,
								GDK_Return, GDK_CONTROL_MASK,
								GTK_ACCEL_VISIBLE);
	
	preference_menu = gtk_image_menu_item_new_with_mnemonic (_("_Preference"));
	gtk_widget_show (preference_menu);
	gtk_container_add (GTK_CONTAINER (edit_menu), preference_menu);
	
	image498 = gtk_image_new_from_stock ("gtk-preferences", GTK_ICON_SIZE_MENU);
	gtk_widget_show (image498);
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (preference_menu), image498);
	
	favorites = gtk_menu_item_new_with_mnemonic (_("F_avorites"));
	gtk_widget_show (favorites);
	gtk_container_add (GTK_CONTAINER (m_Menubar), favorites);
	
	favorites_menu = gtk_menu_new ();
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (favorites), favorites_menu);
	
	separator3 = gtk_separator_menu_item_new ();
	gtk_widget_show (separator3);
	gtk_container_add (GTK_CONTAINER (favorites_menu), separator3);
	gtk_widget_set_sensitive (separator3, FALSE);
	
	add_to_fav_menu = gtk_image_menu_item_new_with_mnemonic (_("_Add to Favorites"));
	gtk_widget_show (add_to_fav_menu);
	gtk_container_add (GTK_CONTAINER (favorites_menu), add_to_fav_menu);
	
	image499 = gtk_image_new_from_stock ("gtk-add", GTK_ICON_SIZE_MENU);
	gtk_widget_show (image499);
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (add_to_fav_menu), image499);
	
	edit_fav_menu = gtk_image_menu_item_new_with_mnemonic (_("_Edit Favorites"));
	gtk_widget_show (edit_fav_menu);
	gtk_container_add (GTK_CONTAINER (favorites_menu), edit_fav_menu);
	
	image500 = gtk_image_new_from_stock ("gtk-edit", GTK_ICON_SIZE_MENU);
	gtk_widget_show (image500);
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (edit_fav_menu), image500);
	
	view = gtk_menu_item_new_with_mnemonic (_("_View"));
	gtk_widget_show (view);
	gtk_container_add (GTK_CONTAINER (m_Menubar), view);
	
	view_menu = gtk_menu_new ();
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (view), view_menu);
	
	font_menu = gtk_image_menu_item_new_from_stock ("gtk-select-font", accel_group);
	gtk_widget_show (font_menu);
	gtk_container_add (GTK_CONTAINER (view_menu), font_menu);
	
	help = gtk_menu_item_new_with_mnemonic (_("_Help"));
	gtk_widget_show (help);
	gtk_container_add (GTK_CONTAINER (m_Menubar), help);
	
	help_menu = gtk_menu_new ();
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (help), help_menu);
	
	about_menu = gtk_image_menu_item_new_from_stock ("gtk-about", accel_group);
	gtk_widget_show (about_menu);
	gtk_container_add (GTK_CONTAINER (help_menu), about_menu);

	gtk_window_add_accel_group (GTK_WINDOW (m_Widget), accel_group);

	g_signal_connect ((gpointer) site_list_menu, "activate",
					G_CALLBACK (CMainFrame::OnSiteList),
					this);
	g_signal_connect ((gpointer) new_con_menu, "activate",
					G_CALLBACK (CMainFrame::OnNewCon),
					this);
	g_signal_connect ((gpointer) reconnect_menu, "activate",
					G_CALLBACK (CMainFrame::OnReconnect),
					this);
	g_signal_connect ((gpointer) close_menu, "activate",
					G_CALLBACK (CMainFrame::OnCloseCon),
					this);
	g_signal_connect ((gpointer) next_con_menu, "activate",
					G_CALLBACK (CMainFrame::OnNextCon),
					this);
	g_signal_connect ((gpointer) previous_con_menu, "activate",
					G_CALLBACK (CMainFrame::OnPrevCon),
					this);
	g_signal_connect ((gpointer) quit_menu, "activate",
					G_CALLBACK (CMainFrame::OnQuit),
					this);
	g_signal_connect ((gpointer) copy_menu, "activate",
					G_CALLBACK (CMainFrame::OnCopy),
					this);
	g_signal_connect ((gpointer) copy_with_ansi_menu, "activate",
					G_CALLBACK (CMainFrame::OnCopyWithColor),
					this);
	g_signal_connect ((gpointer) paste_menu, "activate",
					G_CALLBACK (CMainFrame::OnPaste),
					this);
	g_signal_connect ((gpointer) select_all_menu, "activate",
					G_CALLBACK (CMainFrame::OnSelectAll),
					this);	
	g_signal_connect ((gpointer) emoticon_menu, "activate",
					G_CALLBACK (CMainFrame::OnEmoticons),
					this);	
	g_signal_connect ((gpointer) preference_menu, "activate",
					G_CALLBACK (CMainFrame::OnPreference),
					this);
	g_signal_connect ((gpointer) font_menu, "activate",
					G_CALLBACK (CMainFrame::OnFont),
					this);
	g_signal_connect ((gpointer) about_menu, "activate",
					G_CALLBACK (CMainFrame::OnAbout),
					this);

	m_EditMenu = edit_menu;
	m_FavoritesMenuItem = favorites;
	m_AccelGroup = accel_group;

	GtkWidget* jump_menu = gtk_menu_new ();
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (jump), jump_menu);

	const char* page_str = _("Page");
	for(int i = 1; i < 11; i++)
	{
		long keyval = (i == 10 ? GDK_0 : GDK_0 + +i);
		char title[32];
		sprintf(title, "%s _%d", page_str, i);

		GtkWidget* jump_item = m_JumpMenuItems[i-1] = gtk_menu_item_new_with_mnemonic ( title );
		gtk_widget_show (jump_item);
		gtk_container_add (GTK_CONTAINER (jump_menu), jump_item);
		gtk_widget_add_accelerator (jump_item, "activate", accel_group,
								keyval, (GdkModifierType) GDK_MOD1_MASK,
								GTK_ACCEL_VISIBLE);
		g_signal_connect( G_OBJECT(jump_item), "activate",
        		          G_CALLBACK (CMainFrame::OnJumpToPage),
                		  this);
	}

	CreateFavoritesMenu();
}

void CMainFrame::CreateToolbar()
{
	GtkWidget *toolbar;
	GtkIconSize tmp_toolbar_icon_size;
	GtkWidget *site_list_btn;
	GtkWidget *new_con_btn;
	GtkWidget *recon_btn;
	GtkWidget *close_btn;
	GtkWidget *separatortoolitem2;
	GtkWidget *copy_btn;
	GtkWidget *copy_ansi_btn;
	GtkWidget *paste_btn;
	GtkWidget *separatortoolitem1;
	GtkWidget *add_to_fav_btn;
	GtkWidget *pref_btn;
	GtkWidget *about_btn;
	GtkAccelGroup *accel_group;
	GtkTooltips *tooltips;
	
	tooltips = gtk_tooltips_new ();

	toolbar = gtk_toolbar_new ();
	gtk_widget_show (toolbar);
	gtk_toolbar_set_style (GTK_TOOLBAR (toolbar), GTK_TOOLBAR_ICONS);
	tmp_toolbar_icon_size = gtk_toolbar_get_icon_size (GTK_TOOLBAR (toolbar));
	
	site_list_btn = (GtkWidget*) gtk_tool_button_new_from_stock ("gtk-open");
	gtk_widget_show (site_list_btn);
	gtk_container_add (GTK_CONTAINER (toolbar), site_list_btn);
	gtk_tool_item_set_tooltip (GTK_TOOL_ITEM (site_list_btn), tooltips, _("Site List"), NULL);
	
	new_con_btn = (GtkWidget*) gtk_tool_button_new_from_stock ("gtk-network");
	gtk_widget_show (new_con_btn);
	gtk_container_add (GTK_CONTAINER (toolbar), new_con_btn);
	gtk_tool_item_set_tooltip (GTK_TOOL_ITEM (new_con_btn), tooltips, _("New Connection"), NULL);

	recon_btn = (GtkWidget*) gtk_tool_button_new_from_stock ("gtk-undo");
	gtk_widget_show (recon_btn);
	gtk_container_add (GTK_CONTAINER (toolbar), recon_btn);
	gtk_tool_item_set_tooltip (GTK_TOOL_ITEM (recon_btn), tooltips, _("Reconnect"), NULL);

	close_btn = (GtkWidget*) gtk_tool_button_new_from_stock ("gtk-close");
	gtk_widget_show (close_btn);
	gtk_container_add (GTK_CONTAINER (toolbar), close_btn);
	gtk_tool_item_set_tooltip (GTK_TOOL_ITEM (close_btn), tooltips, _("Close Connection"), NULL);
	
	separatortoolitem2 = (GtkWidget*) gtk_separator_tool_item_new ();
	gtk_widget_show (separatortoolitem2);
	gtk_container_add (GTK_CONTAINER (toolbar), separatortoolitem2);
	
	copy_btn = (GtkWidget*) gtk_tool_button_new_from_stock ("gtk-copy");
	gtk_widget_show (copy_btn);
	gtk_container_add (GTK_CONTAINER (toolbar), copy_btn);
	gtk_tool_item_set_tooltip (GTK_TOOL_ITEM (copy_btn), tooltips, _("Copy"), NULL);
	
	copy_ansi_btn = (GtkWidget*) gtk_tool_button_new_from_stock ("gtk-select-color");
	gtk_widget_show (copy_ansi_btn);
	gtk_container_add (GTK_CONTAINER (toolbar), copy_ansi_btn);
	gtk_tool_item_set_tooltip (GTK_TOOL_ITEM (copy_ansi_btn), tooltips, _("Copy with ANSI Color"), NULL);
	
	paste_btn = (GtkWidget*) gtk_tool_button_new_from_stock ("gtk-paste");
	gtk_widget_show (paste_btn);
	gtk_container_add (GTK_CONTAINER (toolbar), paste_btn);
	gtk_tool_item_set_tooltip (GTK_TOOL_ITEM (paste_btn), tooltips, _("Paste"), NULL);
	
	separatortoolitem1 = (GtkWidget*) gtk_separator_tool_item_new ();
	gtk_widget_show (separatortoolitem1);
	gtk_container_add (GTK_CONTAINER (toolbar), separatortoolitem1);
	
	add_to_fav_btn = (GtkWidget*) gtk_tool_button_new_from_stock ("gtk-add");
	gtk_widget_show (add_to_fav_btn);
	gtk_container_add (GTK_CONTAINER (toolbar), add_to_fav_btn);
	gtk_tool_item_set_tooltip (GTK_TOOL_ITEM (add_to_fav_btn), tooltips, _("Add to Favorites"), NULL);
	
	pref_btn = (GtkWidget*) gtk_tool_button_new_from_stock ("gtk-preferences");
	gtk_widget_show (pref_btn);
	gtk_container_add (GTK_CONTAINER (toolbar), pref_btn);
	gtk_tool_item_set_tooltip (GTK_TOOL_ITEM (pref_btn), tooltips, _("Preference"), NULL);
	
	about_btn = (GtkWidget*) gtk_tool_button_new_from_stock ("gtk-about");
	gtk_widget_show (about_btn);
	gtk_container_add (GTK_CONTAINER (toolbar), about_btn);
	gtk_tool_item_set_tooltip (GTK_TOOL_ITEM (about_btn), tooltips, _("About"), NULL);


	m_Toolbar = toolbar;

	g_signal_connect ((gpointer) site_list_btn, "clicked",
					G_CALLBACK (CMainFrame::OnSiteList),
					this);
	g_signal_connect ((gpointer) new_con_btn, "clicked",
					G_CALLBACK (CMainFrame::OnNewCon),
					this);
	g_signal_connect ((gpointer) recon_btn, "clicked",
					G_CALLBACK (CMainFrame::OnReconnect),
					this);
	g_signal_connect ((gpointer) close_btn, "clicked",
					G_CALLBACK (CMainFrame::OnCloseCon),
					this);
	g_signal_connect ((gpointer) copy_btn, "clicked",
					G_CALLBACK (CMainFrame::OnCopy),
					this);
	g_signal_connect ((gpointer) copy_ansi_btn, "clicked",
					G_CALLBACK (CMainFrame::OnCopyWithColor),
					this);
	g_signal_connect ((gpointer) paste_btn, "clicked",
					G_CALLBACK (CMainFrame::OnPaste),
					this);
	g_signal_connect ((gpointer) pref_btn, "clicked",
					G_CALLBACK (CMainFrame::OnPreference),
					this);
	g_signal_connect ((gpointer) add_to_fav_btn, "clicked",
					G_CALLBACK (CMainFrame::OnAddToFavorites),
					this);
	g_signal_connect ((gpointer) about_btn, "clicked",
					G_CALLBACK (CMainFrame::OnAbout),
					this);
					

	GtkWidget* sep = (GtkWidget*)gtk_separator_tool_item_new();
	gtk_widget_show(sep);
	gtk_container_add (GTK_CONTAINER (toolbar), sep);

	// Create the URL address bar
	GtkWidget* url_bar = gtk_hbox_new (FALSE, 0);
	GtkWidget* url_label = (GtkWidget*) gtk_label_new_with_mnemonic(_("A_ddress:"));
	m_URLEntry = (GtkWidget*) gtk_entry_new();
	gtk_widget_set_size_request(m_URLEntry, 0, -1);
	gtk_tooltips_set_tip(tooltips, m_URLEntry, _("Type URL here, then hit \"Enter\""), NULL);
	gtk_label_set_mnemonic_widget(GTK_LABEL(url_label), m_URLEntry);
	gtk_box_pack_start( GTK_BOX(url_bar), url_label, FALSE, FALSE, 4);
	gtk_box_pack_start( GTK_BOX(url_bar), m_URLEntry, TRUE, TRUE, 4);

	GtkToolItem* url_bar_item = gtk_tool_item_new();
	gtk_tool_item_set_expand(url_bar_item, true);
	gtk_container_add (GTK_CONTAINER (url_bar_item), url_bar);
	gtk_widget_show_all ( (GtkWidget*)url_bar_item);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), url_bar_item, -1);

	g_signal_connect ((gpointer) m_URLEntry, "key-press-event",
					G_CALLBACK (CMainFrame::OnURLEntryKeyDown),
					this);
	g_signal_connect ((gpointer) m_URLEntry, "focus-out-event",
					G_CALLBACK (CMainFrame::OnURLEntryKillFocus),
					this);

	m_Tooltips = tooltips;
}

void CMainFrame::OnNewCon(GtkMenuItem* mitem, CMainFrame* _this)
{
	CInputDialog* dlg = new CInputDialog( _this, _("Connect"), _("Host IP Address:\nAppend port number to IP with a separating colon if it's not 23."), NULL, true );
	if( dlg->ShowModal() == GTK_RESPONSE_OK && *dlg->GetText() )
	{
		_this->NewCon( dlg->GetText(), dlg->GetText(), &AppConfig.m_DefaultSite );
	}
	dlg->Destroy();
}

void CMainFrame::OnQuit(GtkMenuItem* mitem, CMainFrame* _this)
{
	if( _this->CanClose() )
	{
		_this->Hide();
		_this->Destroy();
	}
}


#include "pcmanx_xpm.xpm"
#include "pcmanx_inverse_xpm.xpm"
#include "conn_xpm.xpm"

void CMainFrame::LoadIcons()
{
	m_MainIcon = gdk_pixbuf_new_from_xpm_data((const char**)pcmanx_xpm);
	m_InverseMainIcon = gdk_pixbuf_new_from_xpm_data((const char**)pcmanx_inverse_xpm);
	m_ConnIcon = gdk_pixbuf_new_from_xpm_data((const char**)conn_xpm);
}


void CMainFrame::OnFont(GtkMenuItem* mitem, CMainFrame* _this)
{
	GtkWidget* dlg = gtk_font_selection_dialog_new(_("Font"));
	gtk_window_set_modal( (GtkWindow*)dlg, true);
	gtk_window_set_transient_for( (GtkWindow*)dlg, (GtkWindow*)_this->m_Widget);

	GtkFontSelectionDialog* fsdlg = (GtkFontSelectionDialog*)dlg;

	// This is not a good method because fontsel is a private member of GtkFontSelectionDialog.
	// But we need this functionality.
	GtkFontSelection* fontsel = GTK_FONT_SELECTION(fsdlg->fontsel);
	gtk_widget_set_sensitive(fontsel->face_list, false);

	char pango_font_name[32];
	sprintf( pango_font_name, "%s %d", AppConfig.FontFamily.c_str(), (AppConfig.FontSize > 6 && AppConfig.FontSize <= 72) ? AppConfig.FontSize : 12 );
	gtk_font_selection_dialog_set_font_name(fsdlg, pango_font_name);

	if( gtk_dialog_run((GtkDialog*)dlg) == GTK_RESPONSE_OK )
	{
		gchar* name = gtk_font_selection_dialog_get_font_name( fsdlg );
		gtk_widget_destroy(dlg);
		PangoFontDescription* desc = pango_font_description_from_string( name );
		g_free( name );
		const char* family = pango_font_description_get_family(desc);
		AppConfig.FontFamily = family;
		AppConfig.FontSize = pango_font_description_get_size(desc);
		pango_font_description_free(desc);

		vector<CTelnetView*>::iterator it;
		for( it = _this->m_Views.begin(); it != _this->m_Views.end(); ++it )
			(*it)->SetFontFamily(AppConfig.FontFamily);


		/// FIXME: Poor design! Different connection must be allowed to use different fonts in the future.

		if( _this->GetCurView() )
			_this->GetCurView()->Refresh();
	}
	else
		gtk_widget_destroy(dlg);
}


void CMainFrame::OnAbout(GtkMenuItem* mitem, CMainFrame* _this)
{
/*	GtkAboutDialog* dlg = (GtkAboutDialog*)gtk_about_dialog_new();
	gtk_about_dialog_set_name(dlg, "PCMan X");
	gtk_about_dialog_set_version(dlg, "0.1 pure GTK+ 2");
	gtk_about_dialog_set_copyright( dlg, _("Copy (C) 2005"));
	gtk_about_dialog_set_license(dlg, "GPL");
	gtk_about_dialog_set_website(dlg, "http://pcmanx.sf.net/");
	const char *authors[]={"Hong Jen Yee"};
	gtk_about_dialog_set_authors(dlg, authors );
	gtk_about_dialog_set_logo(dlg, _this->m_MainIcon);
*/
//	gtk_show_about_dialog( GTK_WINDOW(_this->m_Widget) );

	char* authors=_("Hong Jen Yee (Main developer) <hzysoft@sina.com.tw>\n"
					 "Jim Huang (Developer) <jserv@kaffe.org>\n"
					 "Kanru Chen (Developer) <koster@debian.org.tw>\n"
					 "Shih-yuan Lee (Developer) <fourdollars@gmail.com>"
					);
//	FILE* fp = fopen( AppConfig.GetDataPath("AUTHORS") );

	GtkWidget* dlg = gtk_message_dialog_new_with_markup( (GtkWindow*)_this->m_Widget,
						GTK_DIALOG_DESTROY_WITH_PARENT,
						GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
						_("<big>PCMan X  %s\nA free BBS client developed with GTK+ 2.x</big>\n\n"
						"Copyright (C) 2005\n"
						"License: GNU Genral Public License\n"
						"Project Homepage: http://pcmanx.csie.net/\n\n"
						"Authors:\n%s\n"), VERSION, authors );

// GTK+ supports this API since ver 2.6.
/*	gtk_message_dialog_format_secondary_text((GtkMessageDialog*)dlg,
						_("Copyright (C) 2005\n"
						"License: GNU Genral Public License\n"
						"Project Homepage: http://pcmanx.csie.net/\n\n"
						"Authors:\n%s\n")
						, authors	);
*/
	gtk_image_set_from_pixbuf((GtkImage*)((GtkMessageDialog*)dlg)->image, _this->m_MainIcon);
	gtk_dialog_run((GtkDialog*)dlg) == GTK_RESPONSE_OK;
	gtk_widget_destroy(dlg);

}


void CMainFrame::OnCloseCon(GtkMenuItem* mitem, CMainFrame* _this)
{
	CTelnetCon* con = _this->GetCurCon();
	if( !con )
		return;
	if( AppConfig.QueryOnCloseCon && !con->IsClosed() )
	{
		GtkWidget* dlg = gtk_message_dialog_new(GTK_WINDOW(_this->m_Widget), GTK_DIALOG_MODAL, GTK_MESSAGE_QUESTION, GTK_BUTTONS_OK_CANCEL, _("Close Connection?"));
		bool can_close = ( gtk_dialog_run(GTK_DIALOG(dlg)) == GTK_RESPONSE_OK );
		gtk_widget_destroy(dlg);
		if( !can_close )
			return;
	}

	GtkNotebook* nb = GTK_NOTEBOOK(_this->m_pNotebook->m_Widget);
	_this->CloseCon(gtk_notebook_get_current_page(nb), true);
}


void CMainFrame::OnCopy(GtkMenuItem* mitem, CMainFrame* _this)
{
	if( _this->GetCurView() )
	{
		_this->GetCurView()->CopyToClipboard(false, false, AppConfig.CopyTrimTail);
		if( AppConfig.CancelSelAfterCopy )
		{
			_this->GetCurCon()->m_SelStart = _this->GetCurCon()->m_SelEnd;
			_this->GetCurView()->Refresh();
		}
	}
}


void CMainFrame::OnCopyWithColor(GtkMenuItem* mitem, CMainFrame* _this)
{
	if( _this->GetCurView() )
	{
		_this->GetCurView()->CopyToClipboard(false, true, AppConfig.CopyTrimTail);
		if( AppConfig.CancelSelAfterCopy )
		{
			_this->GetCurCon()->m_SelStart = _this->GetCurCon()->m_SelEnd;
			_this->GetCurView()->Refresh();
		}
	}
}


void CMainFrame::OnNextCon(GtkMenuItem* mitem, CMainFrame* _this)
{
	int i = _this->GetNotebook()->GetCurPage() + 1;
	int n = _this->GetNotebook()->GetPageCount();
	_this->GetNotebook()->SetCurPage( i < n ? i : 0 );
}


void CMainFrame::OnPaste(GtkMenuItem* mitem, CMainFrame* _this)
{
	if(_this->GetCurView())
		_this->GetCurView()->PasteFromClipboard(false);
}


void CMainFrame::OnPreference(GtkMenuItem* mitem, CMainFrame* _this)
{
//	bool show_tray_icon = AppConfig.ShowTrayIcon;
	CPrefDlg* dlg = new CPrefDlg(_this);
	dlg->ShowModal();
	dlg->Destroy();

//	FIXME: Currently we cannot freely hide or show tray icon.
/*	if( AppConfig.ShowTrayIcon != show_tray_icon )
		if(AppConfig.ShowTrayIcon)
			_this->ShowTrayIcon();
		else
			_this->HideTrayIcon();
*/
}


void CMainFrame::OnPrevCon(GtkMenuItem* mitem, CMainFrame* _this)
{
	int i = _this->GetNotebook()->GetCurPage() - 1;
	int n = _this->GetNotebook()->GetPageCount();
	_this->GetNotebook()->SetCurPage( i >= 0 ? i : n-1 );

}

void CMainFrame::OnSiteList(GtkMenuItem* mitem, CMainFrame* _this)
{
	CSiteListDlg* dlg = new CSiteListDlg(_this);
	dlg->ShowModal();
	dlg->Destroy();
}


void CMainFrame::OnJumpToPage(GtkWidget* widget, CMainFrame* _this)
{
//	g_print("On jump to, widget=%x, _this->m_JumpMenuItems[0]=%x\n", widget, _this->m_JumpMenuItems[0]);
	for( int i = 0; i < 10; ++i )
		if( widget == _this->m_JumpMenuItems[i] )
		{
			_this->GetNotebook()->SetCurPage(i);
			break;
		}
}

void CMainFrame::OnTelnetConBell(CTelnetView* con)
{
	if( !IsActivated() )
		FlashWindow(true);

	if( AppConfig.BeepOnBell )
		gdk_display_beep(gdk_display_get_default());
	if( GetCurView() == con )
		return;
	string markup = "<span foreground=\"red\">";
	markup += con->GetCon()->m_Site.m_Name;
	markup += "</span>";
	m_pNotebook->SetPageTitle( con, markup.c_str());
}

void CMainFrame::OnTelnetConClose(CTelnetView* con)
{
	if( !con )
		return;
	string markup = "<span foreground=\"#808080\">";
	markup += con->GetCon()->m_Site.m_Name;
	markup += "</span>";
	m_pNotebook->SetPageTitle( con, markup.c_str());
}

void CMainFrame::OnTelnetConConnect(CTelnetView* con)
{
	if( !con )
		return;
	m_pNotebook->SetPageTitle( con, con->GetCon()->m_Site.m_Name.c_str() );
}


void CMainFrame::OnTelnetConRecv(CTelnetView* con)
{
	if( !con )
		return;

	// If the text color of the label is red which indicate 
	// there is new incoming message, we should return.
//	if(  )
//		return;
	string markup = "<span foreground=\"green\">";
	markup += con->GetCon()->m_Site.m_Name;
	markup += "</span>";
	m_pNotebook->SetPageTitle( con, markup.c_str());
}


void CMainFrame::OnNotebookChangeCurPage(GtkNotebook* widget, GtkNotebookPage* page,  gint page_num, CMainFrame* _this)
{
	_this->SetCurView( _this->m_Views[page_num] );
}

void CMainFrame::CloseCon(int idx, bool confirm)
{
	m_pNotebook->RemovePage(idx);
	m_Views.erase( m_Views.begin() + idx );

	int n = m_pNotebook->GetCurPage();
	SetCurView( n >= 0 ? m_Views[n] : NULL );
}

gboolean CMainFrame::OnBlinkTimer(CMainFrame* _this)
{
	if( _this->m_IsFlashing )
	{
		if( gtk_window_get_icon( GTK_WINDOW(_this->m_Widget) ) == _this->m_MainIcon )
			gtk_window_set_icon( GTK_WINDOW(_this->m_Widget), _this->m_InverseMainIcon );
		else
			gtk_window_set_icon( GTK_WINDOW(_this->m_Widget), _this->m_MainIcon );
	}

	if(_this->GetCurView() && _this->GetCurView()->IsVisible() )
		_this->GetCurView()->OnBlinkTimer();
	return true;
}


gboolean CMainFrame::OnClose( GtkWidget* widget, GdkEvent* evt, CMainFrame* _this )
{
	return !_this->CanClose();
}


void CMainFrame::OnDestroy()
{
	g_source_remove( m_BlinkTimer );
	g_source_remove( m_EverySecondTimer );

	gtk_object_destroy((GtkObject*)m_Tooltips);

	Hide();

	while( g_main_context_iteration(NULL, FALSE) );

	CTelnetCon::Cleanup();

	CWidget::OnDestroy();

	gtk_main_quit();
}


void CMainFrame::OnCreate()
{
	CWidget::OnCreate();
	LoadStartupSites();
}

bool CMainFrame::CanClose()
{
	if( GetNotebook()->GetPageCount() == 0 || !AppConfig.QueryOnExit )
		return true;

	GtkWidget* dlg = gtk_message_dialog_new( (GtkWindow*)m_Widget,
						GTK_DIALOG_DESTROY_WITH_PARENT,
						GTK_MESSAGE_QUESTION, GTK_BUTTONS_OK_CANCEL,
						_("Quit PCMan X ?"));
	bool close = gtk_dialog_run((GtkDialog*)dlg) == GTK_RESPONSE_OK;
	gtk_widget_destroy(dlg);
	return close;
}


void CMainFrame::NotImpl(const char* str)
{
	GtkWidget* dlg = gtk_message_dialog_new( (GtkWindow*)m_Widget,
					GTK_DIALOG_DESTROY_WITH_PARENT,
					GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
					_("Function not implemented yet!\nDebug: %s"), str);
	gtk_dialog_run((GtkDialog*)dlg);
	gtk_widget_destroy(dlg);
}


void CMainFrame::OnAddToFavorites(GtkMenuItem* widget, CMainFrame* _this)
{
	if(_this->m_Views.empty() )
		return;

	CSite &site = _this->GetCurCon()->m_Site;
	CSiteDlg* dlg = new CSiteDlg(_this, _("Add To Favorites"), site );
	if(dlg->ShowModal() == GTK_RESPONSE_OK)
	{
		AppConfig.Favorites.push_back(dlg->m_Site);
		CSite& newsite = AppConfig.Favorites.back();

		GtkWidget* fav_item = gtk_image_menu_item_new_with_label( site.m_Name.c_str() );
		gtk_widget_show (fav_item);
		gtk_menu_shell_insert( GTK_MENU_SHELL(_this->m_FavoritesMenu), fav_item, 
			AppConfig.Favorites.size()>0 ? (AppConfig.Favorites.size()-1) : 0 );

		GtkWidget* image = gtk_image_new_from_stock ("gtk-network", GTK_ICON_SIZE_MENU);

		gtk_widget_show (image);
		gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (fav_item), image);

		newsite.m_MenuItem = fav_item;

		g_signal_connect( G_OBJECT(fav_item), "activate",
						G_CALLBACK(CMainFrame::OnFavorite),
						_this);
	}
	dlg->Destroy();
}

void CMainFrame::OnEditFavorites(GtkMenuItem* widget, CMainFrame* _this)
{
	CEditFavDlg* dlg = new CEditFavDlg(_this, AppConfig.Favorites);
	dlg->ShowModal();
	AppConfig.Favorites.swap(dlg->m_Sites);
	dlg->Destroy();

	_this->CreateFavoritesMenu();
}


void CMainFrame::CreateFavoritesMenu()
{
	GtkWidget* favorites_menu = gtk_menu_new();
	vector<CSite>::iterator it;
	for( it = AppConfig.Favorites.begin(); it != AppConfig.Favorites.end(); ++it )
	{
		CSite& site = *it;

		GtkWidget* fav_item = gtk_image_menu_item_new_with_label( site.m_Name.c_str() );
		gtk_widget_show (fav_item);
		gtk_container_add (GTK_CONTAINER (favorites_menu), fav_item);

		GtkWidget* image = gtk_image_new_from_stock ("gtk-network", GTK_ICON_SIZE_MENU);
//		GtkWidget* image = gtk_image_new_from_pixbuf(m_ConnIcon);
		gtk_widget_show (image);
		gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (fav_item), image);

		site.m_MenuItem = fav_item;

		g_signal_connect( G_OBJECT(fav_item), "activate",
						G_CALLBACK(CMainFrame::OnFavorite),
						this);
	}

	GtkWidget* separator3 = gtk_separator_menu_item_new ();
	gtk_widget_show (separator3);
	gtk_container_add (GTK_CONTAINER (favorites_menu), separator3);
	gtk_widget_set_sensitive (separator3, FALSE);

	GtkWidget* add_to_fav_menu = gtk_image_menu_item_new_with_mnemonic (_("_Add to Favorites"));
	gtk_widget_show (add_to_fav_menu);
	gtk_container_add (GTK_CONTAINER (favorites_menu), add_to_fav_menu);

	GtkWidget* image347 = gtk_image_new_from_stock ("gtk-add", GTK_ICON_SIZE_MENU);
	gtk_widget_show (image347);
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (add_to_fav_menu), image347);

	GtkWidget* edit_fav_menu = gtk_image_menu_item_new_with_mnemonic (_("_Edit Favorites"));
	
	gtk_widget_show (edit_fav_menu);
	gtk_container_add (GTK_CONTAINER (favorites_menu), edit_fav_menu);

	GtkWidget* image348 = gtk_image_new_from_stock ("gtk-edit", GTK_ICON_SIZE_MENU);
	gtk_widget_show (image348);
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (edit_fav_menu), image348);


	g_signal_connect ((gpointer) add_to_fav_menu, "activate",
					G_CALLBACK (CMainFrame::OnAddToFavorites),
					this);
	g_signal_connect ((gpointer) edit_fav_menu, "activate",
					G_CALLBACK (CMainFrame::OnEditFavorites),
					this);

	m_FavoritesMenu = favorites_menu;
	gtk_menu_item_set_submenu( GTK_MENU_ITEM(m_FavoritesMenuItem), favorites_menu);
}


void CMainFrame::OnFavorite(GtkMenuItem* item, CMainFrame* _this)
{
	vector<CSite>::iterator it;
	for( it = AppConfig.Favorites.begin(); it != AppConfig.Favorites.end(); ++it )
	{
		CSite& site = *it;
		if( site.m_MenuItem == (GtkWidget*)item )
		{
			_this->NewCon( site.m_Name.c_str(), site.m_URL.c_str(), &site);
			break;
		}
	}
}

void CMainFrame::SetCurView(CTelnetView* view)
{
	m_pView = view;
	if( !m_pView || !m_pView->GetCon() )
	{
		gtk_window_set_title (GTK_WINDOW (m_Widget), "PCMan X "VERSION );
		gtk_entry_set_text( GTK_ENTRY(m_URLEntry), "");
		return;
	}

	CTelnetCon* con = GetCurCon();
	gtk_entry_set_text( GTK_ENTRY(m_URLEntry), con->m_Site.m_URL.c_str());

	string title = con->m_Site.m_Name;

	if( ! con->IsClosed() )
		m_pNotebook->SetPageTitle( m_pView, title.c_str() );

	title += " - PCMan X "VERSION;
	gtk_window_set_title (GTK_WINDOW (m_Widget), title.c_str() );
}

void CMainFrame::OnSelectAll(GtkMenuItem* mitem, CMainFrame* _this)
{
	CTelnetCon* con = _this->GetCurCon();
	if( con )
	{
		con->m_SelStart.x = 0;
		con->m_SelStart.y = 0;
		con->m_SelEnd.x = con->m_ColsPerPage;
		con->m_SelEnd.y = con->m_RowsPerPage-1;
		_this->GetCurView()->Refresh();
	}
}

void CMainFrame::LoadStartupSites()
{
	vector<CSite>::iterator it;
	for( it = AppConfig.Favorites.begin(); it != AppConfig.Favorites.end(); ++it )
	{
		CSite& site = *it;
		if( site.m_Startup )
			NewCon( site.m_Name.c_str(), site.m_URL.c_str(), &site);
	}
	m_pNotebook->SetCurPage(0);
}

gboolean CMainFrame::OnEverySecondTimer(CMainFrame* _this)
{
	vector<CTelnetView*>::iterator it;
	for( it = _this->m_Views.begin(); it != _this->m_Views.end(); ++it )
	{
		CTelnetView* view = *it;
		if( view->GetCon() )
			view->GetCon()->OnTimer();
	}
	return true;
}

void CMainFrame::OnEmoticons(GtkMenuItem* mitem, CMainFrame* _this)
{
	CEmoticonDlg* dlg = new CEmoticonDlg(_this);
	if( dlg->ShowModal() == GTK_RESPONSE_OK )
	{
		CTelnetView* view = _this->GetCurView();
		if( view )
			view->OnTextInput( dlg->GetSelectedStr().c_str() );
	}
	dlg->Destroy();
}

void CMainFrame::OnReconnect(GtkMenuItem* mitem, CMainFrame* _this)
{
	CTelnetCon* con = _this->GetCurCon();
	if( !con )
		return;
	if( con->IsClosed() )
		con->Reconnect();
	else
		_this->NewCon( con->m_Site.m_Name.c_str(), con->m_Site.m_URL.c_str(), &con->m_Site);
}

void CMainFrame::FlashWindow( bool flash )
{
	GdkPixbuf* icon = (m_IsFlashing = flash) ? m_InverseMainIcon : m_MainIcon;
	gtk_window_set_icon((GtkWindow*)m_Widget, icon);
}


gboolean CMainFrame::OnActivated( GtkWidget* widget, GdkEventFocus* evt, CMainFrame* _this )
{
	_this->m_IsActivated = true;
	if( _this->m_IsFlashing )
		_this->FlashWindow(false);
	return false;
}

gboolean CMainFrame::OnDeactivated( GtkWidget* widget, GdkEventFocus* evt, CMainFrame* _this )
{
	_this->m_IsActivated = false;
	return false;
}


gboolean CMainFrame::OnURLEntryKeyDown(GtkWidget *widget, GdkEventKey *evt, CMainFrame* _this)
{
	switch(evt->keyval)
	{
	case GDK_Return:
	{
		const gchar* url = gtk_entry_get_text( GTK_ENTRY(widget) );
		if( url && *url )
		{
			_this->NewCon( url, url, &AppConfig.m_DefaultSite );
			return true;
		}
		//	else goto case GDK_Escape
	}
	case GDK_Escape:
		if( _this->GetCurView() )
			_this->GetCurView()->SetFocus();
		return true;
	}
	return false;
}

void CMainFrame::OnURLEntryKillFocus(GtkWidget* entry, GdkEventFocus* evt, CMainFrame* _this)
{
	if( _this->GetCurView() )
	{
		gtk_entry_set_text( GTK_ENTRY(entry), _this->GetCurCon()->m_Site.m_URL.c_str());
		gtk_editable_select_region(GTK_EDITABLE(entry), 0, 0);
	}
}
