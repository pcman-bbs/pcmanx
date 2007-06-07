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

#ifndef MAINFRAME_H
#define MAINFRAME_H

#ifdef __GNUG__
  #pragma interface "mainframe.h"
#endif

#include "pcmanx_utils.h"

#include "widget.h"

#ifdef USE_DOCKLET
#include "docklet/api.h"
#endif

#include <string>
#include <vector>

using namespace std;

#include "telnetview.h"
#include <signal.h>

/**
@author PCMan
*/

class CTelnetView;
class CNotebook;
class CTelnetCon;
class CSite;

class CMainFrame : public CWidget
{
	static bool g_bIsUpateHandlerExisted;
	static bool g_bUpdateingBBSList;
	static CMainFrame* g_pMyself;
	static GtkActionEntry entries[];
public:
	CMainFrame();

	CTelnetCon* NewCon( string title, string url, CSite* site = NULL );
	CNotebook* GetNotebook(){	return m_pNotebook;	}
	void OnTelnetConBell(CTelnetView* con);
	void OnTelnetConClose(CTelnetView* con);
	void OnTelnetConConnect(CTelnetView* con);
	void OnTelnetConRecv(CTelnetView* con);
	static void OnFont(GtkMenuItem* mitem, CMainFrame* _this);
	static void OnFontEn(GtkMenuItem* mitem, CMainFrame* _this);
	static void OnAbout(GtkMenuItem* mitem, CMainFrame* _this);
#ifdef USE_WGET
	static void updateBBSList(GtkMenuItem* mitem, CMainFrame* _this);
	static void updateBBSListHandler(int nSignalNumber);
#endif
	static void pasteFromClipboard(GtkMenuItem* mitem, CMainFrame* _this);
	static void OnCloseCon(GtkMenuItem* mitem, CMainFrame* _this);
	static void OnCopy(GtkMenuItem* mitem, CMainFrame* _this);
	static void OnCopyWithColor(GtkMenuItem* mitem, CMainFrame* _this);
	static void OnNextCon(GtkMenuItem* mitem, CMainFrame* _this);
	static void OnPaste(GtkMenuItem* mitem, CMainFrame* _this);
	static void OnPreference(GtkMenuItem* mitem, CMainFrame* _this);
	static void OnPrevCon(GtkMenuItem* mitem, CMainFrame* _this);
	static void OnSiteList(GtkMenuItem* mitem, CMainFrame* _this);
	static void OnNotebookChangeCurPage(GtkNotebook* widget, GtkNotebookPage* page,  gint page_num, CMainFrame* _this);
	static gboolean OnNotebookPopupMenu(GtkWidget *widget, GdkEventButton *event, gpointer p_mainframe);
	void SetCurView(CTelnetView* view);
	CTelnetView* GetCurView(){	return (m_pView);	}
	CTelnetCon* GetCurCon() {	return (m_pView ? m_pView->GetCon():NULL);	}
//	CTelnetView* LookupView(GtkWidget* view){	return (CTelnetView*) g_hash_table_lookup(m_TelnetViewHash, view);	}
	static gboolean OnBlinkTimer(CMainFrame* _this);
	static gboolean OnEverySecondTimer(CMainFrame* _this);
	static gboolean OnClose( GtkWidget* widget, GdkEvent* evt, CMainFrame* _this );
	static gboolean OnSize( GtkWidget* widget, GdkEventConfigure* evt, CMainFrame* _this );
	GtkWidget* m_JumpMenuItems[10];
	void OnDestroy();
	virtual void OnCreate();
	virtual bool CanClose();
	void NotImpl(const char* str);
	static void OnEditFavorites(GtkMenuItem* widget, CMainFrame* _this);
	static void OnFavorite(GtkMenuItem* item, CMainFrame* _this);
	static void OnEmoticons(GtkMenuItem* mitem, CMainFrame* _this);
	static gboolean OnActivated( GtkWidget* widget, GdkEventFocus* evt, CMainFrame* _this );

	bool IsActivated(){	return gtk_window_is_active(GTK_WINDOW(m_Widget));	}
	static gboolean OnURLEntryKeyDown(GtkWidget *widget,GdkEventKey *evt, CMainFrame* _this);
	int GetViewIndex(CTermView* view);
	void SwitchToCon(CTelnetCon* con);

	vector<CTelnetView*> m_Views;
#ifdef USE_DOCKLET
	void ShowTrayIcon(){ gtk_widget_show (GTK_WIDGET (m_TrayIcon_Instance) ); };
	void HideTrayIcon(){ gtk_widget_hide (GTK_WIDGET (m_TrayIcon_Instance) ); };
	EggTrayIcon *m_TrayIcon_Instance;
#endif

#ifdef USE_NOTIFIER
	GdkPixbuf *GetMainIcon() { return m_MainIcon; };
#endif

protected:
	void MakeUI();
	static void OnNewCon(GtkMenuItem* mitem, CMainFrame* _this);
	static void OnQuit(GtkMenuItem* mitem, CMainFrame* _this);
	static void OnFullscreenMode(GtkToggleAction* action, CMainFrame* _this);
	static GtkToggleActionEntry fullscreen_mode_entries[];
	void LoadIcons();
	void LoadStartupSites();
	static void OnJumpToPage(GtkWidget* widget, CMainFrame* _this);
	void CloseCon(int idx, bool confirm = false);
	static void OnAddToFavorites(GtkMenuItem* widget, CMainFrame* _this);
	void CreateFavoritesMenu();
	static void OnSelectAll(GtkMenuItem* mitem, CMainFrame* _this);
	static void OnReconnect(GtkMenuItem* mitem, CMainFrame* _this);
	void FlashWindow( bool flash );
	static gboolean OnURLEntryKillFocus(GtkWidget* entry, GdkEventFocus* evt, CMainFrame* _this);

#ifdef USE_NANCY
	static GtkRadioActionEntry cur_bot_entries[];
	static GtkRadioActionEntry all_bot_entries[];
	void UpdateBotStatus();
	static void OnChangeCurrentBot(GtkRadioAction* action, GtkRadioAction* current, CMainFrame* _this);
	static void OnChangeAllBot(GtkRadioAction* action, GtkRadioAction* all, CMainFrame* _this);
#endif

#ifdef USE_DOCKLET
	static void OnTrayButton_Toggled(GtkToggleButton *button, CMainFrame* _this);
//	static void OnTrayButton_Changed(GtkWidget* widget, GtkAllocation *allocation, CMainFrame* _this);
	void set_tray_icon();
	GtkWidget *m_TrayButton;
	GtkWidget *m_TrayIcon;
#endif

	GdkPixbuf* m_ConnIcon;
	GdkPixbuf* m_MainIcon;
	GdkPixbuf* m_InverseMainIcon;

protected:
	CTelnetView* m_pView;
	CNotebook* m_pNotebook;
	GtkUIManager* m_UIManager;
	GtkWidget* m_Toolbar;
	GtkWidget* m_Menubar;
	GtkWidget* m_EditMenu;
	GtkWidget* m_Statusbar;

	GtkAccelGroup* m_AccelGroup;

	guint m_BlinkTimer;
	guint m_EverySecondTimer;
	GtkWidget* m_FavoritesMenuItem;
	GtkWidget* m_FavoritesMenu;

	bool m_IsFlashing;
	GtkWidget* m_URLEntry;
	GtkTooltips* m_Tooltips;
	GtkLabel* m_StatusBarTime;

#ifdef USE_NANCY
	GtkLabel* m_StatusBarBotState;
	GtkRadioMenuItem* m_DisableCurBotRadio;
	GtkRadioMenuItem* m_CurBotNancyRadio;
	GtkRadioMenuItem* m_DisableAllBotRadio;
	GtkRadioMenuItem* m_AllBotNancyRadio;
#endif
};

#endif
