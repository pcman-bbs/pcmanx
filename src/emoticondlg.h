//
// C++ Interface: emoticondlg
//
// Description: 
//
//
// Author: Hong Jen Yee (PCMan) <hzysoft@sina.com.tw>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef EMOTICONDLG_H
#define EMOTICONDLG_H

#include "dialog.h"
#include "listbox.h"
#include <string>
#include <stdio.h>

using namespace std;

/**
@author Hong Jen Yee (PCMan)
*/
class CEmoticonDlg : public CDialog
{
public:
    CEmoticonDlg(CWidget* parent);
    ~CEmoticonDlg();
    string GetSelectedStr(){	return m_SelStr;	}
    static void OnDown(GtkWidget* btn, CEmoticonDlg* _this);
    static void OnAdd(GtkWidget* btn, CEmoticonDlg* _this);
    static void OnEdit(GtkWidget* btn, CEmoticonDlg* _this);
    static void OnRemove(GtkWidget* btn, CEmoticonDlg* _this);
    static void OnUp(GtkWidget* btn, CEmoticonDlg* _this);
    bool OnOK();
    void LoadEmoticons();
    void SaveEmoticons();
    bool OnCancel();
    static void OnListRowActivated(GtkTreeView* view, GtkTreePath* path, GtkTreeViewColumn* cols, CEmoticonDlg* _this);
    CListBox* m_List;
protected:
    string m_SelStr;
    bool m_IsModified;
protected:
    static gboolean SaveEmoticon(GtkTreeModel* model, GtkTreePath* path, GtkTreeIter* iter, FILE* file);
};

#endif
