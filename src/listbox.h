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
#ifndef LISTBOX_H
#define LISTBOX_H

#include <gtk/gtk.h>
#include <widget.h>

/**
@author PCMan
*/
class CListBox : public CWidget
{
public:
    CListBox();

    ~CListBox();


	int Count() {	return gtk_tree_model_iter_n_children(GTK_TREE_MODEL(m_Store), NULL);	}
    void Append(const char* text);
    void Insert(int pos, const char*text);

    void Delete(GtkTreeIter* iter){	gtk_list_store_remove(m_Store, iter);	}
    void Delete(int idx);

    void MoveUp(int idx);
    void MoveDown(int idx);

    int GetCurSel();
    void SetItemText(int idx, const char* text);
    void SetCurSel(int idx);

protected:
    GtkListStore* m_Store;
};

#endif
