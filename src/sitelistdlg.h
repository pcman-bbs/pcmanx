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

#ifndef SITELISTDLG_H
#define SITELISTDLG_H

#include "dialog.h"
#include <stdio.h>

/**
@author PCMan
*/
class CMainFrame;

class CSiteListDlg : public CDialog
{
public:
    CSiteListDlg(CWidget* parent);

    ~CSiteListDlg();
    static void OnSearch(GtkButton* btn, CSiteListDlg* _this);
    static void OnConnect(GtkButton* btn, CSiteListDlg* _this);
    static void OnClose(GtkButton* btn, CSiteListDlg* _this);
    void LoadSiteList();
	void LoadSiteList(GtkTreeIter *parent, FILE *file, char *line);
    static void OnRowActivated(GtkTreeView *tree_view, GtkTreePath* path, GtkTreeViewColumn* col, CSiteListDlg* _this);
public:
	GtkWidget *m_Keyword;
	GtkWidget *m_Tree;
	GtkWidget *m_ConnectBtn;
	GtkTreeStore* m_Store;
protected:
    GdkPixbuf* m_FolderIcon;
    GdkPixbuf* m_SiteIcon;
	CMainFrame* m_pParent;
};

#endif
