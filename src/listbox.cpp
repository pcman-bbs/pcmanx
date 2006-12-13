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
  #pragma implementation "listbox.h"
#endif

#include "listbox.h"

#include "debug.h"

CListBox::CListBox()
        : CWidget()
{

	m_Widget = gtk_tree_view_new();
	PostCreate();
	gtk_tree_view_set_headers_visible (GTK_TREE_VIEW(m_Widget), false);

	GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
	GtkTreeViewColumn *col = gtk_tree_view_column_new_with_attributes("", renderer, "text", 0, NULL);
	gtk_tree_view_insert_column( (GtkTreeView*)m_Widget, col, -1);

	m_Store = gtk_list_store_new(1, G_TYPE_STRING );

	gtk_tree_view_set_model(GTK_TREE_VIEW(m_Widget), GTK_TREE_MODEL(m_Store));
	g_object_unref(m_Store); // destroy model automatically with view

	gtk_tree_selection_set_mode( gtk_tree_view_get_selection(GTK_TREE_VIEW(m_Widget)), GTK_SELECTION_BROWSE);

	Show();
}


void CListBox::Append(string text)
{
	GtkTreeIter iter;
	gtk_list_store_append(m_Store, &iter);
	gtk_list_store_set(m_Store, &iter, 0, (GValue*)text.c_str(), -1);
}


void CListBox::Insert(int pos, string text)
{
	GtkTreeIter iter;
	gtk_list_store_insert(m_Store, &iter, pos);
	gtk_list_store_set(m_Store, &iter, 0, (GValue*)text.c_str(), -1);
}


void CListBox::Delete(int idx)
{
	if( idx >= Count() || idx <0 )
		return;
	GtkTreeIter iter;
	gtk_tree_model_iter_nth_child(GTK_TREE_MODEL(m_Store), &iter, NULL, idx);
	gtk_list_store_remove(m_Store, &iter);
}


void CListBox::MoveUp(int idx)
{
	if( idx <= 0 )
		return;

	GtkTreeIter iter;
	gtk_tree_model_iter_nth_child(GTK_TREE_MODEL(m_Store), &iter, NULL, idx-1);
	
	GtkTreeIter iter2 = iter;
	gtk_tree_model_iter_next(GTK_TREE_MODEL(m_Store), &iter2 );

	gtk_list_store_swap(m_Store, &iter, &iter2);
}

void CListBox::MoveDown(int idx)
{
	if( idx + 1 >= Count() )
		return;

	GtkTreeIter iter;
	gtk_tree_model_iter_nth_child(GTK_TREE_MODEL(m_Store), &iter, NULL, idx);

	GtkTreeIter iter2 = iter;
	gtk_tree_model_iter_next(GTK_TREE_MODEL(m_Store), &iter2 );

	gtk_list_store_swap(m_Store, &iter, &iter2);
}


int CListBox::GetCurSel()
{
	GtkTreeSelection* sel = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_Widget));
	GtkTreeModel* model;
	GList* list = gtk_tree_selection_get_selected_rows( sel, &model);
	if( list )
	{
		GtkTreePath* path = (GtkTreePath*)list->data;
		int* pidx = gtk_tree_path_get_indices( path );
		int idx = pidx ? *pidx : -1;
		g_list_foreach(list, (GFunc)gtk_tree_path_free, NULL);
		g_list_free(list);
		return idx;
	}
	return -1;
}


void CListBox::SetItemText(int idx, string text)
{
	GtkTreeIter iter;
	gtk_tree_model_iter_nth_child(GTK_TREE_MODEL(m_Store), &iter, NULL, idx);
	gtk_list_store_set(m_Store, &iter, 0, (GValue*)text.c_str(), -1);
}

string CListBox::GetItemText(int idx)
{
	gchar* ptext = NULL;
	GtkTreeIter iter;
	gtk_tree_model_iter_nth_child(GTK_TREE_MODEL(m_Store), &iter, NULL, idx);
	gtk_tree_model_get( GTK_TREE_MODEL(m_Store), &iter, 0, &ptext, -1);
	string text(ptext);
	if(ptext)
		g_free(ptext);
	return text;
}

void CListBox::SetCurSel(int idx)
{
	GtkTreeSelection* sel = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_Widget));
	GtkTreeIter iter;
	gtk_tree_model_iter_nth_child(GTK_TREE_MODEL(m_Store), &iter, NULL, idx);
	gtk_tree_selection_select_iter(sel, &iter);
}
