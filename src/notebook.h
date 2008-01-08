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

#ifndef NOTEBOOK_H
#define NOTEBOOK_H

#ifdef __GNUG__
  #pragma interface "notebook.h"
#endif

#include <widget.h>
#include <string>

using namespace std;

/**
@author PCMan
*/
class CNotebook : public CWidget
{
public:
    CNotebook();

    ~CNotebook();
//    void InsertPage(int pos, CWidget* page, string title);
    int AddPage( CWidget* page, string title, GdkPixbuf* icon = NULL);
    void RemovePage( int pos ){	gtk_notebook_remove_page(GTK_NOTEBOOK(m_Widget), pos);	}

    void SetCurPage(int i)
    { gtk_notebook_set_current_page((GtkNotebook*)m_Widget, i); }

    int GetCurPage(){ return gtk_notebook_get_current_page((GtkNotebook*)m_Widget); }

    int GetPageCount()	{	return gtk_notebook_get_n_pages((GtkNotebook*)m_Widget);	}

    void NextPage()	{	gtk_notebook_next_page((GtkNotebook*)m_Widget);	}
    void PrevPage()	{	gtk_notebook_prev_page((GtkNotebook*)m_Widget);	}
    void SetPageTitle(CWidget* page, string title);
    void HideTabs()	{	gtk_notebook_set_show_tabs((GtkNotebook*)m_Widget, false); }
    void ShowTabs()	{	gtk_notebook_set_show_tabs((GtkNotebook*)m_Widget, true); }
    
};

#endif
