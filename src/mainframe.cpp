/* -*- coding: utf-8; indent-tabs-mode: t; tab-width: 4; c-basic-offset: 4; -*- */
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

#ifdef __GNUG__
  #pragma implementation "mainframe.h"
#endif


#include <glib/gi18n.h>
#include <gdk/gdkkeysyms.h>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <ltdl.h>

#include "mainframe.h"

#include "telnetview.h"
#include "notebook.h"
#include "telnetcon.h"
#include "editor.h"

#include "inputdialog.h"
#include "editfavdlg.h"
#include "sitedlg.h"
#include "prefdlg.h"
#include "appconfig.h"
#include "sitelistdlg.h"
#include "emoticondlg.h"
#include "downarticledlg.h"

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>

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

#ifdef USE_DOCKLET
#include "docklet/api.h"

const gchar COLOR_BLOCK[] = "\u2588\u2588\u2588\u2588\u2588\u2588";

void CMainFrame::OnTrayButton_Toggled(
	GtkToggleButton *button UNUSED,
	CMainFrame *_this)
{
	GtkToggleAction *action = (GtkToggleAction*) gtk_action_group_get_action(
				_this->m_ActionGroup, "showhide");
	gtk_toggle_action_set_active(action,
			! gtk_toggle_action_get_active(action));
}


void CMainFrame::OnShowHide(GtkToggleAction *toggleaction, CMainFrame *_this)
{
	bool show_tray_icon = AppConfig.ShowTrayIcon;

	if (show_tray_icon)
	{
		if (gtk_toggle_action_get_active(toggleaction))
			_this->Show();
		else
			_this->Hide();
	}
	else
	{
		gtk_window_iconify((GtkWindow*)_this->m_Widget);
	}
}
#endif

#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

void CMainFrame::OnToggleToolBar(GtkToggleAction *toggleaction, CMainFrame *_this)
{
	AppConfig.ShowToolbar = gtk_toggle_action_get_active(toggleaction);
	if (AppConfig.ShowToolbar) {
		gtk_widget_show_all((GtkWidget *)_this->m_Toolbar);
	} else {
		gtk_widget_hide_all((GtkWidget *)_this->m_Toolbar);
	}
}

void CMainFrame::OnToggleStatusBar(GtkToggleAction *toggleaction, CMainFrame *_this)
{
	AppConfig.ShowStatusBar = gtk_toggle_action_get_active(toggleaction);
	if (AppConfig.ShowStatusBar) {
		gtk_widget_show_all((GtkWidget *)_this->m_Statusbar);
	} else {
		gtk_widget_hide_all((GtkWidget *)_this->m_Statusbar);
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

#ifdef USE_DOCKLET
void CMainFrame::OnTray_Popup(GtkStatusIcon *status_icon UNUSED,
                              guint button, guint activate_time,
                              CMainFrame *_this)
{
	gtk_menu_popup((GtkMenu*)_this->m_TrayPopup, NULL, NULL, NULL, NULL
			, button, activate_time);
}
#endif

CMainFrame* CMainFrame::g_pMyself = NULL;

gboolean CMainFrame::OnSize( GtkWidget* widget, GdkEventConfigure* evt,
                             CMainFrame* _this UNUSED )
{
	gtk_window_get_position( GTK_WINDOW(widget), &AppConfig.MainWndX, & AppConfig.MainWndY );
	AppConfig.MainWndW = evt->width;
	AppConfig.MainWndH = evt->height;
	INFO("x=%d, y=%d, w=%d, h=%d", evt->x, evt->y, evt->width, evt->height );
	INFO("get_pos: x=%d, y=%d", AppConfig.MainWndX, AppConfig.MainWndY );
	return false;
}


CMainFrame::CMainFrame()
{
	char* desktop = getenv("XDG_CURRENT_DESKTOP");

	m_eView = NULL;
	m_pView = NULL;
	m_FavoritesMenuItem = NULL;
	m_FavoritesMenu = NULL;
	m_IsFlashing = false;
	m_Mode = NORMAL_MODE;
#ifdef USE_DOCKLET
	m_TrayIcon = NULL;
#endif

	/* Detecting Unity desktop environment */
	if (desktop != NULL && strcmp("Unity", desktop) == 0) {
		m_Unity = true;
	} else {
		m_Unity = false;
	}

	m_dlhandle = lt_dlopen("libappindicator.so.1");

  CTermView::Opacity = AppConfig.Opacity;

	m_Widget = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_wmclass(GTK_WINDOW(m_Widget), "pcmanx", "PCManX");

	PostCreate();

#if defined(USE_DOCKLET) || defined(USE_NOTIFIER)
	/* We need to make sure m_MainIcon is null at startup. */
	m_MainIcon = (GdkPixbuf *) NULL;
#endif

	LoadIcons();


	gtk_window_set_title (GTK_WINDOW (m_Widget), "PCManX "VERSION );
  widget_enable_rgba(m_Widget);

	m_pNotebook = new CNotebook();
	gtk_notebook_set_scrollable(GTK_NOTEBOOK(m_pNotebook->m_Widget), TRUE);
	g_signal_connect( G_OBJECT(m_pNotebook->m_Widget), "button_press_event",
			G_CALLBACK(CMainFrame::OnNotebookPopupMenu), this );

	MakeUI();


	gtk_window_set_icon((GtkWindow*)m_Widget, m_MainIcon);

	GtkWidget* vbox = gtk_vbox_new(false, 0);
  widget_enable_rgba(vbox);
  widget_enable_rgba(m_pNotebook->m_Widget);
	gtk_widget_show (vbox);

	//GtkWidget* m_Statusbar = gtk_statusbar_new ();
	m_Statusbar = gtk_statusbar_new ();

	gtk_container_add(GTK_CONTAINER(m_Widget), vbox);

	gtk_box_pack_start (GTK_BOX (vbox), m_Menubar, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (vbox), m_Toolbar, FALSE, FALSE, 0);
	if (AppConfig.ShowToolbar) {
		gtk_widget_show_all(m_Toolbar);
	} else {
		gtk_widget_hide_all(m_Toolbar);
	}
	gtk_box_pack_start (GTK_BOX (vbox), m_pNotebook->m_Widget, TRUE, TRUE, 0);
	gtk_widget_set_size_request(m_pNotebook->m_Widget, 300, 200);
	gtk_box_pack_start (GTK_BOX (vbox), m_Statusbar, FALSE, FALSE, 0);

	// Tab key to quick switch keyboard focus to m_URLEntry
	GList *focus_chain = NULL;
	focus_chain = g_list_append(focus_chain, m_Toolbar);
	focus_chain = g_list_append(focus_chain, m_URLEntry);
	gtk_container_set_focus_chain(GTK_CONTAINER(vbox), focus_chain);
	g_list_free(focus_chain);


//	gtk_widget_grab_focus(m_pNotebook->m_Widget);

//	GTK_WIDGET_UNSET_FLAGS(m_pNotebook->m_Widget, GTK_CAN_FOCUS);
//
	g_signal_connect(G_OBJECT(m_Widget), "window-state-event", G_CALLBACK(CMainFrame::OnWindowStateEvent), this);

	g_signal_connect(G_OBJECT(m_Widget), "delete-event", G_CALLBACK(CMainFrame::OnClose), this);

	g_signal_connect(m_pNotebook->m_Widget, "switch-page", G_CALLBACK(CMainFrame::OnNotebookChangeCurPage), this);

	g_signal_connect(m_Widget, "configure-event", G_CALLBACK(CMainFrame::OnSize), this);

	g_signal_connect(G_OBJECT(m_Widget), "focus-in-event", G_CALLBACK(CMainFrame::OnActivated), this);

//	g_signal_connect(G_OBJECT(m_Widget), "focus-out-event", G_CALLBACK(CMainFrame::OnDeactivated), this);

	gtk_box_set_spacing( GTK_BOX (m_Statusbar), 4 );
#ifdef USE_NANCY
	m_StatusBarBotState = (GtkLabel*)gtk_label_new("");
	gtk_box_pack_start (GTK_BOX (m_Statusbar), (GtkWidget*)m_StatusBarBotState, FALSE, FALSE, 2);
	GtkWidget* vsep = gtk_vseparator_new ();
 	gtk_box_pack_start (GTK_BOX (m_Statusbar), vsep, FALSE, FALSE, 2);
#endif
	m_StatusBarTime = (GtkLabel*)gtk_label_new("");
	gtk_box_pack_start (GTK_BOX (m_Statusbar), (GtkWidget*)m_StatusBarTime, FALSE, FALSE, 2);
	if (AppConfig.ShowStatusBar) {
		gtk_widget_show_all(m_Statusbar);
	} else {
		gtk_widget_hide_all(m_Statusbar);
	}

	m_BlinkTimer = g_timeout_add(600, (GSourceFunc)CMainFrame::OnBlinkTimer, this );
	m_EverySecondTimer = g_timeout_add(1000, (GSourceFunc)CMainFrame::OnEverySecondTimer, this );

	CTelnetView::SetParentFrame(this);
	CTelnetView::SetWebBrowser(AppConfig.WebBrowser);
	CTelnetView::SetMailClient(AppConfig.MailClient);

	if (AppConfig.Maximized) {
		gtk_window_maximize((GtkWindow*) m_Widget);
	}else{
		gtk_window_unmaximize((GtkWindow*) m_Widget);
	}

	if(AppConfig.ShowInSimpleMode){
		gtk_toggle_action_set_active(GTK_TOGGLE_ACTION(gtk_action_group_get_action(m_ActionGroup, "simple")), true);
	}
}


CTelnetCon* CMainFrame::NewCon(string title, string url, CSite* site )
{
	/* Remove leading and trailing spaces from url. */
	size_t first = url.find_first_not_of(" \t");
	size_t last = url.find_last_not_of(" \t");
	if (last >= first)
		url = url.substr(first, last - first + 1);

	if ( site == NULL )
		site = &AppConfig.m_DefaultSite;

	CTelnetCon* pCon;
	CEditor* pEditor;

	/**
	*   Since CEditor is extended from CTelnetCon and CEditorView extended from CTelnetView,
	*   there are lots of settings can be reused below.
	*   Therefore, the differences only lie in the new instance part.
	*   We use the magic url "ansi_editor" to identify if the Editor instance should be created or not.
	*/
	if(url == "ansi_editor"){
		m_eView = new CEditorView;
		pEditor = new CEditor( m_eView, *site);
		pCon = pEditor;
		m_pView = m_eView;
	}else{
		m_pView = new CTelnetView;
		pCon = new CTelnetCon( m_pView, *site );
		pEditor = NULL;
	}
	m_Views.push_back(m_pView);

	m_pView->m_pTermData = pCon;
	m_pView->SetContextMenu(m_EditMenu);
	m_pView->SetFont(AppConfig.FontFamily, AppConfig.FontSize, AppConfig.CompactLayout, AppConfig.AntiAliasFont, CTermView::FONT_DEFAULT);
	m_pView->SetFont(AppConfig.FontFamilyEn, AppConfig.FontSizeEn, AppConfig.CompactLayout, AppConfig.AntiAliasFont, CTermView::FONT_EN);
	m_pView->SetHyperLinkColor( &AppConfig.HyperLinkColor );
	m_pView->SetHorizontalCenterAlign( site->m_bHorizontalCenterAlign );
	m_pView->SetVerticalCenterAlign( site->m_bVerticalCenterAlign );
	m_pView->m_CharPaddingX = AppConfig.CharPaddingX;
	m_pView->m_CharPaddingY = AppConfig.CharPaddingY;

	pCon->m_Site.m_Name = title;
	pCon->m_Site.m_URL = url;
	pCon->m_Encoding = pCon->m_Site.m_Encoding;

	m_pView->SetUAO( site->m_UAO );

	pCon->AllocScreenBuf( site->m_RowsPerPage, site->m_RowsPerPage, site->m_ColsPerPage );

	int idx = m_pNotebook->AddPage( m_pView, title, true );
	m_pNotebook->SetCurPage(idx);
	m_pView->SetFocus();

	if(url == "ansi_editor"){
		pEditor->EditorActions(CEditor::Init_Ansi_Editor);
	}else{
		pCon->Connect();
	}

	return pCon;
}


#ifdef USE_NANCY
GtkRadioActionEntry CMainFrame::cur_bot_entries[] =
  {
    {"disable_cur_bot", NULL, _("Disable Bot"), NULL, NULL, 0},
    {"nancy_bot_current", NULL, _("Nancy Bot"), NULL, NULL, 1}
  };

GtkRadioActionEntry CMainFrame::all_bot_entries[] =
  {
    {"disable_all_bot", NULL, _("Disable Bot"), NULL, NULL, 0},
    {"nancy_bot_all", NULL, _("Nancy Bot"), NULL, NULL, 1}
  };
#endif

static const char *ui_info =
  "<ui>"
  "  <menubar>"
  "    <menu action='connect_menu'>"
  "      <menuitem action='site_list'/>"
  "      <menuitem action='new_con'/>"
  "      <menuitem action='reconnect'/>"
  "      <menuitem action='close'/>"
  "      <separator/>"
  "      <menuitem action='next_con'/>"
  "      <menuitem action='previous_con'/>"
  " <menuitem action='first_con'/>"
  " <menuitem action='last_con'/>"
  "      <menuitem action='jump'/>"
  "      <separator/>"
  "      <menuitem action='quit'/>"
  "    </menu>"
  "    <menu action='edit_menu'>"
  "      <menuitem action='copy'/>"
  "      <menuitem action='copy_with_ansi'/>"
  "      <menuitem action='paste'/>"
  "      <menuitem action='paste_from_clipboard'/>"
  "      <menuitem action='select_all'/>"
  "      <menuitem action='down_article'/>"
  "      <separator/>"
  "      <menuitem action='emoticon'/>"
  "      <menuitem action='preference'/>"
  "    </menu>"
  "    <menu action='favorites_menu'>"
  "      <separator/>"
  "      <menuitem action='add_to_fav'/>"
  "      <menuitem action='edit_fav'/>"
  "    </menu>"
  "    <menu action='view_menu'>"
  "      <menuitem action='ascii_font'/>"
  "      <menuitem action='non_ascii_font'/>"
  "      <separator/>"
  "      <menuitem action='toolbar'/>"
  "      <menuitem action='statusbar'/>"
  "      <separator/>"
#ifdef USE_DOCKLET
  "      <menuitem action='showhide'/>"
#endif
  "      <menuitem action='fullscreen' />"
  "      <menuitem action='simple' />"
#ifdef USE_NANCY
  "      <separator/>"
  "      <menu action='cur_bot_menu'>"
  "        <menuitem action='disable_cur_bot'/>"
  "        <menuitem action='nancy_bot_current'/>"
  "      </menu>"
  "      <menu action='all_bot_menu'>"
  "        <menuitem action='disable_all_bot'/>"
  "        <menuitem action='nancy_bot_all'/>"
  "      </menu>"
#endif
  "    </menu>"
  "    <menu action='menu_ansi_editor'>"
  "      <menuitem action='openAnsiEditor'/>"
  "      <separator/>"
  "      <menuitem action='openAnsiFile'/>"
  "      <menuitem action='saveAnsiFile'/>"
  "      <menuitem action='clearScreen'/>"
  "    </menu>"
  "    <menu action='help_menu'>"
  "      <menuitem action='shortcut_list'/>"
  "      <menuitem action='about'/>"
  "    </menu>"
  "  </menubar>"
  "  <toolbar>"
  "    <separator/>"
  "    <toolitem action='site_list'/>"
  "    <toolitem action='new_con'/>"
  "    <toolitem action='reconnect'/>"
  "    <toolitem action='close'/>"
  "    <separator/>"
  "    <toolitem action='copy'/>"
  "    <toolitem action='copy_with_ansi'/>"
  "    <toolitem action='paste'/>"
  "    <toolitem action='down_article'/>"
  "    <separator/>"
  "    <toolitem action='add_to_fav'/>"
  "    <toolitem action='preference'/>"
  "    <toolitem action='about'/>"
  "    <separator/>"
  "  </toolbar>"
  "  <popup name='edit_popup'>"
  "    <menuitem action='copy'/>"
  "    <menuitem action='copy_with_ansi'/>"
  "    <menuitem action='paste'/>"
  "    <menuitem action='paste_from_clipboard'/>"
  "    <menuitem action='select_all'/>"
  "    <separator/>"
  "    <menuitem action='fullscreen' />"
  "    <menuitem action='simple' />"
  "    <separator/>"
  "  </popup>"
#if defined(USE_DOCKLET)
  "  <popup name='tray_popup'>"
  "    <menuitem action='showhide' />"
  "    <separator />"
  "    <menuitem action='quit'/>"
  "  </popup>"
  " <accelerator action='showhide' />"
#endif

  // alternative accelerators
  " <accelerator action='close2'/>"
  " <accelerator action='reconnect1'/>"
  " <accelerator action='next_con1'/>"
  " <accelerator action='previous_con1'/>"
  " <accelerator action='new_con_gnome_term_sty'/>"
  " <accelerator action='next_con_gnome_term_sty'/>"
  " <accelerator action='previous_con_gnome_term_sty'/>"
  " <accelerator action='copy_gnome_term_sty'/>"
  " <accelerator action='paste_gnome_term_sty'/>"
  "</ui>";

void CMainFrame::MakeUI()
{
	m_ActionGroup = gtk_action_group_new("GlobalActions");

	gtk_action_group_set_translation_domain(m_ActionGroup, GETTEXT_PACKAGE);


	//move m_ActionEntries from class member to local.
	GtkActionEntry actionEntries[] =
	  {
		{"connect_menu", NULL, _("_Connect"), NULL, NULL, NULL},
		{"site_list", GTK_STOCK_OPEN, _("_Site List"), AppConfig.keySiteList.data(), _("Site List"), G_CALLBACK (CMainFrame::OnSiteList)},
		{"new_con", GTK_STOCK_NETWORK, _("_New Connection"), AppConfig.keyNewConn0.c_str(), _("New Connection"), G_CALLBACK (CMainFrame::OnNewCon)},
		{"new_con_gnome_term_sty", GTK_STOCK_NETWORK, _("_New Connection"), AppConfig.keyNewConn1.data(), _("New Connection"), G_CALLBACK (CMainFrame::OnNewCon)},
		{"reconnect", GTK_STOCK_UNDO, _("_Reconnect"), AppConfig.keyReconn0.data(), _("Reconnect"), G_CALLBACK (CMainFrame::OnReconnect)},
		{"reconnect1",GTK_STOCK_UNDO, _("_Reconnect"), AppConfig.keyReconn1.data(), _("Reconnect"), G_CALLBACK(CMainFrame::OnReconnect)},
		{"close", GTK_STOCK_CLOSE, _("_Close Connection"), AppConfig.keyClose0.data(), _("Close Connection"), G_CALLBACK (CMainFrame::OnCloseCon)},
		{"close2", GTK_STOCK_CLOSE, _("_Close Connection"), AppConfig.keyClose1.data(), _("Close Connection"), G_CALLBACK(CMainFrame::OnCloseCon)},
		{"next_con", GTK_STOCK_GO_DOWN, _("Ne_xt Page"), AppConfig.keyNextPage.data(), _("Next Page"), G_CALLBACK (CMainFrame::OnNextCon)},
		{"next_con1", GTK_STOCK_GO_DOWN, _("Ne_xt Page"), "<Ctrl>Right", _("Next Page"), G_CALLBACK(CMainFrame::OnNextCon)},
		{"next_con_gnome_term_sty", GTK_STOCK_GO_DOWN, _("Ne_xt Page"), "<Ctrl>Page_Down", _("Next Page"), G_CALLBACK(CMainFrame::OnNextCon)},
		{"previous_con", GTK_STOCK_GO_UP, _("_Previous Page"), AppConfig.keyPrevPage.data(), _("Previous Page"), G_CALLBACK (CMainFrame::OnPrevCon)},
		{"previous_con1", GTK_STOCK_GO_UP, _("_Previous Page"), "<Ctrl>Left", _("Previous Page"), G_CALLBACK(CMainFrame::OnPrevCon)},
		{"previous_con_gnome_term_sty", GTK_STOCK_GO_UP, _("_Previous Page"), "<Ctrl>Page_Up", _("Previous Page"), G_CALLBACK(CMainFrame::OnPrevCon)},
		{"first_con",GTK_STOCK_GO_UP, _("_First Page"), AppConfig.keyFirstPage.data(), _("First Page"), G_CALLBACK(CMainFrame::OnFirstCon)},
		{"last_con", GTK_STOCK_GO_DOWN, _("_Last Page"), AppConfig.keyLastPage.data(), _("Last Page"), G_CALLBACK(CMainFrame::OnLastCon)},
		{"jump", GTK_STOCK_JUMP_TO, _("_Jump to"), NULL, NULL, NULL},
		{"quit", GTK_STOCK_QUIT, _("_Quit"), "", _("Quit"), G_CALLBACK (CMainFrame::OnQuit)},
		{"edit_menu", NULL, _("_Edit"), NULL, NULL, NULL},
		{"copy", GTK_STOCK_COPY, _("_Copy"), AppConfig.keyCopy0.data(), _("Copy"), G_CALLBACK (CMainFrame::OnCopy)},
		{"copy_gnome_term_sty", GTK_STOCK_COPY, _("_Copy"), AppConfig.keyCopy1.data(), _("Copy"), G_CALLBACK (CMainFrame::OnCopy)},
		{"copy_with_ansi", GTK_STOCK_SELECT_COLOR, _("Copy with A_NSI Color"), NULL, _("Copy with ANSI Color"), G_CALLBACK (CMainFrame::OnCopyWithColor)},
		{"paste", GTK_STOCK_PASTE, _("_Paste"), AppConfig.keyPaste0.data(), _("Paste"), G_CALLBACK (CMainFrame::OnPaste)},
		{"paste_gnome_term_sty", GTK_STOCK_PASTE, _("_Paste"), AppConfig.keyPaste1.data(), _("Paste"), G_CALLBACK (CMainFrame::OnPaste)},
		{"paste_from_clipboard", GTK_STOCK_PASTE, _("Paste from Clipboard"), AppConfig.keyPasteClipboard.data(), NULL, G_CALLBACK (CMainFrame::pasteFromClipboard)},
		{"down_article", GTK_STOCK_SELECT_ALL, _("_Download Article"), NULL, _("Download Article"), G_CALLBACK (CMainFrame::OnDownArticle)},
		{"select_all", NULL, _("Select A_ll"), NULL, NULL, G_CALLBACK (CMainFrame::OnSelectAll)},
		{"emoticon", NULL, _("_Emoticons"), AppConfig.keyEmotions.data(), NULL, G_CALLBACK (CMainFrame::OnEmoticons)},
		{"preference", GTK_STOCK_PREFERENCES, _("_Preference"), NULL, _("Preference"), G_CALLBACK (CMainFrame::OnPreference)},
		{"favorites_menu", NULL, _("F_avorites"), NULL, NULL, NULL},
		{"add_to_fav", GTK_STOCK_ADD, _("_Add to Favorites"), NULL, _("Add to Favorites"), G_CALLBACK (CMainFrame::OnAddToFavorites)},
		{"edit_fav", GTK_STOCK_EDIT, _("_Edit Favorites"), NULL, NULL, G_CALLBACK (CMainFrame::OnEditFavorites)},
		{"view_menu", NULL, _("_View"), NULL, NULL, NULL},
		{"ascii_font", GTK_STOCK_SELECT_FONT, _("_ASCII Font"), NULL, NULL, G_CALLBACK (CMainFrame::OnFont)},
		{"non_ascii_font", GTK_STOCK_SELECT_FONT,  _("Non-ASCII _Font"), NULL, NULL, G_CALLBACK (CMainFrame::OnFont)},
	#ifdef USE_NANCY
		{"cur_bot_menu", GTK_STOCK_EXECUTE, _("Bot (Current Connection)"), NULL, NULL, NULL},
		{"all_bot_menu", GTK_STOCK_EXECUTE, _("Bot (All Opened Connections)"), NULL, NULL, NULL},
	#endif
		{"help_menu", NULL, _("_Help"), NULL, NULL, NULL},
		{"shortcut_list", GTK_STOCK_DIALOG_INFO,  _("_Shortcut List"), NULL, NULL, G_CALLBACK (CMainFrame::OnShortcutList)},
		{"about", GTK_STOCK_ABOUT, NULL, NULL, _("About"), G_CALLBACK (CMainFrame::OnAbout)},
		// Ansi Editor Menu
		{"menu_ansi_editor", NULL, _("Ansi Editor"), NULL, NULL, NULL},
		{"openAnsiEditor", NULL, _("Open Ansi Editor"), NULL, NULL, G_CALLBACK (CMainFrame::OnAnsiEditor)},
		{"openAnsiFile", NULL, _("Open Ansi File"), NULL, NULL, G_CALLBACK (CMainFrame::OnOpenAnsiFile)},
		{"saveAnsiFile", NULL, _("Save Ansi File"), NULL, NULL, G_CALLBACK (CMainFrame::OnSaveAnsiFile)},
		{"clearScreen", NULL, _("Clear Screen"), NULL, NULL, G_CALLBACK (CMainFrame::OnClearScreen)}
	  };

		//move m_ToggleActionEntries from class member to local
		GtkToggleActionEntry ToggleActionEntries[] =
		{
	#ifdef USE_DOCKLET
		// Show/Hide Main Window
		{"showhide", NULL, _("Show _Main Window"), "<Alt>M", NULL, G_CALLBACK(CMainFrame::OnShowHide), true},
	#endif
		{"toolbar", NULL, _("Show Toolbar"), NULL, NULL, G_CALLBACK (CMainFrame::OnToggleToolBar), true},
		{"statusbar", NULL, _("Show Status Bar on bottom"), NULL, NULL, G_CALLBACK (CMainFrame::OnToggleStatusBar), true},
		{"fullscreen", NULL, _("F_ullscreen Mode"), AppConfig.keyFullscreen.data(), NULL, G_CALLBACK (CMainFrame::OnFullscreenMode), false},
		{"simple", NULL, _("_Simple Mode"), AppConfig.keySimpleMode.data(), NULL, G_CALLBACK (CMainFrame::OnSimpleMode), false},
		};

	gtk_action_group_add_actions(m_ActionGroup, actionEntries, G_N_ELEMENTS(actionEntries), this);

	gtk_action_group_add_toggle_actions(m_ActionGroup, ToggleActionEntries,
			G_N_ELEMENTS(ToggleActionEntries), this);

#ifdef USE_NANCY
	gtk_action_group_add_radio_actions(m_ActionGroup,
			cur_bot_entries,
			G_N_ELEMENTS(cur_bot_entries),
			0,
			G_CALLBACK (CMainFrame::OnChangeCurrentBot),
			this);
	gtk_action_group_add_radio_actions(m_ActionGroup,
			all_bot_entries,
			G_N_ELEMENTS(all_bot_entries),
			0,
			G_CALLBACK (CMainFrame::OnChangeAllBot),
			this);
#endif

	m_UIManager = gtk_ui_manager_new();
	gtk_ui_manager_insert_action_group(m_UIManager, m_ActionGroup, 0);

	GtkAccelGroup* accel_group = gtk_ui_manager_get_accel_group ( m_UIManager );
	gtk_window_add_accel_group (GTK_WINDOW (m_Widget), accel_group);

	GError * error = NULL;
	if (!gtk_ui_manager_add_ui_from_string(m_UIManager, ui_info, -1, & error))
	{
		g_message("Building menu failed : %s", error->message);
		g_error_free(error); exit(EXIT_FAILURE);
	}

	m_Menubar = gtk_ui_manager_get_widget (m_UIManager, "/ui/menubar");
	m_Toolbar = gtk_ui_manager_get_widget (m_UIManager, "/ui/toolbar");
	gtk_toolbar_set_style( (GtkToolbar*)m_Toolbar, GTK_TOOLBAR_ICONS );

	m_EditMenu = gtk_ui_manager_get_widget (m_UIManager, "/ui/edit_popup");

	m_FavoritesMenuItem = gtk_ui_manager_get_widget (m_UIManager, "/ui/menubar/favorites_menu");

#ifdef USE_NANCY

	m_DisableCurBotRadio = (GtkRadioMenuItem*) gtk_ui_manager_get_widget (m_UIManager,
			"/ui/menubar/view_menu/cur_bot_menu/disable_cur_bot");
	m_CurBotNancyRadio = (GtkRadioMenuItem*) gtk_ui_manager_get_widget (m_UIManager,
			"/ui/menubar/view_menu/cur_bot_menu/nancy_bot_current");

	m_DisableAllBotRadio = (GtkRadioMenuItem*) gtk_ui_manager_get_widget (m_UIManager,
			"/ui/menubar/view_menu/all_bot_menu/disable_all_bot");
	m_AllBotNancyRadio = (GtkRadioMenuItem*) (gtk_ui_manager_get_widget (m_UIManager,
				"/ui/menubar/view_menu/all_bot_menu/nancy_bot_all"));

#endif

	GtkWidget* jump = gtk_ui_manager_get_widget (m_UIManager, "/ui/menubar/connect_menu/jump");

	GtkWidget* jump_menu = gtk_menu_new ();
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (jump), jump_menu);

	const char* page_str = _("Page");
	for(int i = 1; i < 11; i++)
	{
		char title[32], name[32];
		sprintf(title, "%s %d_%d", page_str, i / 10, i % 10);
		sprintf(name, "jumpto_%d", i);
		GtkAction *action = gtk_action_new(name, title, NULL, NULL);
		gtk_action_set_accel_group(action, accel_group);
		g_signal_connect( G_OBJECT(action), "activate",
				G_CALLBACK (CMainFrame::OnJumpToPage),
				this);
		sprintf(name, "<Alt>%d", i % 10);
		gtk_action_group_add_action_with_accel(m_ActionGroup, action, name);
		gtk_container_add (GTK_CONTAINER (jump_menu),
				gtk_action_create_menu_item(action));
		m_JumpTos[i-1] = G_OBJECT(action);
	}

	// Ansi Editor widget: blink check box, click to set the text blink.
	m_chkBlink = gtk_check_button_new_with_label("blink");
	GtkToolItem *itemBlink = gtk_tool_item_new();
	gtk_container_add(GTK_CONTAINER(itemBlink), m_chkBlink);
	gtk_widget_show_all ( (GtkWidget*)itemBlink);
	gtk_toolbar_insert(GTK_TOOLBAR(m_Toolbar), itemBlink, -1);

	// Ansi Editor widget: Set Text color.
	GtkTreeModel *model;
	model = GTK_TREE_MODEL(gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_STRING));
	m_cbTextColor = gtk_combo_box_new_with_model(model);

	GtkCellRenderer *cell;
	cell = gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(m_cbTextColor), cell, TRUE);
	gtk_cell_layout_add_attribute(GTK_CELL_LAYOUT(m_cbTextColor), cell, "text", 0);
	gtk_cell_layout_add_attribute(GTK_CELL_LAYOUT(m_cbTextColor), cell, "foreground", 1);

	GtkTreeIter iter;
	GtkListStore *store = GTK_LIST_STORE(model);
	gtk_list_store_append(store, &iter);
	gtk_list_store_set(store, &iter, 0, "Front", 1, "#000000", -1);

	for (int i = 0; i < 16; i++)
	{
		gchar color[8];
		ParseColor(&(CTermCharAttr::m_DefaultColorTable[i]), color, ARRAY_SIZE(color));
		AppendRow(&iter, store, COLOR_BLOCK, color);
	}
	gtk_combo_box_set_active(GTK_COMBO_BOX(m_cbTextColor), 0);

	GtkToolItem *itemTextColor = gtk_tool_item_new();
	gtk_container_add(GTK_CONTAINER(itemTextColor), m_cbTextColor);
	gtk_widget_show_all ( (GtkWidget*)itemTextColor);
	gtk_toolbar_insert(GTK_TOOLBAR(m_Toolbar), itemTextColor, -1);

	// Ansi Editor widget: Set Background color.
	GtkTreeModel *model_back;
	model_back = GTK_TREE_MODEL(gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_STRING));
	m_cbBgColor = gtk_combo_box_new_with_model(model_back);

	GtkCellRenderer *cell_back;
	cell_back = gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(m_cbBgColor), cell_back, TRUE);
	gtk_cell_layout_add_attribute(GTK_CELL_LAYOUT(m_cbBgColor), cell_back, "text", 0);
	gtk_cell_layout_add_attribute(GTK_CELL_LAYOUT(m_cbBgColor), cell_back, "foreground", 1);

	GtkTreeIter iter_back;
	GtkListStore *store_back = GTK_LIST_STORE(model_back);
	gtk_list_store_append(store_back, &iter_back);
	gtk_list_store_set(store_back, &iter_back, 0, "Background", 1, "#000000", -1);

	for (int i = 0; i < 8; i++)
	{
		gchar color[8];
		ParseColor(&(CTermCharAttr::m_DefaultColorTable[i]), color, ARRAY_SIZE(color));
		AppendRow(&iter_back, store_back, COLOR_BLOCK, color);
	}
	gtk_combo_box_set_active(GTK_COMBO_BOX(m_cbBgColor), 0);

	GtkToolItem *itemBgColor = gtk_tool_item_new();
	gtk_container_add(GTK_CONTAINER(itemBgColor), m_cbBgColor);
	gtk_widget_show_all ( (GtkWidget*)itemBgColor);
	gtk_toolbar_insert(GTK_TOOLBAR(m_Toolbar), itemBgColor, -1);

	GtkWidget* sep = (GtkWidget*)gtk_separator_tool_item_new();
	gtk_widget_show(sep);
	gtk_container_add (GTK_CONTAINER (m_Toolbar), sep);
	// Create the URL address bar
	GtkWidget* url_bar = gtk_hbox_new (FALSE, 0);
	GtkWidget* url_label = (GtkWidget*) gtk_label_new_with_mnemonic(_("A_ddress:"));
	m_URLEntry = (GtkWidget*) gtk_entry_new();
	gtk_widget_set_size_request(m_URLEntry, 0, -1);
	GtkTooltips* tooltips = gtk_tooltips_new();
	gtk_tooltips_set_tip(tooltips, m_URLEntry, _("Type URL here, then hit \"Enter\""), NULL);
	gtk_label_set_mnemonic_widget(GTK_LABEL(url_label), m_URLEntry);
	gtk_box_pack_start( GTK_BOX(url_bar), url_label, FALSE, FALSE, 4);
	gtk_box_pack_start( GTK_BOX(url_bar), m_URLEntry, TRUE, TRUE, 4);

	GtkToolItem* url_bar_item = gtk_tool_item_new();
	gtk_tool_item_set_expand(url_bar_item, true);
	gtk_container_add (GTK_CONTAINER (url_bar_item), url_bar);
	gtk_widget_show_all ( (GtkWidget*)url_bar_item);
	gtk_toolbar_insert(GTK_TOOLBAR(m_Toolbar), url_bar_item, -1);

	g_signal_connect ((gpointer) m_URLEntry, "key-press-event",
			G_CALLBACK (CMainFrame::OnURLEntryKeyDown),
			this);
	g_signal_connect ((gpointer) m_URLEntry, "focus-out-event",
			G_CALLBACK (CMainFrame::OnURLEntryKillFocus),
			this);

	gtk_toggle_action_set_active(GTK_TOGGLE_ACTION(gtk_action_group_get_action(m_ActionGroup, "toolbar")), AppConfig.ShowToolbar);
	gtk_toggle_action_set_active(GTK_TOGGLE_ACTION(gtk_action_group_get_action(m_ActionGroup, "statusbar")), AppConfig.ShowStatusBar);

	// Ansi Editor widget events
	g_signal_connect(GTK_OBJECT(m_cbTextColor), "changed", G_CALLBACK(SetTextColor), this);
	g_signal_connect(GTK_OBJECT(m_cbBgColor), "changed", G_CALLBACK(SetBgColor), this);
	g_signal_connect(GTK_OBJECT(m_chkBlink), "toggled", G_CALLBACK(SetBlink), this);

	CreateFavoritesMenu();
	CreateTrayIcon();
}

void CMainFrame::OnNewCon(GtkMenuItem* mitem UNUSED, CMainFrame* _this)
{
	CInputDialog* dlg = new CInputDialog( _this, _("Connect"), _("Host IP Address:\nAppend port number to IP with a separating colon if it's not 23."), NULL, true );
	if( dlg->ShowModal() == GTK_RESPONSE_OK && !dlg->GetText().empty() )
	{
		_this->NewCon( dlg->GetText(), dlg->GetText() );
	}
	dlg->Destroy();
}

void CMainFrame::OnQuit(GtkMenuItem* mitem UNUSED, CMainFrame* _this)
{
	if( _this->CanClose() )
	{
		_this->Hide();
		_this->Destroy();
	}
}

static void inverse_pixbuf(GdkPixbuf* pixbuf)
{
	int x = 0, y = 0;
	int channels = gdk_pixbuf_get_n_channels(pixbuf);
	int rowstride = gdk_pixbuf_get_rowstride(pixbuf);
	int width = gdk_pixbuf_get_width(pixbuf);
	int height = gdk_pixbuf_get_height(pixbuf);
	guchar *pixels = gdk_pixbuf_get_pixels(pixbuf);
	for (x = 0; x < width; x++) {
		for (y = 0; y < height; y++) {
			guchar *ptr = pixels + y * rowstride + x * channels;
			ptr[0] ^= 0xFF; //red
			ptr[1] ^= 0xFF; //green
			ptr[2] ^= 0xFF; //blue
		}
	}
	return;
}

void CMainFrame::LoadIcons()
{
	GtkImage* image = GTK_IMAGE(gtk_image_new_from_file(DATADIR "/pixmaps/pcmanx.svg"));
	GdkPixbuf* icon = gtk_image_get_pixbuf(image);
	m_MainIcon = gdk_pixbuf_scale_simple(icon, 32, 32, GDK_INTERP_BILINEAR);
	m_InverseMainIcon = gdk_pixbuf_copy(m_MainIcon);
	inverse_pixbuf(m_InverseMainIcon);
	g_object_unref(icon);
}

void CMainFrame::OnFont(GtkMenuItem* mitem UNUSED, CMainFrame* _this)
{
	GtkWidget* dlg = gtk_font_selection_dialog_new(_("Font"));
	gtk_window_set_modal( (GtkWindow*)dlg, true);
	gtk_window_set_transient_for( (GtkWindow*)dlg, (GtkWindow*)_this->m_Widget);

	GtkFontSelectionDialog* fsdlg = (GtkFontSelectionDialog*)dlg;
	GtkWidget* apply_to_all = gtk_check_button_new_with_label( _("Apply to all opened pages") );
	gtk_widget_show(apply_to_all);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(apply_to_all), true);
	gtk_box_pack_start( GTK_BOX(fsdlg->action_area), apply_to_all, true, true, 4);
	gtk_box_reorder_child( GTK_BOX(fsdlg->action_area), apply_to_all, 0 );
	gtk_box_set_homogeneous(GTK_BOX(fsdlg->action_area), false);

	// This is not a good method because fontsel is a private member of GtkFontSelectionDialog.
	// But we need this functionality.
	GtkFontSelection* fontsel = GTK_FONT_SELECTION(fsdlg->fontsel);
	gtk_widget_set_sensitive(fontsel->face_list, false);

	char pango_font_name[32];
	int *font_size = NULL;
	string *font_family = NULL;
	int font_type;
	const char *font_action = gtk_action_get_name(GTK_ACTION(mitem));
	if (!strcmp(font_action, "non_ascii_font")) {
		font_size = &AppConfig.FontSize;
		font_family = &AppConfig.FontFamily;
		font_type = CTermView::FONT_DEFAULT;
        }
	else if (!strcmp(font_action, "ascii_font")) {
		font_size = &AppConfig.FontSizeEn;
		font_family = &AppConfig.FontFamilyEn;
		font_type = CTermView::FONT_EN;
	}
	else {
		g_assert_not_reached();
	}

	sprintf( pango_font_name, "%s %d", (*font_family).c_str(), (*font_size > 6 && *font_size <= 72) ? *font_size : 12 );
	gtk_font_selection_dialog_set_font_name(fsdlg, pango_font_name);

	if( gtk_dialog_run((GtkDialog*)dlg) == GTK_RESPONSE_OK )
	{
		gchar* name = gtk_font_selection_dialog_get_font_name( fsdlg );
		PangoFontDescription* desc = pango_font_description_from_string( name );
		g_free( name );
		const char* family = pango_font_description_get_family(desc);
		*font_family = family;
		*font_size = pango_font_description_get_size(desc) / PANGO_SCALE;
		pango_font_description_free(desc);

		if( gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(apply_to_all) ) )
		{
			vector<CTelnetView*>::iterator it;
			for( it = _this->m_Views.begin(); it != _this->m_Views.end(); ++it )
				(*it)->SetFontFamily(*font_family, font_type);
			/// FIXME: Poor design! Different connection must be allowed to use different fonts in the future.
		}
		else if( _this->GetCurView() )
			_this->GetCurView()->SetFontFamily(*font_family, font_type);

		gtk_widget_destroy(dlg);

		if( _this->GetCurView() )
			_this->GetCurView()->Refresh();
	}
	else
		gtk_widget_destroy(dlg);
}

void CMainFrame::OnFullscreenMode(GtkMenuItem* mitem UNUSED, CMainFrame* _this)
{
	if(_this->m_Mode == SIMPLE_MODE) {
		gtk_toggle_action_set_active(GTK_TOGGLE_ACTION(gtk_action_group_get_action(_this->m_ActionGroup, "simple")), false);
	}

	if (_this->m_Mode != FULLSCREEN_MODE) {
		_this->m_Mode = FULLSCREEN_MODE;
		gtk_window_fullscreen((GtkWindow *)_this->m_Widget);
		gtk_widget_hide_all((GtkWidget *)_this->m_Menubar);
		gtk_widget_hide_all((GtkWidget *)_this->m_Toolbar);
		gtk_widget_hide_all((GtkWidget *)_this->m_Statusbar);
		_this->m_pNotebook->HideTabs();
	} else {
		_this->m_Mode = NORMAL_MODE;
		gtk_window_unfullscreen((GtkWindow *)_this->m_Widget);
		gtk_widget_show_all((GtkWidget *)_this->m_Menubar);
		if (AppConfig.ShowToolbar)
			gtk_widget_show_all((GtkWidget *)_this->m_Toolbar);
		if (AppConfig.ShowStatusBar)
			gtk_widget_show_all((GtkWidget *)_this->m_Statusbar);
		_this->m_pNotebook->ShowTabs();
	}
}

void CMainFrame::OnSimpleMode(GtkMenuItem* mitem UNUSED, CMainFrame* _this)
{
	if(_this->m_Mode == FULLSCREEN_MODE) {
		gtk_toggle_action_set_active(GTK_TOGGLE_ACTION(gtk_action_group_get_action(_this->m_ActionGroup, "fullscreen")), false);
	}

	if (_this->m_Mode != SIMPLE_MODE) {
		_this->m_Mode = SIMPLE_MODE;
		gtk_window_unfullscreen((GtkWindow *)_this->m_Widget);
		if (_this->m_Unity == false) {
		    gtk_widget_hide_all((GtkWidget *)_this->m_Menubar);
		}
		gtk_widget_hide_all((GtkWidget *)_this->m_Toolbar);
		gtk_widget_hide_all((GtkWidget *)_this->m_Statusbar);
		_this->m_pNotebook->HideTabs();
	} else {
		_this->m_Mode = NORMAL_MODE;
		gtk_window_unfullscreen((GtkWindow *)_this->m_Widget);
		if (_this->m_Unity == false) {
		    gtk_widget_show_all((GtkWidget *)_this->m_Menubar);
		}
		if (AppConfig.ShowToolbar)
			gtk_widget_show_all((GtkWidget *)_this->m_Toolbar);
		if (AppConfig.ShowStatusBar)
			gtk_widget_show_all((GtkWidget *)_this->m_Statusbar);
		_this->m_pNotebook->ShowTabs();
	}
}

void CMainFrame::OnShortcutList(GtkMenuItem* mitem UNUSED, CMainFrame* _this)
{
	char* connect_shortcuts= _(
			"Site List	Alt+S\n\n"
			"New Connection	Alt+Q\n"
			"New Connection	Ctrl+Shift+T\n\n"
			"Reconnection	Alt+R\n"
			"Reconnection   Ctrl+Ins\n\n"
			"Close		Alt+W\n"
			"Close		Ctrl+Del\n\n"
			"Next Page	Alt+X\n"
			"Next Page	Alt+?\n"
			"Next Page	Ctrl+Shift+PgDn\n\n"
			"Previous Page	Alt+Z\n"
			"Previous Page	Alt+?\n"
			"Previous Page	Ctrl+Shift+PgUp\n\n"
			"First Page	Ctrl+Home\n"
			"Last Page	Ctrl+End");

	char* edit_shortcuts= _(
			"Copy		Alt+O\n"
			"Copy		Ctrl+Shift+C\n\n"
			"Paste		Alt+P\n"
			"Paste		Ctrl+Shift+V\n\n"
			"Paste from Clipboard	Shift+Ins\n\n"
			"Emotions		Ctrl+Enter");

	char* view_shortcuts= _(
			"Full Screen Mode	Alt+Enter\n"
			"Simple Mode		Shift+Enter\n"
#ifdef USE_DOCKLET
			"Show Main Window	Alt+M"
#endif
			);
	GtkWidget* dlg = gtk_message_dialog_new_with_markup( (GtkWindow*)_this->m_Widget,
						GTK_DIALOG_DESTROY_WITH_PARENT,
						GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
						_("<b>Connect Shortcuts</b>\n%s\n\n"
						  "<b>Edit Shortcuts</b>\n%s\n\n"
						  "<b>View Shortcuts</b>\n%s\n\n"),
						   connect_shortcuts, edit_shortcuts, view_shortcuts );

	gtk_image_set_from_pixbuf((GtkImage*) ((GtkMessageDialog*)dlg)->image, _this->m_MainIcon);
	gtk_dialog_run((GtkDialog*) dlg); // == GTK_RESPONSE_OK
	gtk_widget_destroy(dlg);
}


void CMainFrame::OnAbout(GtkMenuItem* mitem UNUSED, CMainFrame* _this)
{
	char* authors = _(
			"Hong Jen Yee (Main developer) <pcman.tw@gmail.com>\n"
			"Jim Huang (Developer) <jserv.tw@gmail.com>\n"
			"Kan-Ru Chen (Developer) <kanru@kanru.info>\n"
			"Chia I Wu (Developer) <b90201047@ntu.edu.tw>\n"
			"Shih-Yuan Lee (Developer) <fourdollars@gmail.com>\n"
			"Youchen Lee (Developer) <copyleft@utcr.org>\n"
			"Emfox Zhou (Developer) <emfoxzhou@gmail.com>\n"
			"Jason Xia (Developer) <jasonxh@gmail.com>"
			);
	char* translators = _( "Chinese Simplified (zh_CN): Haifeng Chen <optical.dlz@gmail.com>" );

	GtkWidget* dlg = gtk_message_dialog_new_with_markup( (GtkWindow*)_this->m_Widget,
						GTK_DIALOG_DESTROY_WITH_PARENT,
						GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
						_("<b>PCManX %s</b>\nA free BBS client developed with GTK+ 2.x\n\n"
						"Copyright © 2005-2012\n"
						"License: GNU Genral Public License\n"
						"Project: <a href=\"%s\">%s</a>\n"
						"Mailing List: <a href=\"%s\">%s</a>\n"
						"Bug Report: <a href=\"%s\">%s</a>\n\n"
						"<b>Authors</b>:\n%s\n\n"
						"<b>Translators</b>:\n%s\n\n"), PACKAGE_VERSION, PROJECT_SITE, PROJECT_SITE,
                        PROJECT_FORUM, PROJECT_FORUM, PACKAGE_BUGREPORT, PACKAGE_BUGREPORT, authors, translators );

// GTK+ supports this API since ver 2.6.
/*	gtk_message_dialog_format_secondary_text((GtkMessageDialog*)dlg,
						_("Copyright (C) 2005\n"
						"License: GNU Genral Public License\n"
						"Project Homepage: http://code.google.com/p/pcmanx-gtk2/\n\n"
						"Authors:\n%s\n")
						, authors	);
*/
	gtk_image_set_from_pixbuf((GtkImage*)((GtkMessageDialog*)dlg)->image, _this->m_MainIcon);
	gtk_dialog_run((GtkDialog*)dlg); // == GTK_RESPONSE_OK
	gtk_widget_destroy(dlg);
}

void CMainFrame::pasteFromClipboard(GtkMenuItem* pMenuItem UNUSED, CMainFrame* pMainFrame)
{
	CTelnetView* t_pView = pMainFrame->GetCurView();
	if (t_pView != NULL)
		t_pView->PasteFromClipboard(true);
}

void CMainFrame::OnCloseSelectCon(GtkWidget *notebook, GtkMenuItem* mitem, CMainFrame* _this)
{
	/**
	*   Close pages selected by middle click of tabs.
	*
	*   At first choose which tab is the closet tab to the click location.
	*   Then switch to the selected page.
	*   Finally close it, and then switch back to the original page.
	*
	*   TODO:
	*   The most right hand tab will be selected
	*   even the click location is not exactly on the tab.
	*/
	int window_w = 0;
	int window_h = 0;
	gtk_window_get_size(GTK_WINDOW(gtk_widget_get_toplevel(notebook)), &window_w, &window_h);

	int closet_tab_x = window_w;
	int number_of_pages = gtk_notebook_get_n_pages(GTK_NOTEBOOK(notebook));
	int nth_page_number = 0;
	int number_of_closet_tab = 0;
	/* pick up the tab which is closet to the click location. */
	for(nth_page_number = 0; nth_page_number < number_of_pages; nth_page_number++)
	{
		GtkWidget *tab_label;
		tab_label = gtk_notebook_get_tab_label(  GTK_NOTEBOOK(notebook),
			gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook),
			nth_page_number));
		int lx, ly;
		gtk_widget_get_pointer(tab_label, &lx, &ly);
		if(lx > 0 && lx < closet_tab_x)
		{
			closet_tab_x = lx;
			number_of_closet_tab = nth_page_number;
		}
	}

	/* switch to the page which is clicked. */
	int page_idx_before_close = gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook));
	gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), number_of_closet_tab);
	_this->SetCurView( _this->m_Views[number_of_closet_tab] );

	if ( !(_this->QueryOnCloseCon(_this)) )
	{
		return;
	}

	int page_idx_after_close = 0;
	page_idx_before_close < number_of_closet_tab ? page_idx_after_close = page_idx_before_close : page_idx_after_close = page_idx_before_close - 1;
	/* close the current page and then switch page back to the original one. */
	_this->CloseConAndPageSwitch(_this->m_pNotebook->GetCurPage(), true, notebook, page_idx_after_close);
}


void CMainFrame::OnCloseCon(GtkMenuItem* mitem UNUSED, CMainFrame* _this)
{
	if ( !(_this->QueryOnCloseCon(_this)) )
	{
		return;
	}
	_this->CloseCon(_this->m_pNotebook->GetCurPage(), true);
}

bool CMainFrame::QueryOnCloseCon(CMainFrame* _this)
{
	CTelnetCon* con = _this->GetCurCon();
	if( !con )
		return false;
	if( AppConfig.QueryOnCloseCon && !con->IsClosed() )
	{
		GtkWidget* dlg = gtk_message_dialog_new(GTK_WINDOW(_this->m_Widget),
			GTK_DIALOG_MODAL,
			GTK_MESSAGE_QUESTION,
			GTK_BUTTONS_OK_CANCEL,
			_("Close Connection?"));
		bool can_close = ( gtk_dialog_run(GTK_DIALOG(dlg)) == GTK_RESPONSE_OK );
		gtk_widget_destroy(dlg);
		if ( !can_close )
			return false;
	}
	return true;
}

void CMainFrame::OnCopy(GtkMenuItem* mitem UNUSED, CMainFrame* _this)
{
	if( _this->GetCurView() )
	{
		_this->GetCurView()->CopyToClipboard(false, false, AppConfig.CopyTrimTail);
		if( AppConfig.CancelSelAfterCopy )
		{
			_this->GetCurCon()->m_Sel->NewStart(0, 0);
			_this->GetCurView()->Refresh();
		}
	}
}


void CMainFrame::OnCopyWithColor(GtkMenuItem* mitem UNUSED, CMainFrame* _this)
{
	if( _this->GetCurView() )
	{
		_this->GetCurView()->CopyToClipboard(false, true, AppConfig.CopyTrimTail);
		if( AppConfig.CancelSelAfterCopy )
		{
			_this->GetCurCon()->m_Sel->NewStart(0, 0);
			_this->GetCurView()->Refresh();
		}
	}
}


void CMainFrame::OnNextCon(GtkMenuItem* mitem UNUSED, CMainFrame* _this)
{
	int i = _this->GetNotebook()->GetCurPage() + 1;
	int n = _this->GetNotebook()->GetPageCount();
	_this->GetNotebook()->SetCurPage( i < n ? i : 0 );
}

void CMainFrame::OnFirstCon(GtkMenuItem* mitem UNUSED, CMainFrame* _this)
{
	_this->GetNotebook()->SetCurPage( 0 );
}

void CMainFrame::OnLastCon(GtkMenuItem* mitem UNUSED, CMainFrame* _this)
{
	int n = _this->GetNotebook()->GetPageCount();
	_this->GetNotebook()->SetCurPage( n - 1 );
}

void CMainFrame::OnPaste(GtkMenuItem* mitem UNUSED, CMainFrame* _this)
{
	if(_this->GetCurView())
		_this->GetCurView()->PasteFromClipboard(false);
}

void CMainFrame::OnDownArticle(GtkMenuItem* mitem UNUSED, CMainFrame* _this)
{
  int uao = 0;
	CTelnetCon *con = _this->GetCurCon();
	if (!con)
		return;
	if(_this->GetCurView())
    uao = _this->GetCurView()->m_UAO;

	CDownArticleDlg *dlg = new CDownArticleDlg(_this, con, uao);
	//dlg->Show();
	dlg->ShowModal();
	dlg->Destroy();
}

void CMainFrame::OnPreference(GtkMenuItem* mitem UNUSED, CMainFrame* _this)
{
#ifdef USE_DOCKLET
	bool show_tray_icon = AppConfig.ShowTrayIcon;
#endif
	CPrefDlg* dlg = new CPrefDlg(_this);
	dlg->ShowModal();
	dlg->Destroy();

  CTermView::Opacity = AppConfig.Opacity;

  GtkNotebook* nb = GTK_NOTEBOOK(_this->GetNotebook()->m_Widget);
  int idx = gtk_notebook_get_current_page(nb);
  GtkWidget* view = gtk_notebook_get_nth_page(nb, idx);
  if (view != NULL)
    gtk_widget_queue_draw(view);

	CTelnetView::SetWebBrowser(AppConfig.WebBrowser);
	CTelnetView::SetMailClient(AppConfig.MailClient);

#if defined(USE_NOTIFIER) && !defined(USE_LIBNOTIFY)
	popup_notifier_set_timeout( AppConfig.PopupTimeout );
#endif

#ifdef USE_DOCKLET
	if (AppConfig.ShowTrayIcon != show_tray_icon) {
		if (_this->m_dlhandle != NULL) {
			void (*app_indicator_set_status)(void*, gint) =
				(void (*)(void*, gint)) lt_dlsym(_this->m_dlhandle, "app_indicator_set_status");
			if (AppConfig.ShowTrayIcon) {
				app_indicator_set_status(_this->m_indicator, 1);
			}
			else {
				app_indicator_set_status(_this->m_indicator, 0);
			}
		}
		else {
			if (AppConfig.ShowTrayIcon) {
				_this->ShowTrayIcon();
			}
			else {
				_this->HideTrayIcon();
			}
		}
	}
#endif

	if (_this->m_Mode == NORMAL_MODE) {
		if (AppConfig.ShowToolbar)
			gtk_widget_show_all(_this->m_Toolbar);
		else
			gtk_widget_hide_all(_this->m_Toolbar);
		if (AppConfig.ShowStatusBar)
			gtk_widget_show_all(_this->m_Statusbar);
		else
			gtk_widget_hide_all(_this->m_Statusbar);
	}
}


void CMainFrame::OnPrevCon(GtkMenuItem* mitem UNUSED, CMainFrame* _this)
{
	int i = _this->GetNotebook()->GetCurPage() - 1;
	int n = _this->GetNotebook()->GetPageCount();
	_this->GetNotebook()->SetCurPage( i >= 0 ? i : n-1 );

}

void CMainFrame::OnSiteList(GtkMenuItem* mitem UNUSED, CMainFrame* _this)
{
	CSiteListDlg* dlg = new CSiteListDlg(_this);
	dlg->ShowModal();
	dlg->Destroy();
}


void CMainFrame::OnJumpToPage(GObject* obj, CMainFrame* _this)
{
	INFO("On jump to, obj=%p, _this->m_JumpTos[0]=%p",
	     obj, _this->m_JumpTos[0]);
	for( int i = 0; i < 10; ++i )
		if( obj == _this->m_JumpTos[i] )
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
	m_pNotebook->SetPageTitle( con, markup);
}

void CMainFrame::OnTelnetConClose(CTelnetView* con)
{
	if( !con )
		return;
	string markup = "<span foreground=\"#808080\">";
	markup += con->GetCon()->m_Site.m_Name;
	markup += "</span>";
	m_pNotebook->SetPageTitle( con, markup);
}

void CMainFrame::OnTelnetConConnect(CTelnetView* con)
{
	if( !con )
		return;
	m_pNotebook->SetPageTitle( con, con->GetCon()->m_Site.m_Name );
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
	m_pNotebook->SetPageTitle( con, markup);
}


void CMainFrame::OnNotebookChangeCurPage(GtkNotebook* widget UNUSED,
                                         GtkNotebookPage* page UNUSED,
                                         gint page_num,
                                         CMainFrame* _this)
{
	_this->SetCurView( _this->m_Views[page_num] );
	if( _this->GetCurEditor() != NULL )
		_this->m_eView = (CEditorView *) _this->m_Views[page_num];
}

gboolean CMainFrame::OnNotebookPopupMenu(GtkWidget *widget,
                                         GdkEventButton *event,
                                         CMainFrame* _this)
{
	/* initialized once */
	static GtkWidget *menu = NULL;
	static GtkWidget *menu_item_close = NULL;

	if (menu == NULL) {
		// set menu items
		GtkWidget *menu_item_close =
			gtk_image_menu_item_new_with_label( _("Close") );
		GtkWidget *menu_item_reconnect =
			gtk_image_menu_item_new_with_label( _("Reconnect") );
		GtkWidget *menu_item_add2fav =
			gtk_image_menu_item_new_with_label( _("Add to Favorites") );

		// set images
		GtkWidget *image_close =
			gtk_image_new_from_stock (GTK_STOCK_CLOSE, GTK_ICON_SIZE_MENU);
		GtkWidget *image_reconnect =
			gtk_image_new_from_stock (GTK_STOCK_UNDO, GTK_ICON_SIZE_MENU);
		GtkWidget *image_add2fav =
			gtk_image_new_from_stock (GTK_STOCK_ADD, GTK_ICON_SIZE_MENU);

		gtk_image_menu_item_set_image (
			(GtkImageMenuItem *) menu_item_close, image_close);
		gtk_image_menu_item_set_image (
			(GtkImageMenuItem *) menu_item_reconnect, image_reconnect);
		gtk_image_menu_item_set_image (
			(GtkImageMenuItem *) menu_item_add2fav, image_add2fav);

		menu = gtk_menu_new();

		// widgets show
		gtk_widget_show (menu_item_reconnect);
		gtk_container_add (GTK_CONTAINER (menu), menu_item_reconnect);

		gtk_widget_show (menu_item_close);
		gtk_container_add (GTK_CONTAINER (menu), menu_item_close);

		gtk_widget_show (menu_item_add2fav);
		gtk_container_add (GTK_CONTAINER (menu), menu_item_add2fav);

		// signals
		g_signal_connect ( G_OBJECT(menu_item_reconnect), "activate",
		                G_CALLBACK (CMainFrame::OnReconnect),
		                _this);
		g_signal_connect ( G_OBJECT(menu_item_close), "activate",
		                G_CALLBACK (CMainFrame::OnCloseCon),
		                _this);
		g_signal_connect ( G_OBJECT(menu_item_add2fav), "activate",
		                G_CALLBACK (CMainFrame::OnAddToFavorites),
		                _this);
	}

	// Feature: let mouse middle click be able to close tab
	// similar to the behavior under Firefox
	if (AppConfig.MidClickAsClose &&
	    event->type == GDK_BUTTON_PRESS && event->button == 2) {
		_this->OnCloseSelectCon(widget, GTK_MENU_ITEM(menu_item_close), _this);
		return TRUE;
	}

	// if not right check the mouse
	if (event->type != GDK_BUTTON_PRESS || event->button != 3)
	        return FALSE;

	// popup
	gtk_menu_popup(GTK_MENU(menu), NULL, NULL, NULL, NULL,
		event->button, event->time);
	return TRUE;
}

void CMainFrame::CloseConAndPageSwitch(int idx, bool confirm UNUSED, GtkWidget *notebook, int page_idx)
{
	m_pNotebook->RemovePage(idx);
	m_Views.erase( m_Views.begin() + idx );

	gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), page_idx);
	SetCurView( page_idx >= 0 ? m_Views[page_idx] : NULL );
}

void CMainFrame::CloseCon(int idx, bool confirm UNUSED)
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

gboolean CMainFrame::OnWindowStateEvent(GtkWindow* window,
										GdkEventWindowState* event,
										CMainFrame* _this)
{
	if ((event->changed_mask & GDK_WINDOW_STATE_MAXIMIZED) == GDK_WINDOW_STATE_MAXIMIZED) {
		if ((event->new_window_state & GDK_WINDOW_STATE_MAXIMIZED) == GDK_WINDOW_STATE_MAXIMIZED) {
			AppConfig.Maximized = true;
		} else {
			AppConfig.Maximized = false;
		}
	}
	return true;
}

gboolean CMainFrame::OnClose( GtkWidget* widget UNUSED,
                              GdkEvent* evt UNUSED,
                              CMainFrame* _this )
{
	return !_this->CanClose();
}


void CMainFrame::OnDestroy()
{
	g_source_remove( m_BlinkTimer );
	g_source_remove( m_EverySecondTimer );

	Hide();
#ifdef USE_DOCKLET
	g_object_unref( m_TrayIcon );
#endif

	//while( g_main_context_iteration(NULL, FALSE) );
	while (gtk_events_pending())
		gtk_main_iteration();

	CWidget::OnDestroy();

#ifdef USE_SCRIPT
	FinalizeScriptInterface();
#endif

	gtk_main_quit();

	if (m_dlhandle != NULL) {
		lt_dlclose(m_dlhandle);
		m_dlhandle = NULL;
	}
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
						_("Quit PCManX ?"));
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


void CMainFrame::OnAddToFavorites(GtkMenuItem* widget UNUSED,
                                  CMainFrame* _this)
{
	if(_this->m_Views.empty() )
		return;

	CSite &site = _this->GetCurCon()->m_Site;
	CSiteDlg* dlg = new CSiteDlg(_this, _("Add To Favorites"), site );
	if(dlg->ShowModal() == GTK_RESPONSE_OK)
	{
		AppConfig.Favorites.push_back(dlg->m_Site);
		CSite& newsite = AppConfig.Favorites.back();

		GtkWidget* fav_item = gtk_image_menu_item_new_with_label( newsite.m_Name.c_str() );
		gtk_widget_show (fav_item);
		gtk_menu_shell_insert( GTK_MENU_SHELL(_this->m_FavoritesMenu), fav_item,
			AppConfig.Favorites.size()>0 ? (AppConfig.Favorites.size()-1) : 0 );

		GtkWidget* image = gtk_image_new_from_stock (GTK_STOCK_NETWORK, GTK_ICON_SIZE_MENU);

		gtk_widget_show (image);
		gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (fav_item), image);

		newsite.m_MenuItem = fav_item;

		g_signal_connect( G_OBJECT(fav_item), "activate",
						G_CALLBACK(CMainFrame::OnFavorite),
						_this);
	}
	dlg->Destroy();
}

void CMainFrame::OnEditFavorites(GtkMenuItem* widget UNUSED, CMainFrame* _this)
{
	CEditFavDlg* dlg = new CEditFavDlg(_this, AppConfig.Favorites);
	dlg->ShowModal();
	AppConfig.Favorites.swap(dlg->m_Sites);
	AppConfig.SaveFavorites();
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

		GtkWidget* image = gtk_image_new_from_stock (GTK_STOCK_NETWORK, GTK_ICON_SIZE_MENU);
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

	GtkWidget* image347 = gtk_image_new_from_stock (GTK_STOCK_ADD, GTK_ICON_SIZE_MENU);
	gtk_widget_show (image347);
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (add_to_fav_menu), image347);

	GtkWidget* edit_fav_menu = gtk_image_menu_item_new_with_mnemonic (_("_Edit Favorites"));

	gtk_widget_show (edit_fav_menu);
	gtk_container_add (GTK_CONTAINER (favorites_menu), edit_fav_menu);

	GtkWidget* image348 = gtk_image_new_from_stock (GTK_STOCK_EDIT, GTK_ICON_SIZE_MENU);
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

void CMainFrame::CreateTrayIcon()
{
#ifdef USE_DOCKLET
	// Setup popup menu
	m_TrayPopup = gtk_ui_manager_get_widget(m_UIManager, "/ui/tray_popup");
	if (m_dlhandle != NULL) {
		void*(*app_indicator_new)(const gchar*, const gchar*, gint) =
			(void*(*)(const gchar*, const gchar*, gint)) lt_dlsym(m_dlhandle, "app_indicator_new");
		void (*app_indicator_set_menu)(void*, GtkMenu *) =
			(void (*)(void*, GtkMenu *)) lt_dlsym(m_dlhandle, "app_indicator_set_menu");
		void (*app_indicator_set_status)(void*, gint) =
			(void (*)(void*, gint)) lt_dlsym(m_dlhandle, "app_indicator_set_status");
		m_indicator = app_indicator_new("PCManX", DATADIR "/pixmaps/pcmanx.svg", 0);
		app_indicator_set_menu(m_indicator, GTK_MENU(m_TrayPopup));
		if (AppConfig.ShowTrayIcon) {
			app_indicator_set_status(m_indicator, 1);
		}
		else {
			app_indicator_set_status(m_indicator, 0);
		}
	}
	else {
		m_TrayIcon = gtk_status_icon_new();
		gtk_status_icon_set_from_pixbuf(m_TrayIcon, m_MainIcon);
		gtk_status_icon_set_tooltip(m_TrayIcon, "PCManX");

		g_signal_connect (G_OBJECT (m_TrayIcon), "popup-menu",
				G_CALLBACK (CMainFrame::OnTray_Popup), this);

		g_signal_connect (G_OBJECT (m_TrayIcon), "activate",
				G_CALLBACK (CMainFrame::OnTrayButton_Toggled), this);
	}
#endif
}

void CMainFrame::OnFavorite(GtkMenuItem* item, CMainFrame* _this)
{
	vector<CSite>::iterator it;
	for( it = AppConfig.Favorites.begin(); it != AppConfig.Favorites.end(); ++it )
	{
		CSite& site = *it;
		if( site.m_MenuItem == (GtkWidget*)item )
		{
			_this->NewCon( site.m_Name, site.m_URL, &site );
			break;
		}
	}
}


static void format_elapsed_time_str( char* time_str, unsigned int sec )
{
	static const char* time_format = _("Time Elapsed : %02d : %02d : %02d");
	unsigned int min = sec / 60;
	sec %= 60;
	unsigned int hr = min / 60;
	min %= 60;
	sprintf( time_str, time_format, hr, min, sec );
}


void CMainFrame::SetCurView(CTelnetView* view)
{
	m_pView = view;
	if( !m_pView || !m_pView->GetCon() )
	{
		gtk_window_set_title (GTK_WINDOW (m_Widget), "PCManX "VERSION );
		gtk_entry_set_text( GTK_ENTRY(m_URLEntry), "");
		return;
	}

	CTelnetCon* con = GetCurCon();
	gtk_entry_set_text( GTK_ENTRY(m_URLEntry), con->m_Site.m_URL.c_str());

	string title = con->m_Site.m_Name;

	if( ! con->IsClosed() )
		m_pNotebook->SetPageTitle( m_pView, title );

	title += " - PCManX "VERSION;
	gtk_window_set_title (GTK_WINDOW (m_Widget), title.c_str() );

	char time_str[100];
	format_elapsed_time_str(time_str, con->m_Duration);
	gtk_label_set_text( m_StatusBarTime, time_str );

#ifdef USE_NANCY
	UpdateBotStatus();
#endif

}

void CMainFrame::OnSelectAll(GtkMenuItem* mitem UNUSED, CMainFrame* _this)
{
	CTelnetCon* con = _this->GetCurCon();
	if( con )
	{
		con->m_Sel->SelectPage();
		_this->GetCurView()->Refresh();
	}
}

void CMainFrame::LoadStartupSites()
{
	vector<CSite>::iterator it=AppConfig.Favorites.begin();
	for( it = AppConfig.Favorites.begin(); it != AppConfig.Favorites.end(); ++it )
	{
		CSite& site = *it;
		if( site.m_Startup )
			NewCon( site.m_Name, site.m_URL, &site);
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
	CTelnetCon* con = _this->GetCurCon();
	if( con )
	{
		char time_str[100];
		format_elapsed_time_str(time_str, con->m_Duration);
		gtk_label_set_text( _this->m_StatusBarTime, time_str );
	}
	return true;
}

void CMainFrame::OnEmoticons(GtkMenuItem* mitem UNUSED, CMainFrame* _this)
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

void CMainFrame::OnReconnect(GtkMenuItem* mitem UNUSED, CMainFrame* _this)
{
	CTelnetCon* con = _this->GetCurCon();
	if( !con )
		return;
	if( con->IsClosed() )
		con->Reconnect();
	else
		_this->NewCon( con->m_Site.m_Name, con->m_Site.m_URL, &con->m_Site);
}

void CMainFrame::FlashWindow( bool flash )
{
	GdkPixbuf* icon = (m_IsFlashing = flash) ? m_InverseMainIcon : m_MainIcon;
	gtk_window_set_icon((GtkWindow*)m_Widget, icon);
}


gboolean CMainFrame::OnActivated( GtkWidget* widget UNUSED,
                                  GdkEventFocus* evt UNUSED,
                                  CMainFrame* _this )
{
	if( _this->m_IsFlashing )
		_this->FlashWindow(false);
	return false;
}

gboolean CMainFrame::OnURLEntryKeyDown(GtkWidget *widget, GdkEventKey *evt, CMainFrame* _this)
{
	switch(evt->keyval)
	{
	case GDK_Return:
	case GDK_KP_Enter:
	{
		string url = gtk_entry_get_text( GTK_ENTRY(widget) );
		if( !url.empty() )
		{
			_this->NewCon( url, url );
			return true;
		}
		//	else goto case GDK_Escape
	}
	case GDK_Escape:
		if( _this->GetCurView() )
			_this->GetCurView()->SetFocus();
		else
			gtk_entry_set_text( GTK_ENTRY(widget), "" );
		return true;
	}
	return false;
}

gboolean CMainFrame::OnURLEntryKillFocus(GtkWidget* entry,
                                         GdkEventFocus* evt UNUSED,
                                         CMainFrame* _this)
{
	if( _this && _this->GetCurView() )
	{
		gtk_entry_set_text( GTK_ENTRY(entry), _this->GetCurCon()->m_Site.m_URL.c_str());
		gtk_editable_select_region(GTK_EDITABLE(entry), 0, 0);
	}
	return FALSE;
}


int CMainFrame::GetViewIndex(CTermView* view)
{
	DEBUG( "get view index, view = %p", view );
	if( !view )
		return -1;
	DEBUG( "view->m_Widget = %p", view->m_Widget );
	return gtk_notebook_page_num( GTK_NOTEBOOK(m_pNotebook->m_Widget), view->m_Widget );
}


void CMainFrame::SwitchToCon(CTelnetCon* con)
{
	int idx = GetViewIndex( con->GetView() );
	DEBUG( "switch to con %d", idx );
	if( idx >= 0 )
		m_pNotebook->SetCurPage(idx);
}

#ifdef USE_NANCY

void CMainFrame::OnChangeCurrentBot(GtkRadioAction *action UNUSED,
                                    GtkRadioAction *current,
                                    CMainFrame* _this)
{
  CTelnetCon* con = _this->GetCurCon();
  if( !con ) return;
  if( gtk_radio_action_get_current_value(current) == 0 )
    con->set__UseNancy(false);
  else
    con->set__UseNancy(true);

  _this->UpdateBotStatus();
}

void CMainFrame::OnChangeAllBot(GtkRadioAction *action UNUSED,
                                GtkRadioAction *all,
                                CMainFrame* _this)
{
  if( _this->m_Views.empty() ) return;
  gboolean use_nancy = ( gtk_radio_action_get_current_value(all) != 0 );
  CTelnetCon::set__OpenConnectionWithNancySupport(use_nancy);

  vector<CTelnetView*>::iterator it = _this->m_Views.begin();
  for( ; it != _this->m_Views.end() ; ++it )
    (*it)->GetCon()->set__UseNancy(use_nancy);

  if( use_nancy )
    gtk_check_menu_item_set_active( (GtkCheckMenuItem*)_this->m_CurBotNancyRadio, true );
  else
    gtk_check_menu_item_set_active( (GtkCheckMenuItem*)_this->m_DisableCurBotRadio, true );

  _this->UpdateBotStatus();
}

void CMainFrame::UpdateBotStatus()
{
	if( GetCurCon()->get__UseNancy() )
	{
		gtk_check_menu_item_set_active( (GtkCheckMenuItem*)m_CurBotNancyRadio, true );
		gchar* bot_state = g_markup_printf_escaped( "<span foreground=\"#ff0000\">%s</span>", _("Nancy Bot is enabled") );
		gtk_label_set_markup( m_StatusBarBotState, bot_state );
		g_free(bot_state);
	}
	else
	{
		gtk_check_menu_item_set_active( (GtkCheckMenuItem*)m_DisableCurBotRadio, true );
		gchar* bot_state = g_markup_printf_escaped( "<span foreground=\"gray\">%s</span>", _("Nancy Bot is disabled") );
		gtk_label_set_markup( m_StatusBarBotState, bot_state );
		g_free(bot_state);
	}
}

#endif	//	#ifdef USE_NANCY
/* vim: set fileencodings=utf-8 tabstop=4 noexpandtab shiftwidth=4 softtabstop=4: */



/**
*   Open Ansi Editor. Send url string "ansi_editor" to NewCon().
*   The NewCon() method will create CEditor object when the url is "ansi_editor".
*/
void CMainFrame::OnAnsiEditor(GtkMenuItem *mitem, CMainFrame *_this)
{
	_this->NewCon("Ansi Editor", "ansi_editor");
}

void CMainFrame::OnOpenAnsiFile(GtkMenuItem *mitem, CMainFrame *_this)
{
	GtkWidget *dialog = gtk_file_chooser_dialog_new (
							"Open Ansi File",
							NULL,
							GTK_FILE_CHOOSER_ACTION_OPEN,
							GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
							GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
							NULL);

	GtkFileFilter *filter = gtk_file_filter_new();
	gtk_file_filter_set_name(filter, "Ansi file");
	gtk_file_filter_add_pattern(filter, "*.ans");
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);

	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
	{
		string path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER (dialog));
		size_t pos = path.find_last_of("/\\");
		string filename = path.substr(pos + 1, -1);
		string title = "AnsiEditor: ";
		title += filename;
		_this->NewCon(title, "ansi_editor");
		_this->GetCurEditor()->EditorActions(CEditor::Load_Ansi_File, path);
		_this->GetCurEditorView()->UpdateEditor();
	}
	gtk_widget_destroy (dialog);
}

void CMainFrame::OnSaveAnsiFile(GtkMenuItem *mitem, CMainFrame *_this)
{
	//do nothing if the current view is not Ansi Editor
	if ( _this->GetCurEditor() == NULL )
		return;

	GtkWidget *dialog = gtk_file_chooser_dialog_new (
							"Save Ansi File",
							GTK_WINDOW(_this),
							GTK_FILE_CHOOSER_ACTION_SAVE,
							GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
							GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
							NULL);

	GtkFileFilter *filter = gtk_file_filter_new();
	gtk_file_filter_set_name(filter, "Ansi file");
	gtk_file_filter_add_pattern(filter, "*.ans");
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);

	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
	{
		string filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER (dialog));
		_this->GetCurEditor()->EditorActions(CEditor::Save_Ansi_File, filename);
	}
	gtk_widget_destroy (dialog);
}

void CMainFrame::OnClearScreen(GtkMenuItem *mitem, CMainFrame *_this)
{
	//do nothing if the current view is not Ansi Editor
	if ( _this->GetCurEditor() == NULL )
		return;

	_this->GetCurEditor()->EditorActions(CEditor::Clear_Screen);
	_this->GetCurEditorView()->UpdateEditor();
}

/**
*   Set text to blink.
*/
void CMainFrame::SetBlink(GtkToggleButton *togglebutton, CMainFrame *_this)
{
	//do nothing if the current view is not Ansi Editor
	if( _this->m_Views.empty() ||  _this->GetCurEditor() == NULL )
		return;

	bool blink = (togglebutton->active)? true: false;

	_this->GetCurEditor()->ApplyAnsiColor(-1, blink, -1, -1);

	_this->GetCurEditorView()->SetFocus();
}


void CMainFrame::SetTextColor(GtkComboBox *widget, CMainFrame* _this)
{
	//do nothing if the current view is not Ansi Editor
	if( _this->GetCurEditor() == NULL )
		return;

	//do not change color if view is empty
	if( _this->m_Views.empty() )
	{
		gtk_combo_box_set_active(GTK_COMBO_BOX(_this->m_cbTextColor), 0);
		return;
	}

	int cbIndex = gtk_combo_box_get_active(GTK_COMBO_BOX(_this->m_cbTextColor));

	//index 0 is just a title, not a color. Do nothing
	if( cbIndex == 0 )
		return;

	cbIndex--;

	bool bright = (int)(cbIndex / 8);

	int foreground;
	stringstream ss;
	ss << cbIndex % 8;
	ss >> foreground;

	_this->GetCurEditor()->ApplyAnsiColor(bright, -1, foreground, -1);
	_this->GetCurEditorView()->SetFocus();
}


void CMainFrame::SetBgColor(GtkComboBox *widget, CMainFrame* _this)
{
	//do nothing if the current view is not Ansi Editor
	if( _this->GetCurEditor() == NULL )
		return;

	//do not change color if view is empty
	if( _this->m_Views.empty() )
	{
		gtk_combo_box_set_active(GTK_COMBO_BOX(_this->m_cbBgColor), 0);
		return;
	}

	int cbIndex = gtk_combo_box_get_active(GTK_COMBO_BOX(_this->m_cbBgColor));

	//index 0 is just a title, not a bg color. Do nothing
	if( cbIndex == 0 )
		return;

	cbIndex--;

	stringstream ss;
	ss << cbIndex % 8;
	int bgColor;
	ss >> bgColor;

	_this->GetCurEditor()->ApplyAnsiColor(-1, -1, -1, bgColor);
	_this->GetCurEditorView()->SetFocus();
}

void CMainFrame::AppendRow(GtkTreeIter *iter, GtkListStore *store, const gchar *display, const gchar *color)
{
	gtk_list_store_append(store, iter);
	gtk_list_store_set(store, iter, 0, display, 1, color, -1);
}

/**
*   parse the output format from GdkColor: #rrrrggggbbbb to the normal format: #rrggbb
*/
void CMainFrame::ParseColor(GdkColor *color, gchar *res, size_t res_len)
{
	g_assert(res_len >= 8);

	string color_present = gdk_color_to_string(color);

	res[0] = color_present[0];
	res[1] = color_present[1];
	res[2] = color_present[2];
	res[3] = color_present[5];
	res[4] = color_present[6];
	res[5] = color_present[9];
	res[6] = color_present[10];
	res[7] = 0;
}
