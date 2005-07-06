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
#ifndef MAINFRAME_H
#define MAINFRAME_H


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "widget.h"

#ifdef USE_DOCKLET
#include "docklet/eggtrayicon.h"
#endif

#include <string>
#include <queue>
#include <vector>

using namespace std;

#include "telnetview.h"

/**
@author PCMan
*/

class CTelnetView;
class CNotebook;
class CTelnetCon;
class CSite;

class CMainFrame : public CWidget
{
public:
    CMainFrame();
    ~CMainFrame();

    CTelnetCon* NewCon(const char* title, const char* url, CSite* site);
	CNotebook* GetNotebook(){	return m_pNotebook;	}
    void OnConBell(CTelnetView* con);
    static void OnFont(GtkMenuItem* mitem, CMainFrame* _this);
    static void OnAbout(GtkMenuItem* mitem, CMainFrame* _this);
    static void OnCloseCon(GtkMenuItem* mitem, CMainFrame* _this);
    static void OnCopy(GtkMenuItem* mitem, CMainFrame* _this);
    static void OnCopyWithColor(GtkMenuItem* mitem, CMainFrame* _this);
    static void OnNextCon(GtkMenuItem* mitem, CMainFrame* _this);
    static void OnPaste(GtkMenuItem* mitem, CMainFrame* _this);
    static void OnPreference(GtkMenuItem* mitem, CMainFrame* _this);
    static void OnPrevCon(GtkMenuItem* mitem, CMainFrame* _this);
    static void OnSiteList(GtkMenuItem* mitem, CMainFrame* _this);
    static void OnNotebookChangeCurPage(GtkNotebook* widget, GtkNotebookPage* page,  gint page_num, CMainFrame* _this);
	void SetCurView(CTelnetView* view);
	CTelnetView* GetCurView(){	return (m_pView);	}
	CTelnetCon* GetCurCon() {	return (m_pView ? m_pView->GetCon():NULL);	}
//	CTelnetView* LookupView(GtkWidget* view){	return (CTelnetView*) g_hash_table_lookup(m_TelnetViewHash, view);	}
    static gboolean OnBlinkTimer(CMainFrame* _this);
    static gboolean OnClose( GtkWidget* widget, GdkEvent* evt, CMainFrame* _this );
    GtkWidget* m_JumpMenuItems[10];
    void OnDestroy();
    virtual void OnCreate();
    virtual bool CanClose();
    void NotImpl(const char* str);
    static void OnEditFavorites(GtkMenuItem* widget, CMainFrame* _this);
    static void OnFavorite(GtkMenuItem* item, CMainFrame* _this);

//    queue<>;
	vector<CTelnetView*> m_Views;
#ifdef USE_DOCKLET
    EggTrayIcon *m_TrayIcon_Instance;
#endif

protected:
    void CreateMenu();
    void CreateToolbar();
    static void OnNewCon(GtkMenuItem* mitem, CMainFrame* _this);
    static void OnQuit(GtkMenuItem* mitem, CMainFrame* _this);
    void LoadIcons();
	void LoadStartupSites();
    static void OnJumpToPage(GtkWidget* widget, CMainFrame* _this);
    void CloseCon(int idx, bool confirm = false);
    static void OnAddToFavorites(GtkMenuItem* widget, CMainFrame* _this);
    void CreateFavoritesMenu();
    static void OnSelectAll(GtkMenuItem* mitem, CMainFrame* _this);

#ifdef USE_DOCKLET
    static void OnTrayButton_Toggled(GtkToggleButton *button, CMainFrame* _this);
    static void OnTrayButton_Changed(CMainFrame* _this);
    void set_tray_icon();
    GtkWidget *m_TrayButton;
    GtkWidget *m_TrayIcon;
#endif

    GdkPixbuf* m_ConnIcon;
    GdkPixbuf* m_MainIcon;
protected:

protected:
    CTelnetView* m_pView;
    CNotebook* m_pNotebook;
    GtkWidget* m_Toolbar;
    GtkWidget* m_Menubar;
    GtkWidget* m_EditMenu;

    GtkAccelGroup* m_AccelGroup;

    gint m_BlinkTimer;
    GtkWidget* m_FavoritesMenuItem;
    GtkWidget* m_FavoritesMenu;
};

#endif
